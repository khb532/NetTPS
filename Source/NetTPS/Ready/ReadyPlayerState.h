#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ReadyPlayerState.generated.h"

UCLASS()
class NETTPS_API AReadyPlayerState : public APlayerState
{
	GENERATED_BODY()
	
/* Method */
public:
	//	캐릭터 선택 관련 RPC
	UFUNCTION(Server, Reliable)
	void ServerRPC_SelectPlayer(int32 idx_player);

	//	서버에서 모든 클라에 '누가 무슨 캐릭터를 선택했는지' 전파
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_SelectPlayer(int32 idx_player);

	//	To Client, Create Ready Widget
	UFUNCTION(Client, Reliable)
	void ClientRPC_MakeReadyUI();


private:




	
/* Field*/
public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AReadyPlayer> m_ReadyPlayer_Class;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<class AReadyPlayer> m_pReadyPlayer;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UReadyWidget> m_ReadyWidget;

private:
	void Func(void);
	
};
