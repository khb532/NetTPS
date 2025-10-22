#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyWidget.generated.h"

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
	class UNetGameInstance* GameInstance;

	// Widget Switcher
	UPROPERTY(meta=(BindWidget))
	class UWidgetSwitcher* WidgetSwitcher;

	// Main Lobby
	UPROPERTY(meta=(BindWidget))
	class UButton* Btn_GoCreate;

	// Create Session Move Button
	UPROPERTY(meta=(BindWidget))
	class UButton* Btn_GoFind;
	
	//	Edit Session Name
	UPROPERTY(meta=(BindWidget))
	class UEditableTextBox* EditSessionName;

	//	Slider Player Count
	UPROPERTY(meta=(BindWidget))
	class USlider* SliderPlayerCount;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TxtPlayerCount;

	UPROPERTY(meta=(BindWidget))
	class UButton* Btn_Create;

	UPROPERTY(meta=(BindWidget))
	class UScrollBox* Scroll_SessionList;

	UPROPERTY(meta=(BindWidget))
	class UButton* Btn_Find;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class USessionInfoWidget> SessionInfoWidgetClass;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Txt_Find;

	UPROPERTY(meta=(BindWidget))
	class UButton* Btn_BackFromCreate;
	
	UPROPERTY(meta=(BindWidget))
	class UButton* Btn_BackFromFind;
	
protected:

	

private:


	
};
