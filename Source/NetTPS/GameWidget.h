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
	//	Begin Play
	virtual void NativeConstruct() override;
	
	//	PlayerInfoWidget 추가 함수
	void AddPlayerInfo(TObjectPtr<class ANetPlayerState> PlayerState);

	//	Chat Enter Input Call
	UFUNCTION()
	void OnTextboxCommitted(const FText& text, ETextCommit::Type commitMethod);

	//	Add Chat UI
	void AddChat(FString text);

	UFUNCTION()
	FEventReply OnPointerEvent (FGeometry MyGeometry, const FPointerEvent& MouseEvent);

	UFUNCTION()
	void OnRetry();

	UFUNCTION()
	void ShowBtnRetry(bool visible);
	
private:




/* Field */
public:
	//	PlayerInfoWidget Blueprint
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UPlayerInfoWidget> PlayerInfoWidgetClass;

	//	Player List
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UVerticalBox> PlayerInfoList;

	//	Scroll Chat Box
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UScrollBox> scroll_Chat;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UEditableTextBox> edit_Chat;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UChatWidget> ChatWidgetClass;

	//	Border Empty
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UBorder> border_Empty;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UButton> Btn_Retry;
	
private:



	
};
