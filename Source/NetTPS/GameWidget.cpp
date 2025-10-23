#include "GameWidget.h"

#include "ChatWidget.h"
#include "PlayerInfoWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Components/VerticalBox.h"

void UGameWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//	Chat Enter Input Call
	//	build.cs : "SlateCore" Module Addon
	edit_Chat->OnTextCommitted.AddDynamic(this, &UGameWidget::OnTextboxCommitted);
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
		UE_LOG(LogTemp, Warning, TEXT("%s"), *text.ToString());
		
		//	Create Content UI , Add Content
		TObjectPtr<UChatWidget> chat = CreateWidget<UChatWidget>(GetWorld(), ChatWidgetClass);
		chat->SetContent(text.ToString());
		scroll_Chat->AddChild(chat);

		//	edit_Chat Clear
		edit_Chat->SetText(FText());
	}
	else if (commitMethod == ETextCommit::OnCleared)
	{
		//	Force edit_Chat Enabled
		edit_Chat->SetFocus();
	}
}
