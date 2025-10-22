#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyWidget.generated.h"

class UScrollBox;
class UTextBlock;
class USlider;
class UEditableTextBox;
class UButton;
class UWidgetSwitcher;
class UNetGameInstance;

UCLASS()
class NETTPS_API ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()
	
/* Method */
public:
	virtual void NativeConstruct() override;

	//	세션 정보를 받아서 SessionInfoWidget 만드는 함수
	//	+ NetGameInstance, OnFindComplete Delegate에 등록할 함수
	UFUNCTION()
	void OnFindComplete(int32 Idx, FString SessionName);

	UFUNCTION()
	void OnClickGoCreate();
	
	UFUNCTION()
	void OnClickGoFind();

	UFUNCTION()
	void OnClickCreate();

	// Session Name Changed Call
	UFUNCTION()
	void OnValueChangedSessionName(const FText& Text);
	
	// Slider Changed Call
	UFUNCTION()
	void OnValueChangedPlayerCount(float Value);

	UFUNCTION()
	void OnClickFind();

	UFUNCTION()
	void OnClickBack();
	
protected:


	
private:


	
/* Field */
public:
	// Game Instance
	UPROPERTY()
	TObjectPtr<UNetGameInstance> GameInstance;

	// Widget Switcher
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWidgetSwitcher> WidgetSwitcher;

	// Main Lobby
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Btn_GoCreate;

	// Create Session Move Button
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Btn_GoFind;
	
	//	Edit Session Name
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UEditableTextBox> EditSessionName;

	//	Slider Player Count
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USlider> SliderPlayerCount;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TxtPlayerCount;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Btn_Create;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UScrollBox> Scroll_SessionList;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Btn_Find;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class USessionInfoWidget> SessionInfoWidgetClass;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Txt_Find;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Btn_BackFromCreate;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Btn_BackFromFind;
	
protected:

	

private:


	
};
