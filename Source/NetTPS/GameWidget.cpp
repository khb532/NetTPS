#include "GameWidget.h"

#include "ChatWidget.h"
#include "DetailTreeNode.h"
#include "NetPlayerState.h"
#include "NetTPSPlayerController.h"
#include "PlayerInfoWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Components/VerticalBox.h"

void UGameWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//	Chat Enter Input Call
	//	build.cs : "SlateCore" Module Addon
	edit_Chat->OnTextCommitted.AddDynamic(this, &UGameWidget::OnTextboxCommitted);

	//	Border OnClick Event Addon
	border_Empty->OnMouseButtonDownEvent.BindDynamic(this, &UGameWidget::OnPointerEvent);

	// UPROPERTY 에서 meta = (BindWidget) 을 생성자에서
	Btn_Retry = Cast<UButton>(GetWidgetFromName(TEXT("Btn_Retry")));

	// Btn_Retry On Clicked Callback
	Btn_Retry->OnClicked.AddDynamic(this/*현재클래스위치의*/, &UGameWidget::OnRetry);
	
}

void UGameWidget::AddPlayerInfo(TObjectPtr<ANetPlayerState> PlayerState)
{
	//	Create PlayerInfoWidget
	UPlayerInfoWidget* info = CreateWidget<UPlayerInfoWidget>(GetWorld(), PlayerInfoWidgetClass);
	
	//	Add to Player Info List
	PlayerInfoList->AddChildToVerticalBox(info);
	
	info->Init(PlayerState);
}

void UGameWidget::OnTextboxCommitted(const FText& text, ETextCommit::Type commitMethod)
{

	if (commitMethod == ETextCommit::OnEnter)
	{
		//	AddChat(text.ToString());
		//	Send Chat to Server
		//	Get My PlayerState
		TObjectPtr<APlayerController> pc = GetWorld()->GetFirstPlayerController();
		TObjectPtr<ANetPlayerState> ps = pc->GetPlayerState<ANetPlayerState>();
		ps->ServerRPC_SendChat(text.ToString());

		//	edit_Chat Clear
		edit_Chat->SetText(FText());
	}
	else if (commitMethod == ETextCommit::OnCleared)
	{
		//	Force edit_Chat Enabled
		edit_Chat->SetFocus();
	}
}

void UGameWidget::AddChat(FString text)
{
	//	Current Scroll offset
	float scroll_offset = scroll_Chat->GetScrollOffset();
	//	End Scroll offset
	float endscroll_offset = scroll_Chat->GetScrollOffsetOfEnd();

	UE_LOG(LogTemp, Warning, TEXT("cur : %f, end : %f"), scroll_offset, endscroll_offset);
	//	Create Content UI , Add Content
    TObjectPtr<UChatWidget> chat = CreateWidget<UChatWidget>(GetWorld(), ChatWidgetClass);
    chat->SetContent(text);
    scroll_Chat->AddChild(chat);

	//	If Scroll loc = End
	if (scroll_offset == endscroll_offset)
	{
		//	여러줄의 채팅일경우, 첫줄의 위치만 인식하는 문제
		//	ScrollToEnd 를 0.1 Delay
		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle, [this]()
		{
			//	Set End scroll
			scroll_Chat->ScrollToEnd();
		}, 0.01f, false);
	}
}

FEventReply UGameWidget::OnPointerEvent(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
	FEventReply Reply;
	//	InputMode : GameOnly
	TObjectPtr<APlayerController> pc = GetWorld()->GetFirstPlayerController();
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(pc);

	//	MouseCursor Disable
	pc->SetShowMouseCursor(false);

	
	return UWidgetBlueprintLibrary::Handled();
}

void UGameWidget::OnRetry()
{
	// Observer Mode
	ANetTPSPlayerController* pc = Cast<ANetTPSPlayerController>(GetWorld()->GetFirstPlayerController());

	// To SERVER : Change Spectator Req
	pc->ServerRPC_ChangeToSpectator();

	pc->SetShowMouseCursor(false);

	/*
	// MainUI Remove
	RemoveFromParent();

	Visible Off 로 대체
	*/
	ShowBtnRetry(false);
	
}

void UGameWidget::ShowBtnRetry(bool visible)
{
	// Show Button
	Btn_Retry->SetVisibility(visible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}