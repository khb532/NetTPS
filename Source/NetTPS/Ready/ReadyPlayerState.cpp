#include "ReadyPlayerState.h"

#include "ReadyPlayer.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/KismetMathLibrary.h"
#include "Ready/ReadyWidget.h"

void AReadyPlayerState::ServerRPC_SelectPlayer_Implementation(int32 idx_player)
{
	//	All Client, idx_player 전달
	MulticastRPC_SelectPlayer(idx_player);
}

void AReadyPlayerState::MulticastRPC_SelectPlayer_Implementation(int32 idx_player)
{
	//	Create ReadyPlayer
	//	idx_player , Mesh 설정
	if (m_pReadyPlayer == nullptr)
	{
		m_pReadyPlayer = GetWorld()->SpawnActor<AReadyPlayer>(m_ReadyPlayer_Class);
	}
	else
	{
		m_pReadyPlayer->SetMesh(idx_player);
	}
}

void AReadyPlayerState::ClientRPC_MakeReadyUI_Implementation()
{
	CreateWidget<UReadyWidget>(GetWorld(), m_ReadyWidget);
}

void AReadyPlayerState::Func()
{
	// 타겟을 향하는 방향
	FVector toTarget = boss->target->GetActorLocation() - boss->GetActorLocation();

	// 수직 성분을 0으로
	toTarget.Z = 0;
	toTarget.Normalize();

	// 타겟을 바라보는 각도
	FRotator rot = UKismetMathLibrary::MakeRotFromZ(toTarget, FVector::UpVector);
	boss->SetActorRotation(rot);
}