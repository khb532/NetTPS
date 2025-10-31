#include "Ready/ReadyGameMode.h"

#include "ReadyPlayerState.h"

void AReadyGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	//	접속한 클라에게 UI 만들라고 전파
	TObjectPtr<AReadyPlayerState> ps = NewPlayer->GetPlayerState<AReadyPlayerState>();

	if (ps)
	{
		ps->ClientRPC_MakeReadyUI();
	}
}
