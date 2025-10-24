#include "NetPlayerState.h"

#include "GameWidget.h"
#include "NetGameState.h"

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

void ANetPlayerState::ServerRPC_SendChat_Implementation(const FString& chat)
{
	//	Chat format = Name : Content
	FString sendchat = FString::Printf(TEXT("%s : %s"), *GetPlayerName(), *chat);

	//	Muticast To All Client
	MulticastRPC_SendChat(sendchat);
}

void ANetPlayerState::MulticastRPC_SendChat_Implementation(const FString& chat)
{
	//	Get GameState
	TObjectPtr<ANetGameState> gs = Cast<ANetGameState>(GetWorld()->GetGameState());
	//	GameUI : AddChat call
	gs->GameUI->AddChat(chat);
}
