// Copyright Epic Games, Inc. All Rights Reserved.


#include "NetTPSPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "NetTPS.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/SpectatorPawn.h"
#include "Widgets/Input/SVirtualJoystick.h"

void ANetTPSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// only spawn touch controls on local player controllers
	if (SVirtualJoystick::ShouldDisplayTouchInterface() && IsLocalPlayerController())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);

		} else {

			UE_LOG(LogNetTPS, Error, TEXT("Could not spawn mobile controls widget."));
		}

	}
}

void ANetTPSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!SVirtualJoystick::ShouldDisplayTouchInterface())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
}

void ANetTPSPlayerController::Respawn()
{
	// 현재 Possess Pawn
	APawn * pawn = GetPawn();
	// GameMode
	AGameModeBase * gm = GetWorld()->GetAuthGameMode();

	UnPossess();

	// Spectator Pawn Destroy
	pawn->Destroy();

	// 다시 Default Pawn Spawn, Possess
	gm->RestartPlayer(this);
}

void ANetTPSPlayerController::ServerRPC_ChangeToSpectator_Implementation()
{
	// 현재 Possess Pawn
	APawn * pawn = GetPawn();
	// GameMode
	AGameModeBase * gm = GetWorld()->GetAuthGameMode();

	UnPossess();
	
	// Spectator Pawn Spawn
	ASpectatorPawn * spectator = GetWorld()->SpawnActor<ASpectatorPawn>(gm->SpectatorClass, pawn->GetTransform());
	// Spawned Spectator Possess
	Possess(spectator);
	// NetPlayer Destroy
	pawn->Destroy();

	FTimerHandle RespawnTimer;
	GetWorldTimerManager().SetTimer(RespawnTimer, this, &ANetTPSPlayerController::Respawn, 5, false);
}
