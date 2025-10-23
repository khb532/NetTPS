#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NetPlayerState.generated.h"

//	Name Update Delegate
DECLARE_DELEGATE_OneParam(FUpdateName, FString);
//	Score Update Delegate
DECLARE_DELEGATE_OneParam(FUpdateScroe, int32);


UCLASS()
class NETTPS_API ANetPlayerState : public APlayerState
{
	GENERATED_BODY()

/* Method */
public:

	virtual void OnRep_PlayerName() override;

	virtual void OnRep_Score() override;


private:




/* Field */
public:
	//	Name Delegate
	FUpdateName OnUpdateName;
	//	Score Delegate
	FUpdateScroe OnUpdateScore;



private:





	
};
