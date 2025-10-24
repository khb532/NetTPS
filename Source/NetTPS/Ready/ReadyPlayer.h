#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ReadyPlayer.generated.h"

UCLASS()
class NETTPS_API AReadyPlayer : public AActor
{
	GENERATED_BODY()

public:
	AReadyPlayer();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
