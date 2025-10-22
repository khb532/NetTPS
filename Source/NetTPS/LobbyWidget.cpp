#include "LobbyWidget.h"
#include "NetGameInstance.h"
#include "SessionInfoWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// GameInstance Load
	GameInstance = Cast<UNetGameInstance>(GetWorld()->GetGameInstance());

	// Mouse cursor on
	GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);

	// Widget Click Event Addon
	Btn_GoCreate->OnClicked.AddDynamic(this, &ULobbyWidget::OnClickGoCreate);
	Btn_GoFind->OnClicked.AddDynamic(this, &ULobbyWidget::OnClickGoFind);
	Btn_Create->OnClicked.AddDynamic(this, &ULobbyWidget::OnClickCreate);
	Btn_Find->OnClicked.AddDynamic(this, &ULobbyWidget::OnClickFind);
	Btn_BackFromCreate->OnClicked.AddDynamic(this, &ULobbyWidget::OnClickBack);
	Btn_BackFromFind->OnClicked.AddDynamic(this, &ULobbyWidget::OnClickBack);

	// Widget Value Event Addon
	EditSessionName->OnTextChanged.AddDynamic(this, &ULobbyWidget::OnValueChangedSessionName);
	SliderPlayerCount->OnValueChanged.AddDynamic(this, &ULobbyWidget::OnValueChangedPlayerCount);

	//	GameInstance : Delegate Addon
	GameInstance->OnFindComplete.BindUObject(this, &ULobbyWidget::OnFindComplete);
	
}

void ULobbyWidget::OnFindComplete(int32 Idx, FString SessionName)
{
	if (Idx == -1)
	{
		//	Set txt Find 
		Txt_Find->SetText(FText::FromString(TEXT("세션 조회")));

		//	Off button
		Btn_Find->SetIsEnabled(true);
	}
	else
	{
		//	Create SessionInfoWidget
		USessionInfoWidget* Session = CreateWidget<USessionInfoWidget>(GetWorld(), SessionInfoWidgetClass);
	
		// Created Widget -> Add on Scroll_SessionList
		Scroll_SessionList->AddChild(Session);
	
		//	Set Created Session Info
		Session->SetSessionInfo(Idx, SessionName);
	}
}

void ULobbyWidget::OnClickGoCreate()
{
	WidgetSwitcher->SetActiveWidgetIndex(1);
}

void ULobbyWidget::OnClickGoFind()
{
	WidgetSwitcher->SetActiveWidgetIndex(2);

	//	최초 조회 시작
	OnClickFind();
}

void ULobbyWidget::OnClickCreate()
{
	// Session Name
	FString SessionName = EditSessionName->GetText().ToString();
	// Player Count
	int32 PlayerCnt = SliderPlayerCount->GetValue();

	// Create Session
	GameInstance->CreateMySession(SessionName, PlayerCnt);
}

void ULobbyWidget::OnValueChangedSessionName(const FText& Text)
{
	// Text length > 0 ? Create button on : off
	Btn_Create->SetIsEnabled(Text.ToString().Len() > 0);	// Text.IsEmpty()
}

void ULobbyWidget::OnValueChangedPlayerCount(float Value)
{
	// Value -> TxtPlayerCount insert
	TxtPlayerCount->SetText(FText::AsNumber(Value));
}

void ULobbyWidget::OnClickFind()
{
	//	Session List Clear
	Scroll_SessionList->ClearChildren();

	//	Set txt Find 
	Txt_Find->SetText(FText::FromString(TEXT("Searching...")));

	//	Off button
	Btn_Find->SetIsEnabled(false);
	
	// Search Session
	GameInstance->FindOtherSession();

}

void ULobbyWidget::OnClickBack()
{
	WidgetSwitcher->SetActiveWidgetIndex(0);
}
