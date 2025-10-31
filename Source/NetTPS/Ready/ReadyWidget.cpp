#include "ReadyWidget.h"

#include "ReadyPlayerState.h"
#include "Components/Button.h"

void UReadyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TObjectPtr<APlayerController> pc = GetWorld()->GetFirstPlayerController();
	if (pc)
	{
		m_PlayerState = pc->GetPlayerState<AReadyPlayerState>();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Player controller not found"));
	}
	
	btn_Player01->OnClicked.AddDynamic(this, &UReadyWidget::OnClickPlayer01);	
	btn_Player02->OnClicked.AddDynamic(this, &UReadyWidget::OnClickPlayer02);	
	btn_Ready->OnClicked.AddDynamic(this, &UReadyWidget::OnClickReady);	
	btn_Start->OnClicked.AddDynamic(this, &UReadyWidget::OnClickStart);
}

void UReadyWidget::OnClickPlayer01()
{
	m_PlayerState->ServerRPC_SelectPlayer(0);
}

void UReadyWidget::OnClickPlayer02()
{
	m_PlayerState->ServerRPC_SelectPlayer(1);
}

void UReadyWidget::OnClickReady()
{
	
}

void UReadyWidget::OnClickStart()
{
	
}


