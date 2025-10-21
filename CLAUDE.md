# CLAUDE.md

이 파일은 Claude Code(claude.ai/code)가 이 저장소에서 작업할 때 참고할 가이드를 제공합니다.

**언어**: Claude Code는 모든 응답을 한국어로 제공해야 합니다.

## 프로젝트 개요

NetTPS는 **Unreal Engine 5.6** 기반 멀티플레이어 TPS 프로젝트로, 여러 게임 시스템 변형을 포함합니다:
- **Variant_Combat**: 콤보/차지 공격이 있는 근접 전투와 StateTree AI
- **Variant_Platforming**: 다단 점프, 벽 점프, 대시 등 고급 이동
- **Variant_SideScrolling**: 소프트 플랫폼이 있는 2D 플랫포머 메카닉
- **Base Multiplayer**: 총 전투, 턴 기반 큐브 생성, Steam 로비 연동

**언어**: 모든 커밋 메시지와 주석은 한국어

## 빌드 & 개발

### 빠른 시작
```bash
# 에디터 실행 (독립 게임 인스턴스)
NetTPS.bat

# Rider IDE에서 개발
# 구성: NetTPSEditor | Win64 | Development
# F5 눌러서 PIE(Play In Editor)로 디버그
```

### 빌드 구성
- **모듈**: `NetTPS` (Source/NetTPS/NetTPS.Build.cs)
- **타겟**: `NetTPS.Target.cs` (배포용), `NetTPSEditor.Target.cs` (에디터용) - **절대 수정 금지**
- **의존성**: Core, Engine, EnhancedInput, AIModule, StateTreeModule, UMG, OnlineSubsystem, OnlineSubsystemSteam

### 멀티플레이어 테스트
- PIE(Play In Editor)에서 여러 프로세스 인스턴스로 테스트
- Steam 연동은 AppID 480(Spacewar) 사용
- Steam 없으면 자동으로 LAN 폴백 활성화 (NULL subsystem 감지)

## 아키텍처

### 네트워킹 & 복제

**세션 관리** (`NetGameInstance`):
- `CreateMySession(DisplayName, PlayerCount)` - Steam 로비 또는 LAN 세션 생성
- Steam vs NULL subsystem 자동 감지하여 LAN 폴백
- `DefaultEngine.ini`에서 설정: SteamNetDriver, IpNetDriver 폴백

**서버 권한 패턴**:
```cpp
if (HasAuthority())          // 서버 전용 로직
if (IsLocallyControlled())   // 소유 클라이언트 예측
```

**RPC 흐름 예시** (총 발사):
```
Fire() [클라이언트]
  → ServerRPC_Fire() [서버에서 검증]
  → MulticastRPC_FiringAction(bHit, HitInfo, ComboCount) [모든 클라이언트]
```

**복제 설정**:
```cpp
GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)
{
    DOREPLIFETIME(ANetPlayer, OwnGun);      // 총 소유 상태
    DOREPLIFETIME(ANetPlayer, bCanMakeCube); // 턴 플래그
}
```

### 캐릭터 시스템 계층

```
ANetTPSCharacter (기본)
├── CameraBoom, FollowCamera
├── Enhanced Input: Move, Look, Jump
└── 메서드: DoMove(), DoLook(), DoJumpStart()

ANetPlayer (멀티플레이어 총 전투)
├── 총 시스템: TakeGun, Fire, Reload (Server/Multicast RPC)
├── 턴 기반 큐브: MakeCube, bCanMakeCube
├── UI: MainWidget (HUD), CompHp (체력바)
└── 사망/리스폰: 관전자 모드 전환

ACombatCharacter (근접 전투)
├── 인터페이스: ICombatAttacker, ICombatDamageable
├── 콤보 시스템: AnimNotify 타이밍을 사용하는 ComboAttackMontage
├── 차지 공격: 홀드-릴리즈 방식, 차지 루프
├── 데미지: Sphere trace → KnockbackImpulse + LaunchImpulse
└── 사망: Ragdoll → RespawnTimer → RespawnCharacter()

APlatformingCharacter (고급 이동)
├── 다단 점프: 일반 → 더블 → 벽 점프
├── Coyote time (관대한 점프)
├── 애니메이션 몽타주가 있는 대시
└── 트레이스로 벽 감지

ASideScrollingCharacter (2D 플랫포머)
├── 제한된 이동 (좌/우만)
├── 소프트 플랫폼 (통과 가능)
└── 상호작용 시스템 (200cm 반경)
```

### 전투 시스템

**콤보 흐름**:
1. 플레이어가 공격 누름 → `DoComboAttackStart()`
2. `ComboAttack()`이 `ComboAttackMontage[ComboCount section]` 재생
3. `AnimNotify_CheckCombo`가 허용 시간 내 입력 캐시 확인
4. 눌렸으면 `ComboCount` 증가, 아니면 초기화
5. `AttackMontageEnded` 델리게이트가 상태 초기화

**차지 공격 흐름**:
1. 공격 홀드 → `ChargedAttackPressed()`가 `bIsChargingAttack = true` 설정
2. `ChargeLoopSection` 재생 (루프 몽타주)
3. `AnimNotify_CheckChargedAttack`가 홀드하는 동안 루프
4. 릴리즈 → `ChargedAttackReleased()`가 `ChargeAttackSection`으로 점프

**데미지 적용**:
```cpp
DoAttackTrace(FName DamageSourceBone)
{
    // 본에서 전방으로 Sphere trace
    FVector TraceEnd = Start + (Forward * MeleeTraceDistance);
    SweepMultiByObjectType(..., MeleeTraceRadius);

    // ICombatDamageable 타겟에 데미지 적용
    Target->ApplyDamage(MeleeDamage, this, ImpactPoint, Impulse);
}
```

### 게임 스테이트 & 턴 시스템

**ANetGameState**:
- `AllPlayers: TArray<ANetPlayer*>` - 연결된 플레이어들
- `CurrentTurnIdx: int32` - 활성 턴 인덱스
- `ChangeTurn()` - 다음 플레이어로 순환 (서버 권한)
- `CalcPos(ANetPlayer*)` - 스폰 위치 계산

**턴 기반 메카닉**:
- '1' 키 누름 → `MakeCube()` → `ServerRPC_MakeCube()`
- 서버가 `bCanMakeCube` 플래그 검증
- 큐브 스폰하고 `ChangeTurn()` 호출

### UI 시스템 (UMG)

**UMainWidget** (`Source/NetTPS/Private/UI/MainWidget.cpp`):
```cpp
UPROPERTY(meta=(BindWidget))
UHorizontalBox* MagazineBox;  // 탄약 디스플레이

// 메서드
AddBullet(int32 count)        // 탄약 위젯 추가
PopBullet()                   // 탄약 하나 제거
ShowCrosshair(bool)           // 조준점 토글
ShowDamageUI()                // 붉은 데미지 플래시
```

**위젯 컴포넌트**:
- `CompHp` - 빌보드 체력바 (캐릭터 위에 추적)
- `BulletWidget` - 개별 탄약 UI 요소

### 애니메이션 시스템

**NetPlayerAnim** (Animation Blueprint):
```cpp
// ABP에 동기화되는 프로퍼티
Velocity, isAir, GroundSpeed
DirH, DirV        // 이동 방향 블렌딩
isArmed, isDie
PitchAngle, YawAngle

// AnimNotify 콜백
AnimNotify_OnReloadComplete()
AnimNotify_CheckCombo()
AnimNotify_DoAttackTrace()
```

**몽타주 시스템**:
- `PlayerMontage` - 발사, 재장전 애니메이션
- `ComboAttackMontage` - 콤보 단계마다 여러 섹션
- `ChargedAttackMontage` - 차지 루프 + 공격 섹션

### 입력 시스템 (Enhanced Input)

**캐릭터별 입력 액션**:
- **Base**: Move, Look, Jump, MouseLook
- **NetPlayer**: + TakeGun, Fire, Reload
- **CombatCharacter**: + ComboAttack, ChargedAttack
- **PlatformingCharacter**: + Dash
- **SideScrollingCharacter**: + Interact, Drop

**바인딩 패턴**:
```cpp
UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AClass::Move);
```

## 프로젝트 구조

```
Source/NetTPS/
├── Public/<Domain>/
│   ├── Combat/          # CombatCharacter, CombatEnemy, interfaces
│   ├── Platforming/     # PlatformingCharacter
│   ├── SideScrolling/   # SideScrollingCharacter
│   ├── AI/              # CombatAIController, StateTree utilities
│   ├── Gameplay/        # NetPlayer, NetActor, NetGameState
│   ├── Interfaces/      # ICombatAttacker, ICombatDamageable
│   └── UI/              # MainWidget, HPBar
└── Private/<Domain>/    # Public 구조 미러링

Content/
├── Net/                 # 로비/네트워킹 맵
├── ThirdPerson/         # 기본 삼인칭 게임플레이
├── Variant_Combat/      # 전투 시스템 블루프린트
├── Variant_Platforming/
├── Variant_SideScrolling/
├── Characters/          # 모델, 애니메이션
├── Input/               # 입력 액션, 매핑
└── UI/                  # 위젯 블루프린트

Config/
└── Default*.ini         # Engine, Game, Input 설정
```

## 코딩 컨벤션 (AGENTS.md)

**네이밍**:
- **클래스**: UE 접두사를 사용한 PascalCase (`ANetPlayer`, `UMainWidget`, `FLinearColor`)
- **변수**: PascalCase
- **불리언**: `b` 접두사 (`bIsAlive`, `bCanAttack`, `bHasGun`)
- **포인터**: `p` 접두사 또는 PascalCase (`pActor`, `PlayerController`)

**파일 구성**:
- 클래스당 `.h/.cpp` 한 쌍
- 파일명은 클래스명과 정확히 일치
- 헤더는 `Public/<Domain>/`, 구현은 `Private/<Domain>/`

**매크로**: `UCLASS()`, `UPROPERTY()`, `UFUNCTION()`, `DOREPLIFETIME()`

## 커밋 가이드라인

**형식**: `[PREFIX] 한국어 설명 한 줄`

**접두사**:
- `[FEAT]` - 완성된 새 기능 (게임 플레이 흐름 통합)
- `[WIP]` - 구현 중인 기능 (빌드 가능 여부 무관)
- `[DOCS]` - 문서만 수정 (코드 변경 없음)
- `[FIX]` - 기존 `[FEAT]` 기능 수정/보완

**예시**:
```
[FEAT] Steam Online Subsystem 연동 및 로비 시스템 구현
[FEAT] 턴 기반 큐브 생성과 사망 리스폰 구현
[FIX] 넉백 계산 오버플로우 수정
[DOCS] CLAUDE.md 작성
```

## 주요 파일 참조

**네트워킹 진입점**:
- `Source/NetTPS/Public/Gameplay/NetGameInstance.h` - 세션 관리
- `Source/NetTPS/Public/Gameplay/NetPlayer.h` - RPC가 있는 멀티플레이어 캐릭터
- `Source/NetTPS/Public/Gameplay/NetGameState.h` - 턴 기반 게임 스테이트

**전투 시스템**:
- `Source/NetTPS/Public/Variant_Combat/CombatCharacter.h` - 근접 전투 아키텍처
- `Source/NetTPS/Public/Variant_Combat/CombatEnemy.h` - AI 적
- `Source/NetTPS/Public/Interfaces/ICombatAttacker.h` - 전투 인터페이스

**설정**:
- `Config/DefaultEngine.ini` - Steam 연동, 네트워크 드라이버
- `Config/DefaultGame.ini` - 게임 인스턴스 클래스
- `Source/NetTPS/NetTPS.Build.cs` - 모듈 의존성

**개발**:
- `NetTPS.bat` - 빠른 에디터 실행기

## 자주 쓰는 패턴

**서버 권한 검증**:
```cpp
void ANetPlayer::ServerRPC_Fire_Implementation()
{
    if (HasAuthority() == false) return;
    // 검증 및 실행
}
```

**OnRep 콜백이 있는 복제**:
```cpp
UPROPERTY(ReplicatedUsing=OnRep_Color)
FLinearColor MColor;

void OnRep_Color() { /* 머티리얼 업데이트 */ }
```

**AnimNotify → Combat Interface**:
```cpp
// AnimNotify 안에서
if (ICombatAttacker* Attacker = Cast<ICombatAttacker>(Character))
{
    Attacker->DoAttackTrace(BoneName);
}
```

**델리게이트 패턴**:
```cpp
FOnMontageEnded BlendOutDelegate;
BlendOutDelegate.BindUObject(this, &AClass::OnMontageEnded);
PlayAnimMontage(Montage, 1.f, StartSection);
```
