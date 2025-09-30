#include "NetPlayer.h"

#include "EnhancedInputComponent.h"
#include "Gun.h"
#include "Kismet/GameplayStatics.h"

ANetPlayer::ANetPlayer()
{
	GunComp = CreateDefaultSubobject<USceneComponent>(TEXT("GunComponent"));
	GunComp->SetupAttachment(GetMesh(), TEXT("weapon_l"));
	GunComp->SetRelativeLocation((FVector(0, 7.f, 5.5f)));
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
	}
}


void ANetPlayer::TakeGun()
{
	// 나와 가장 가까운 총의 이름을 출력
	float dist = 0.f;
	AGun* CloseGun = nullptr;
	
	for (AActor* i : AllGun)
	{
		float comparedist = 0.f;
		AGun* ig = Cast<AGun>(i);
		if (ig != nullptr)
		{
			if (ig == AllGun[0])
			{
				dist = comparedist;
				CloseGun = ig;
			}
			
			comparedist = FVector::Dist(ig->GetActorLocation(), GetActorLocation());			

			if (comparedist < dist)
			{
				dist = comparedist;
				CloseGun = ig;
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("%f , %s"), dist, *CloseGun->GetName());
}
