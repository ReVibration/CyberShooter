// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSGameMode.h"
#include "TDSPlayerController.h"
#include "TDSCharacter.h"

ATDSGameMode::ATDSGameMode()
{
	// Set this game mode to use our custom player controller class 
	PlayerControllerClass = ATDSPlayerController::StaticClass();

	// Set the default pawn to be the player
	DefaultPawnClass = ATDSCharacter::StaticClass();
}

void ATDSGameMode::HandleGameOver(AController* DeadController)
{
	ATDSPlayerController* PC = Cast<ATDSPlayerController>(DeadController);

	if (PC)
	{
		PC->ShowGameOver();
	}
}

