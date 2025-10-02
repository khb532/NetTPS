#include "NetPlayer.h"

#include "EnhancedInputComponent.h"
#include "Gun.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

ANetPlayer::ANetPlayer()
{
	GunComp = CreateDefaultSubobject<USceneComponent>(TEXT("GunComponent"));
	GunComp->SetupAttachment(GetMesh(), TEXT("weapon_l"));
	GunComp->SetRelativeLocation((FVector(0, 7.f, 5.5f)));

	CameraBoom->SetRelativeLocation(CameraBoomLocationWithoutGun);
}

void ANetPlayer::BeginPlay()
{
	Super::BeginPlay();

	// Level에 있는 모든 총을 찾자
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGun::StaticClass(), AllGun);
	
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
}

void ANetPlayer::DettachGun(AGun* ptr)
{
	hasGun = false;
	UStaticMeshComponent* mesh = ptr->GetComponentByClass<UStaticMeshComponent>();
	mesh->SetSimulatePhysics(true);

	ptr->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	ChangeCameraBoomSetting();
}

void ANetPlayer::Fire()
{
	if (!hasGun) return;
	if (bReloading) return;

	// 총알이 없으면 탈출
	if (OwnGun->GetBulletCount() <= 0) return;
	// Fire Anim Play
	PlayAnimMontage(PlayerMontage, 1, FName(TEXT("Fire")));

	OwnGun->PopBullet();
	UE_LOG(LogTemp, Warning, TEXT("현재 잔탄 : %d"), OwnGun->GetBulletCount());
}

void ANetPlayer::Reload()
{
	if (!hasGun) return;
	if (OwnGun->IsFillBullet()) return;
	if (bReloading) return;
	bReloading = true;
	// Reload Anim Play
	PlayAnimMontage(PlayerMontage, 1, FName(TEXT("Reload")));

}

void ANetPlayer::OnReloadComplete()
{
	OwnGun->FillBullet();
	UE_LOG(LogTemp, Warning, TEXT("Reload"));
	UE_LOG(LogTemp, Warning, TEXT("현재 잔탄 : %d"), OwnGun->GetBulletCount());
	bReloading = false;
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

