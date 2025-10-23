#include "NetPlayerState.h"

void ANetPlayerState::OnRep_PlayerName()
{
	Super::OnRep_PlayerName();

	//	Noti Updated Name To PlayerInfoWidget
	OnUpdateName.ExecuteIfBound(GetPlayerName());
}

void ANetPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	//	Noti Updated Score To PlayerInfoWidget
	OnUpdateScore.ExecuteIfBound(GetScore());
}
