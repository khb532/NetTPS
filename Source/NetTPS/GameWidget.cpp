#include "GameWidget.h"
#include "PlayerInfoWidget.h"
#include "Components/VerticalBox.h"

void UGameWidget::AddPlayerInfo(TObjectPtr<ANetPlayerState> PlayerState)
{
	//	Create PlayerInfoWidget
	UPlayerInfoWidget* info = CreateWidget<UPlayerInfoWidget>(GetWorld(), PlayerInfoWidgetClass);
	
	//	Add to Player Info List
	PlayerInfoList->AddChildToVerticalBox(info);
	
	info->Init(PlayerState);
}
