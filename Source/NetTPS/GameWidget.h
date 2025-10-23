#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameWidget.generated.h"


UCLASS()
class NETTPS_API UGameWidget : public UUserWidget
{
	GENERATED_BODY()

/* Method */
public:
	//	PlayerInfoWidget 추가 함수
	void AddPlayerInfo();



private:




/* Field */
public:
	//	PlayerInfoWidget Blueprint
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UPlayerInfoWidget> PlayerInfoWidgetClass;

	//	Player List
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UVerticalBox> PlayerInfoList;

private:



	
};
