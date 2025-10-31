#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ReadyGameMode.generated.h"

UCLASS()
class NETTPS_API AReadyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	
};
