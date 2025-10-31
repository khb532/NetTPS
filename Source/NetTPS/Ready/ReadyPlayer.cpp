#include "ReadyPlayer.h"


AReadyPlayer::AReadyPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

	m_Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	SetRootComponent(m_Mesh);

	
}

void AReadyPlayer::BeginPlay()
{
	Super::BeginPlay();

	SetActorRotation(FRotator(0, 90, 0));
}

void AReadyPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}

void AReadyPlayer::SetMesh(int32 idx)
{
	//	idx 번째 Mesh 설정
	m_Mesh->SetSkeletalMesh(m_arr_PlayerMesh[idx]);
}

