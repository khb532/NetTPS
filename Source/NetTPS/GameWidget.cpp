#include "GameWidget.h"
#include "PlayerInfoWidget.h"
#include "Components/VerticalBox.h"

void UGameWidget::AddPlayerInfo()
{
	//	Create PlayerInfoWidget
	UPlayerInfoWidget* info = CreateWidget<UPlayerInfoWidget>(GetWorld(), PlayerInfoWidgetClass);

	//	Add to Player Info List
	PlayerInfoList->AddChildToVerticalBox(info);
}
