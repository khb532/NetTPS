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

	// 현재 가장 가까운 거리
	float closedist = CanTakeGunRange;
	// 가까운 총의 인덱스
	int32 closeidx = -1;
	
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
		UE_LOG(LogTemp, Warning, TEXT("%f, %d"), closedist, closeidx);

}
