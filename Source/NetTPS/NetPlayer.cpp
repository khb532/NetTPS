#include "NetPlayer.h"

#include "EnhancedInputComponent.h"
#include "Gun.h"
#include "HPBar.h"
#include "Blueprint/UserWidget.h"
#include "MainWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"

ANetPlayer::ANetPlayer()
{
	GunComp = CreateDefaultSubobject<USceneComponent>(TEXT("GunComponent"));
	GunComp->SetupAttachment(GetMesh(), TEXT("weapon_l"));
	GunComp->SetRelativeLocation((FVector(0, 7.f, 5.5f)));

	CompHp = CreateDefaultSubobject<UWidgetComponent>(TEXT("HP"));
	CompHp->SetupAttachment(RootComponent);
}

void ANetPlayer::BeginPlay()
{
	Super::BeginPlay();

	// Level에 있는 모든 총을 찾자
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGun::StaticClass(), AllGun);

	CameraBoom->SetRelativeLocation(CameraBoomLocationWithoutGun);
	
	// MainUI 생성
	MainUI = CreateWidget<UMainWidget>(GetWorld(), MainWidget);
	MainUI->AddToViewport();
	MainUI->ShowCrosshair(false);

}

void ANetPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// 총 집기&놓기 Input -> 호출함수 등록
		EIC->BindAction(TakeGunAction, ETriggerEvent::Started, this, &ANetPlayer::TakeGun);
		EIC->BindAction(FireAction, ETriggerEvent::Started, this, &ANetPlayer::Fire);
		EIC->BindAction(ReloadAction, ETriggerEvent::Started, this, &ANetPlayer::Reload);
	}
}

void ANetPlayer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// 누르고 있을 때
	if (GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::J))
	{
		UE_LOG(LogTemp, Warning, TEXT("J 키 누르고있음"));
	}
	// 눌렀을 때
	if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::J))
	{
		UE_LOG(LogTemp, Warning, TEXT("J 키 눌렀었음"));
		
	}
	// 떼었을 때
	if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustReleased(EKeys::J))
	{
		UE_LOG(LogTemp, Warning, TEXT("J 키 뗐음"));
		
	}
	BillboardHpbar();
	PrintNetLog();
}

void ANetPlayer::AttachGun()
{
	// physics Off
	// Attach to GunComp
	UStaticMeshComponent* mesh = OwnGun->GetComponentByClass<UStaticMeshComponent>();
	mesh->SetSimulatePhysics(false);

	OwnGun->AttachToComponent(GunComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	hasGun = true;

	ChangeCameraBoomSetting();

	// 총알 UI를 잔탄량만큼 채우기
	MainUI->AddBullet(OwnGun->GetBulletCount());

	// crosshair on
	MainUI->ShowCrosshair(true);
}

void ANetPlayer::DettachGun(AGun* ptr)
{
	hasGun = false;
	UStaticMeshComponent* mesh = ptr->GetComponentByClass<UStaticMeshComponent>();
	mesh->SetSimulatePhysics(true);

	ptr->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	ChangeCameraBoomSetting();

	// 총알UI 전체삭제
	MainUI->PopAllBullet();

	// crosshair off
	MainUI->ShowCrosshair(false);

	OnFireComplete();
}

void ANetPlayer::Fire()
{
	if (!hasGun) return;
	if (bReloading) return;
	// 총알이 없으면 탈출
	if (OwnGun->GetBulletCount() <= 0) return;

	// 공격중
	if (isFire)
	{
		// Combo 연결
		isCombo = true;
	}
	else    // 첫 공격
	{
		isFire = true;
		// 공격 실행
		FiringAction();
	}
}

void ANetPlayer::Reload()
{
	if (!hasGun) return;
	if (OwnGun->IsFillBullet()) return;
	if (bReloading) return;
	if (isFire) return;
	bReloading = true;
	// Reload Anim Play
	PlayAnimMontage(PlayerMontage, 1, FName(TEXT("Reload")));

	
}

void ANetPlayer::DamageProcess(float damage)
{
	// 내가 컨트롤하지 않는 캐릭터만
	// 머리위의 HP bar
	UHPBar* hpbar = Cast<UHPBar>(CompHp->GetWidget());
	// 머리위의 HP bar 갱신
	float CurHp = hpbar->UpdateHP(damage);

	// 내가 컨트롤중인 Player만
	// MainUI의 HPbar 갱신
	MainUI->HpBarUI->UpdateHP(damage);

	// 사망 여부 설정
	isDead = CurHp <= 0;
}

void ANetPlayer::BillboardHpbar()
{
	// 컨트롤중인 카메라
	AActor* Camera = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);	
	// 카메라의 앞 방향의 반대로, Z벡터는 고정
	FRotator rot = UKismetMathLibrary::MakeRotFromXZ(-Camera->GetActorForwardVector(), Camera->GetActorUpVector());
	// CompHp의 회전 적용
	CompHp->SetWorldRotation(rot);
}

void ANetPlayer::PrintNetLog()
{
	// NetConnection State
	FString ConnectionString = GetNetConnection() != nullptr ? TEXT("Valid Connection") : TEXT("Invalid Connection");

	// Owner State
	FString OwnerName = GetOwner() != nullptr ? GetOwner()->GetActorNameOrLabel() : TEXT("Invalid Owner");

	// Possessing State
	FString Possessing = IsLocallyControlled() ? TEXT("Valid Possessing") : TEXT("Invalid Possessing");

	// Role
	FString RoleString = FString::Printf(TEXT("Local : %s, Remote : %s"),
		*UEnum::GetValueAsString<ENetRole>(GetLocalRole()),
		*UEnum::GetValueAsString<ENetRole>(GetRemoteRole()));
	

	FString LogString = FString::Printf(TEXT("Connection : %s\r\nOwner : %s\r\nMine : %s\r\nRole : %s"),
		*ConnectionString,
		*OwnerName,
		*Possessing,
		*RoleString);
	DrawDebugString(GetWorld(), GetActorLocation(), LogString, nullptr, FColor::Yellow, 0);
}

void ANetPlayer::OnReloadComplete()
{
	bReloading = false;
	OwnGun->FillBullet();
	UE_LOG(LogTemp, Warning, TEXT("Reload"));
	UE_LOG(LogTemp, Warning, TEXT("현재 잔탄 : %d"), OwnGun->GetBulletCount());

	// 총알UI 가득 채우기
	MainUI->AddBullet(OwnGun->GetBulletCount());
}

void ANetPlayer::FiringAction()
{
	// Fire Anim Play
	FString FireName = FString::Printf(TEXT("Fire_%d"), ComboCount);
	PlayAnimMontage(PlayerMontage, 1, FName(FireName));

	OwnGun->PopBullet();
	UE_LOG(LogTemp, Warning, TEXT("현재 잔탄 : %d"), OwnGun->GetBulletCount());

	MainUI->PopBullet();

	////////* 탄착지점 이펙트 출력 *////////
	// 시작지점
	FVector StartPos = FollowCamera->GetComponentLocation();
	// 종료지점
	FVector EndPos = StartPos + FollowCamera->GetForwardVector() * 10000;

	FCollisionQueryParams params;
	params.AddIgnoredActor(this);

	// 충돌시 정보 수신
	FHitResult HitResult;

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartPos, EndPos, ECC_Visibility, params);

	if (bHit)
	{
		// UKismetMathLibrary::GetReflectionVector();
		// 입사각
		FVector InVector = EndPos - StartPos;
		// 법선(노말)
		FVector NormalVector = HitResult.Normal;
		// 반사각 (Rotation)
		float Dot = FVector::DotProduct(InVector, NormalVector);
		FVector OutVector = InVector - 2 * Dot * NormalVector;
		FRotator Rot = UKismetMathLibrary::MakeRotFromX(OutVector);
		
		// 맞은지점 파티클 효과 생성
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitEffect, HitResult.Location, Rot);

		// 만약 맞은 Actor가 NetPlayer라면
		if ( auto Player = Cast<ANetPlayer>(HitResult.GetActor()) )
		{
			// 데미지 처리 하자.
			Player->DamageProcess(20);

			// DamgeUI on
			MainUI->ShowDamageUI();
		}
	}
}

void ANetPlayer::OnCombo()
{
	// 콤보키를 누르지 않는다면 탈출
	if (isCombo == false) return;

	// 다음 콤보 체크를 위해 isCombo = false
	isCombo = false;

	ComboCount++;

	// 공격액션 실행
	FiringAction();
}

void ANetPlayer::OnFireComplete()
{
	// 콤보관련 변수 초기화
	isFire = isCombo = false;
	ComboCount = 0;
}

void ANetPlayer::ChangeCameraBoomSetting()
{
	GetCharacterMovement()->bOrientRotationToMovement = !hasGun;
	bUseControllerRotationYaw = hasGun;
	CameraBoom->SetRelativeLocation(hasGun ? CameraBoomLocationWithGun : CameraBoomLocationWithoutGun);
	CameraBoom->TargetArmLength = hasGun ? TargetArmLengthWithGun : TargetArmLengthWithoutGun;
}


void ANetPlayer::TakeGun()
{
	if (!hasGun)
	{
		// 나와 가장 가까운 총의 이름을 출력
		closeidx = -1;
		// 현재 가장 가까운 거리
		float closedist = CanTakeGunRange;
	
		for (int32 i = 0; i < AllGun.Num(); i++)
		{
			// 현재 인덱스의 거리
			float dist = FVector::Dist(AllGun[i]->GetActorLocation(), GetActorLocation());

			// 거리가 너무 멀면 스킵
			if (dist > CanTakeGunRange) continue;
			// 허용 범위 내에 있다면
			if (dist < CanTakeGunRange)
			{
				closeidx = i;
				closedist = dist;
			}
		}
		// 어느 총도 범위내에 없는경우 제외
		if (closeidx != -1)
		{
			OwnGun = Cast<AGun>(AllGun[closeidx]);
			AttachGun();
		}
	}
	// 재장전중 총 탈착방지
	else if ( bReloading == false)
	{
		// swap
		AGun* tmp = OwnGun;
		OwnGun = nullptr;
		DettachGun(tmp);
	}

}




