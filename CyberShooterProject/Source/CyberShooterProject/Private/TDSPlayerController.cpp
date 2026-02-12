// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSPlayerController.h"
#include "Blueprint/UserWidget.h"

void ATDSPlayerController::BeginPlay()
{
	// Call the base class BeginPlay to ensure any inherited functionality is executed
	Super::BeginPlay();
	
	// Get the name of the current level to determine which UI to show
	FString LevelName = GetWorld()->GetMapName();

	//If we're in the main menu level, show the main menu UI
	if (LevelName.Contains("MainMenulevel"))
	{
		ShowMainMenu();
		// Enable the mouse cursor for interaction
		bShowMouseCursor = true;
		//Set the input mode to UI only, so the player can interact with the menu without affecting the game world
		SetInputMode(FInputModeUIOnly());
	};
}

void ATDSPlayerController::ShowMainMenu()
{
	// Check if the MainMenuClass is set, and if so, create the widget and add it to the viewport to display the main menu UI
	if (MainMenuClass)
	{
		UUserWidget* Menu = CreateWidget(this, MainMenuClass);
		Menu->AddToViewport();
	}
}

void ATDSPlayerController::ShowGameOver()
{
	//Check if the GameOverClass is set, and
	if (GameOverClass)
	{
		//Create the game over widget and add it to the viewport to display the game over UI
		UUserWidget* GameOver = CreateWidget(this, GameOverClass);
		GameOver->AddToViewport();

		//Enable the mouse cursor for interaction with the game over screen
		bShowMouseCursor = true; 
		SetInputMode(FInputModeUIOnly());
	}
}