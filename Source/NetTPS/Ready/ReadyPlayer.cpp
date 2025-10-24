#include "ReadyPlayer.h"


AReadyPlayer::AReadyPlayer()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AReadyPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

void AReadyPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

