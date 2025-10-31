#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReadyWidget.generated.h"

UCLASS()
class NETTPS_API UReadyWidget : public UUserWidget
{
	GENERATED_BODY()

	/* Method */
public:
	UFUNCTION()
	void OnClickPlayer01();

	UFUNCTION()
	void OnClickPlayer02();

	UFUNCTION()
	void OnClickReady();

	UFUNCTION()
	void OnClickStart();

protected:
	virtual void NativeConstruct() override;



	/* Field */
public:
	UPROPERTY()
	TObjectPtr<class AReadyPlayerState> m_PlayerState;
	
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UButton> btn_Player01;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UButton> btn_Player02;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UButton> btn_Ready;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UButton> btn_Start;
	
};
