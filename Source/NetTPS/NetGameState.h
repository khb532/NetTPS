#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "NetGameState.generated.h"


UCLASS()
class NETTPS_API ANetGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	void AddPlayer(class ANetPlayer* Player);

	//	PlayerState 추가될 때 호출되는 함수
	//	Server, Client 모두 호출 됨
	virtual void AddPlayerState(APlayerState* PlayerState) override;

	void ChangeTurn();

	// Calc Spawn Location
	void CalcPos(class ANetPlayer* Player);
public:
	// All Player
	UPROPERTY()
	TArray<class ANetPlayer*> AllPlayers;

	int32 CurrentTurnIdx = -1;		// 큐브를 만들수있는 플레이어 Index

	// Player Spawn Location Index
	int32 PosIdx = 0;

	//	Game Widget Blueprint
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UGameWidget> GameWidgetClass;

	//	Created Game Widget Pointer
	UPROPERTY()
	TObjectPtr<class UGameWidget> GameUI;
	
};
