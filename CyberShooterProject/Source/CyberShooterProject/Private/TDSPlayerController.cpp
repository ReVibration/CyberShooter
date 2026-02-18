// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSPlayerController.h"
#include "TDSHUDWidget.h"
#include "TDSCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void ATDSPlayerController::BeginPlay()
{
	// Call the base class BeginPlay to ensure any inherited functionality is executed
	Super::BeginPlay();
	
	// Get the name of the current level to determine which UI to show
	FString LevelName = GetWorld()->GetMapName();

	//If we're in the main menu level, show the main menu UI
	if (LevelName.Contains("MainMenulevel"))
	{
		// When inside the main menu show the main menu and set the input mode
		ShowMainMenu();
		SetMenuInputMode();
	}
	else
	{
		// When outside of the main menu show the HUD and set the input mode
		ShowHUD();
		SetGameInputMode();
	}
}

void ATDSPlayerController::ClearAllUI() 
{
	// If there is a child instance of the UI then you can remove from the parent
	if (ActiveMainMenu) { ActiveMainMenu->RemoveFromParent(); ActiveMainMenu = nullptr; }
	if (ActiveHUD) { ActiveHUD->RemoveFromParent();      ActiveHUD = nullptr; }
	if (ActiveGameOver) { ActiveGameOver->RemoveFromParent(); ActiveGameOver = nullptr; }
}

void ATDSPlayerController::ShowMainMenu()
{
	// Make sure you have a valid reference
	if (!MainMenuClass) return;

	// Get the main active main menu widget
	ActiveMainMenu = CreateWidget<UUserWidget>(this, MainMenuClass);
	if (ActiveMainMenu)
	{
		// If it is found then add it to the view port
		ActiveMainMenu->AddToViewport(0);
	}

}

void ATDSPlayerController::ShowHUD()
{
	if (!HUDClass) return;

	if (!ActiveHUD)
	{
		ActiveHUD = CreateWidget<UTDSHUDWidget>(this, HUDClass);
		if (ActiveHUD)
		{
			ActiveHUD->AddToViewport(0);
		}
	}

	if (ActiveHUD)
	{
		if (ATDSCharacter* TDS = Cast<ATDSCharacter>(GetPawn()))
		{
			ActiveHUD->SetPlayer(TDS);
		}
	}
}


void ATDSPlayerController::ShowGameOver()
{
	// Remove HUD so it doesn't overlap/capture focus
	HideHUD();

	// Make sure that you have valid reference
	if (!GameOverClass) return;

	// Get the game over class
	ActiveGameOver = CreateWidget<UUserWidget>(this, GameOverClass);
	if (ActiveGameOver)
	{
		// If it is found then add it to the view port in a higher order
		ActiveGameOver->AddToViewport(200); // high Z-order
	}

	// Set the game over input mode
	SetGameOverInputMode();

	// Hard stop input (prevents spinning / moving)
	SetIgnoreMoveInput(true);
	SetIgnoreLookInput(true);

	// Optional pause (UI still works)
	SetPause(true);
}


void ATDSPlayerController::SetGameInputMode()
{
	// Set the correct mouse functions
	bShowMouseCursor = true;
	bEnableClickEvents = false;
	bEnableMouseOverEvents = false;
	 
	// Make sure that previous modes are disabled
	SetIgnoreMoveInput(false);
	SetIgnoreLookInput(false);

	FInputModeGameOnly Mode;

	// Don't eat the first mouse-down when capturing focus
	Mode.SetConsumeCaptureMouseDown(false);

	SetInputMode(Mode);

	// Unpause the game 
	SetPause(false);
}

void ATDSPlayerController::SetMenuInputMode()
{
	// Set the correct mouse functions
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	FInputModeUIOnly Mode;
	Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(Mode);

	// Menu should not control the pawn
	SetIgnoreMoveInput(true);
	SetIgnoreLookInput(true);

	SetPause(false);
}

void ATDSPlayerController::SetGameOverInputMode()
{
	bShowMouseCursor = true;

	// We don't want click events for gameplay
	bEnableClickEvents = false;
	bEnableMouseOverEvents = false;

	SetIgnoreMoveInput(false);
	SetIgnoreLookInput(false);

	FInputModeGameAndUI Mode;
	Mode.SetHideCursorDuringCapture(false);
	Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	// Important: focus the viewport so first click is not “just focus”
	Mode.SetWidgetToFocus(nullptr);

	SetInputMode(Mode);

	SetPause(false);
}


void ATDSPlayerController::HideHUD()
{
	// The HUD widget exists you need to hide it
	if (ActiveHUD)
	{
		ActiveHUD->RemoveFromParent();
		ActiveHUD = nullptr;
	}
}
