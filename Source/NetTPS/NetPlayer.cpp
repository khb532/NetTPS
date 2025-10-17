#include "NetPlayer.h"

#include "EnhancedInputComponent.h"
#include "Gun.h"
#include "HPBar.h"
#include "Blueprint/UserWidget.h"
#include "MainWidget.h"
#include "NetGameState.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

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

	// if Server
	if (HasAuthority())
	{
		// GameState Load
		ANetGameState* GSB = Cast<ANetGameState>(GetWorld()->GetGameState());
		if (GSB != nullptr)
		{
			GSB->AddPlayer(this);
		}
	}
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
	if (GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::One))
	{
		UE_LOG(LogTemp, Warning, TEXT("1번키 누르고있음"));
	}
	// 눌렀을 때
	if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::One))
	{
		MakeCube();		
	}
	// 떼었을 때
	if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustReleased(EKeys::One))
	{
		UE_LOG(LogTemp, Warning, TEXT("1번키 떼었음"));
		
	}
	BillboardHpbar();
	PrintNetLog();
}

void ANetPlayer::MakeCube()
{
	if (!bCanMakeCube) return;
	// if Not LocalPlayer return
	if (!IsLocallyControlled()) return;
	
	ServerRPC_MakeCube();	
}

void ANetPlayer::ServerRPC_MakeCube_Implementation()
{
	GetWorld()->SpawnActor<AActor>(CubeFactory, GetActorLocation() + GetActorForwardVector() * 300.f, GetActorRotation());

	ANetGameState* GSB = Cast<ANetGameState>(GetWorld()->GetGameState());
	if (GSB != nullptr)
		GSB->ChangeTurn();
}

void ANetPlayer::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate Var
	DOREPLIFETIME(ANetPlayer, OwnGun);
	DOREPLIFETIME(ANetPlayer, bCanMakeCube);
}

void ANetPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	FString isServer = HasAuthority() ? TEXT("Server") : TEXT("Client");
	UE_LOG(LogTemp, Warning, TEXT("%s - %s - %s"), *isServer, TEXT(__FUNCTION__), *GetActorNameOrLabel());

	ClientRPC_OnPossess();
}

void ANetPlayer::AttachGun()
{
	if (OwnGun == nullptr) return;
	// physics Off
	// Attach to GunComp
	UStaticMeshComponent* mesh = OwnGun->GetComponentByClass<UStaticMeshComponent>();
	mesh->SetSimulatePhysics(false);

	OwnGun->AttachToComponent(GunComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	hasGun = true;

	ChangeCameraBoomSetting();

	if (IsLocallyControlled())
	{
		// 총알 UI를 잔탄량만큼 채우기
		MainUI->AddBullet(OwnGun->GetBulletCount());

		// crosshair on
		MainUI->ShowCrosshair(true);
	}
}

void ANetPlayer::DettachGun(AGun* ptr)
{
	hasGun = false;
	UStaticMeshComponent* mesh = ptr->GetComponentByClass<UStaticMeshComponent>();
	mesh->SetSimulatePhysics(true);

	ptr->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	ChangeCameraBoomSetting();

	OnFireComplete();
	// 만약에 내 Player 라면
	if (IsLocallyControlled())
	{
		// 총알 UI 모두 지우자
		MainUI->PopAllBullet();
		// Crosshair 비활성
		MainUI->ShowCrosshair(false);
	}
}

void ANetPlayer::DieProcess()
{
	// 움직이지 못하게 설정
	GetCharacterMovement()->DisableMovement();
	// 충돌되지 않게 설정
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	// 만약에 내 Player 라면
	if (IsLocallyControlled())
	{
		// 총을 들고 있다면 총 놓자.
		if (hasGun) TakeGun();
		// 화면 흑백 처리
		FollowCamera->PostProcessSettings.ColorSaturation = FVector4(0, 0, 0, 1);
		// 다시하기 버튼 보이게
		MainUI->ShowBtnRetry();
		// 마우스 보이게
		GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
	}
}

void ANetPlayer::ClientRPC_OnPossess_Implementation()
{
	// MainUI 생성
	MainUI = CreateWidget<UMainWidget>(GetWorld(), MainWidget);
	MainUI->AddToViewport();
	CompHp->SetVisibility(false);

	FString isServer = HasAuthority() ? TEXT("Server") : TEXT("Client");
	UE_LOG(LogTemp, Warning, TEXT("%s - %s - %s"), *isServer, TEXT(__FUNCTION__), *GetActorNameOrLabel());
}

void ANetPlayer::Fire()
{
	ServerRPC_Fire();
}

void ANetPlayer::Reload()
{
	ServerRPC_Reload();
}

void ANetPlayer::DamageProcess(float damage)
{
	if (IsLocallyControlled())
	{
		MainUI->ShowDamageUI();
	}
	
	// 내 Player 라면 mainUI 에 있는 HPBar 를 설정
	// 그렇지 않으면 compHP 에 있는 HPBar 를 설정
	UHPBar* hpBar = IsLocallyControlled() ? MainUI->HpBarUI : Cast<UHPBar>(CompHp->GetWidget());
	
	// 머리 위에 있는 HPBar 갱신
	float currHP = hpBar->UpdateHP(damage);
	// 내가 컨트롤 하고 있는 Player
	
	// 죽었는지 여부 설정
	bIsDead = currHP <= 0;
	if (bIsDead)
	{
		// Controller UnPossess Test
		DieProcess();
	}
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

	if (IsLocallyControlled())
	{
		// 총알UI 가득 채우기
		MainUI->AddBullet(OwnGun->GetBulletCount());
	}
}

void ANetPlayer::FiringAction()
{
	
	// 시작 지점
	FVector startPos = FollowCamera->GetComponentLocation();
	// 종료 지점
	FVector endPos = startPos + FollowCamera->GetForwardVector() * 100000;
	// 예외 옵션
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	// 부딪혔을 때 그 정보를 담을 변수
	FHitResult hitInfo;
	// LineTrace 실행
	bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, params);
	// 모든 [클라] 에게 LineTrace 결과 넘겨서 총쏘게 하자
	MulticastRPC_FiringAction(bHit, hitInfo, ComboCount);
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

void ANetPlayer::ServerRPC_Reload_Implementation()
{
	if (!hasGun) return;
	if (OwnGun->IsFillBullet()) return;
	if (bReloading) return;
	if (isFire) return;
	
	MulticastRPC_Reload();
}

void ANetPlayer::MulticastRPC_Reload_Implementation()
{
	bReloading = true;
	// Reload Anim Play
	PlayAnimMontage(PlayerMontage, 1, FName(TEXT("Reload")));
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
	// Request to Server
	ServerRPC_TakeGun();
}

void ANetPlayer::MulticastRPC_FiringAction_Implementation(bool bHit, FHitResult hitInfo, int32 combo)
{
	// Fire Anim Play
	FString FireName = FString::Printf(TEXT("Fire_%d"), ComboCount);
	PlayAnimMontage(PlayerMontage, 1, FName(FireName));

	if (OwnGun != nullptr)
		OwnGun->PopBullet();
	UE_LOG(LogTemp, Warning, TEXT("현재 잔탄 : %d"), OwnGun->GetBulletCount());
	if (MainUI != nullptr)
		MainUI->PopBullet();

	if (bHit)
	{
		// UKismetMathLibrary::GetReflectionVector()
		// 입사각
		FVector inVector = hitInfo.TraceEnd - hitInfo.TraceStart;
		// 법선벡터 (노멀벡터)
		FVector normalVector = hitInfo.Normal;
		// 반사각 계산
		float dot = FVector::DotProduct(inVector, normalVector);
		FVector outVector = inVector - 2 * dot * normalVector;
		// 반사각으로 파티클 효과가 재생되게 각도 구하자.
		FRotator rot = UKismetMathLibrary::MakeRotFromX(outVector);
		
		// 맞은 지점에 파티클 효과 표현
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitEffect, hitInfo.Location, rot);

		// 만약 맞은 Actor 가 NetPlayer 라면
		if (ANetPlayer* player = Cast<ANetPlayer>(hitInfo.GetActor()))
		{
			// 데미지 처리 하자.
			player->DamageProcess(20);
		}
	}

}

void ANetPlayer::ServerRPC_Fire_Implementation()
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

void ANetPlayer::MulticastRPC_DetachGun_Implementation(class AGun* gun)
{
	DettachGun(gun);
}

void ANetPlayer::ServerRPC_TakeGun_Implementation()
{
	if (!hasGun)
	{
		// Dead
		if (bIsDead) return;
		// 나와 가장 가까운 총의 이름을 출력
		closeidx = -1;
		// 현재 가장 가까운 거리
		float closedist = CanTakeGunRange;
	
		for (int32 i = 0; i < AllGun.Num(); i++)
		{
			// Already Owend Gun
			if ( AllGun[i]->GetOwner() != nullptr ) continue;
			
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
			OwnGun->SetOwner(this);
			AttachGun();
		}
	}
	// 재장전중 총 탈착방지
	else if ( bReloading == false)
	{
		// swap
		AGun* tmp = OwnGun;
		// OwnGun, Owner off
		OwnGun->SetOwner(nullptr);
		
		OwnGun = nullptr;
		
		// All Client Detach gun
		MulticastRPC_DetachGun(tmp);
	}
}




