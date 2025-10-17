
#include "NetGameState.h"

#include "NetPlayer.h"
#include "Containers/ContainerAllocationPolicies.h"

void ANetGameState::AddPlayer(class ANetPlayer* Player)
{
	// Add Player
	AllPlayers.Add(Player);		// PlayerArray : 엔진제공

	// if Added Player = Server
	if (Player->IsLocallyControlled())
	{
		// Set Initial Turn
		ChangeTurn();
	}

	CalcPos(Player);
}

void ANetGameState::ChangeTurn()
{
	// CurrentTurnIdx != -1
	if (CurrentTurnIdx != -1)
		AllPlayers[CurrentTurnIdx]->bCanMakeCube = false;

	// Next Turn
	CurrentTurnIdx = (CurrentTurnIdx + 1) % AllPlayers.Num();

	// CurrentIdx Player -> CanMakeCube
	AllPlayers[CurrentTurnIdx]->bCanMakeCube = true;
}

void ANetGameState::CalcPos(class ANetPlayer* Player)
{
	// Location Num
	int32 cnt = 3;

	// 360 degrees devide cnt
	float angle = 360.f / cnt;

	// degree -> radian
	float rad = FMath::DegreesToRadians(PosIdx * angle);

	// 원점기준 radian 각도로 회전된 위치
	FVector dir = FVector(FMath::Sin(rad), FMath::Cos(rad), 0);
	
	// dir 방향으로 400 거리
	FVector pos = Player->GetActorLocation() + dir * 400;
	
	// Player를 pos위치로
	Player->SetActorLocation(pos);

	PosIdx++;
}
