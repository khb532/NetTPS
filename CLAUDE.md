# CLAUDE.md

이 파일은 Claude Code(claude.ai/code)가 이 저장소에서 작업할 때 참고할 가이드를 제공합니다.

**언어**: Claude Code는 모든 응답을 한국어로 제공해야 합니다.

## 프로젝트 개요

NetTPS는 **Unreal Engine 5.6** 기반 멀티플레이어 TPS 프로젝트입니다:
- 총 전투 시스템
- 턴 기반 큐브 생성
- Steam 로비 연동 (LAN 폴백 지원)
- 세션 생성/검색/참여 기능

**언어**: 모든 커밋 메시지와 주석은 한국어

**주의**: `Variant_*` 폴더의 코드는 번들 샘플 코드로 직접 관리하지 않습니다.

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

### 캐릭터 시스템

```
ANetTPSCharacter (기본 캐릭터)
├── CameraBoom, FollowCamera
├── Enhanced Input: Move, Look, Jump
└── 메서드: DoMove(), DoLook(), DoJumpStart()

ANetPlayer (멀티플레이어 총 전투)
├── 총 시스템: TakeGun, Fire, Reload (Server/Multicast RPC)
├── 턴 기반 큐브: MakeCube, bCanMakeCube
├── UI: MainWidget (HUD), CompHp (체력바)
└── 사망/리스폰: 관전자 모드 전환
```

### 총 전투 시스템

**발사 흐름**:
1. 클라이언트가 Fire() 호출
2. ServerRPC_Fire()로 서버에 요청
3. 서버가 Line Trace로 히트 검증
4. MulticastRPC_FiringAction()으로 모든 클라이언트에 재생

**재장전**:
- Reload() → ServerRPC_Reload() → 애니메이션 재생
- AnimNotify_OnReloadComplete()에서 탄약 보충

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

**UMainWidget** (인게임 HUD):
```cpp
UPROPERTY(meta=(BindWidget))
UHorizontalBox* MagazineBox;  // 탄약 디스플레이

// 메서드
AddBullet(int32 count)        // 탄약 위젯 추가
PopBullet()                   // 탄약 하나 제거
ShowCrosshair(bool)           // 조준점 토글
ShowDamageUI()                // 붉은 데미지 플래시
```

**ULobbyWidget** (로비 UI):
- WidgetSwitcher - 메인/생성/검색 화면 전환
- EditSessionName - 세션 이름 입력
- SliderPlayerCount - 플레이어 수 설정
- SessionSlotBox - 검색된 세션 목록

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
```

**몽타주 시스템**:
- `PlayerMontage` - 발사, 재장전 애니메이션

### 입력 시스템 (Enhanced Input)

**입력 액션**:
- **NetTPSCharacter**: Move, Look, Jump, MouseLook
- **NetPlayer**: + TakeGun, Fire, Reload, MakeCube

**바인딩 패턴**:
```cpp
UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AClass::DoMove);
```

## 프로젝트 구조

```
Source/NetTPS/
├── (루트)               # 직접 관리하는 코드
│   ├── NetTPSCharacter  # 기본 삼인칭 캐릭터
│   ├── NetPlayer        # 멀티플레이어 캐릭터
│   ├── NetGameInstance  # 세션 관리
│   ├── NetGameState     # 게임 스테이트
│   ├── MainWidget       # 인게임 HUD
│   ├── LobbyWidget      # 로비 UI
│   └── ...
└── Variant_*/           # 번들 샘플 코드 (직접 관리 안 함)

Content/
├── Net/                 # 로비/네트워킹 맵
│   ├── LobbyMap         # 로비 맵
│   └── UI/              # 로비 위젯 블루프린트
├── ThirdPerson/         # 기본 삼인칭 게임플레이
│   ├── Lvl_ThirdPerson  # 메인 게임 레벨
│   └── Blueprints/      # 캐릭터 블루프린트
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

**네트워킹 핵심**:
- `Source/NetTPS/NetGameInstance.h` - 세션 생성/검색/참여
- `Source/NetTPS/NetPlayer.h` - 멀티플레이어 캐릭터 (RPC)
- `Source/NetTPS/NetGameState.h` - 턴 기반 게임 스테이트

**UI**:
- `Source/NetTPS/LobbyWidget.h` - 로비 UI
- `Source/NetTPS/MainWidget.h` - 인게임 HUD

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

**위젯 바인딩 (UE5 권장)**:
```cpp
// ✅ TObjectPtr 사용 (UE5)
UPROPERTY(meta=(BindWidget))
TObjectPtr<UButton> Btn_GoCreate;

UPROPERTY(meta=(BindWidget))
TObjectPtr<UWidgetSwitcher> WidgetSwitcher;
```
