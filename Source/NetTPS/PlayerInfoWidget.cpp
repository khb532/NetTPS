#include "PlayerInfoWidget.h"

#include "NetPlayerState.h"
#include "Components/TextBlock.h"

void UPlayerInfoWidget::Init(TObjectPtr<class ANetPlayerState> PlayerState)
{
	//	AddOn Delegate
	PlayerState->OnUpdateName.BindUObject(this, &UPlayerInfoWidget::UpdateName);
	PlayerState->OnUpdateScore.BindUObject(this, &UPlayerInfoWidget::UpdateScore);
}

void UPlayerInfoWidget::UpdateName(FString Name)
{
	txt_Name->SetText(FText::FromString(Name));
}

void UPlayerInfoWidget::UpdateScore(int32 Score)
{
	txt_Score->SetText(FText::AsNumber(Score));
}
