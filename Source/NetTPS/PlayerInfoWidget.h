#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class NETTPS_API UPlayerInfoWidget : public UUserWidget
{
	GENERATED_BODY()

/* Method */
public:
	//	Initialize
	void Init(TObjectPtr<class ANetPlayerState> PlayerState);

	//	Udapte Name
	UFUNCTION()
	void UpdateName(FString Name);
	
	//	Update Score
	UFUNCTION()
	void UpdateScore(int32 Score);
	

private:




	
/* Field */
public:
	//	Name
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> txt_Name;
	
	//	Score
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> txt_Score;
};
