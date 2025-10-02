#include "Gun.h"

AGun::AGun()
{
	PrimaryActorTick.bCanEverTick = true;

	FillBullet();
}

void AGun::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGun::PopBullet()
{
	CurBullet--;
}

