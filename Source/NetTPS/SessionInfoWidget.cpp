#include "SessionInfoWidget.h"
#include "NetGameInstance.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void USessionInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Btn_Join->OnClicked.AddDynamic(this, &USessionInfoWidget::OnClickJoin);
}

void USessionInfoWidget::SetSessionInfo(int32 Idx, FString SessionName)
{
	//	Set Session Order
	SessionIdx = Idx;

	//	Set Session Name
	Txt_SessionName->SetText(FText::FromString(SessionName));
}

void USessionInfoWidget::OnClickJoin()
{
	//	Load GameInstance
	UNetGameInstance* GameInstance = Cast<UNetGameInstance>(GetGameInstance());

	//	Join Session Idx
	GameInstance->JoinOtherSession(SessionIdx);
}
