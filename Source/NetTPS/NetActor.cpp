
#include "NetActor.h"
#include "NetPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ANetActor::ANetActor()
{
	PrimaryActorTick.bCanEverTick = true;
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	bReplicates = true;
}

void ANetActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ANetActor::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate 할 변수들 등록
	// #include "Net/UnrealNetwork.h"
	DOREPLIFETIME(ANetActor, RotYaw);
	
}

void ANetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PrintLog();
	FindOwner();
	Rotate();
}

void ANetActor::PrintLog()
{
	// NetConnection State
	FString ConnectionString = GetNetConnection() != nullptr ? TEXT("Valid Connection") : TEXT("Invalid Connection");

	// Owner State
	FString OwnerName = GetOwner() != nullptr ? GetOwner()->GetActorNameOrLabel() : TEXT("Invalid Owner");

	// Role
	FString RoleString = FString::Printf(TEXT("Local : %s, Remote : %s"),
		*UEnum::GetValueAsString<ENetRole>(GetLocalRole()),
		*UEnum::GetValueAsString<ENetRole>(GetRemoteRole()));
	

	FString LogString = FString::Printf(TEXT("Connection : %s\r\nOwner : %s\r\nRole : %s"),
		*ConnectionString,
		*OwnerName,
		*RoleString);
	DrawDebugString(GetWorld(), GetActorLocation(), LogString, nullptr, FColor::Yellow, 0);
}

void ANetActor::FindOwner()
{
	// Not Server
	if (HasAuthority() == false) return;

	// Search Whole NetPlayer
	TArray<AActor*> AllPlayers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANetPlayer::StaticClass(), AllPlayers);

	float CloseDist = FindDistance;
	AActor* ClosestPlayer = nullptr;

	for (int32 i = 0; i < AllPlayers.Num(); i++)
	{
		// i 번째 player
		float dist = FVector::Dist(AllPlayers[i]->GetActorLocation(), GetActorLocation());
		// 거리밖 플레이어는 skip
		if (dist > FindDistance) continue;

		// 현재 최단거리보다 측정거리가 작으면 최단거리 갱신, 최단거리플레이어 갱신
		if (dist < CloseDist)
		{
			CloseDist = dist;
			ClosestPlayer = AllPlayers[i];
		}
	}
	// Owner와 최단거리 Player가 같다면 탈출
	if (GetOwner() == ClosestPlayer) return;

	SetOwner(ClosestPlayer);
}

void ANetActor::Rotate()
{
	if (HasAuthority())		// Server
	{
		AddActorWorldRotation(FRotator(0, 50 * GetWorld()->GetDeltaSeconds(), 0));
		RotYaw = GetActorRotation().Yaw;
	}
	else    // Client
	{
		FRotator rot = GetActorRotation();
		rot.Yaw = RotYaw;
		SetActorRotation(rot);
	}
}

