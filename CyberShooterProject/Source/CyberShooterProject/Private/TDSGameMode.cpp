// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSGameMode.h"
#include "TDSPlayerController.h"
#include "TDSCharacter.h"

#include "Kismet/GameplayStatics.h"
#include "TDSGameInstance.h"
#include "TDSRoomDefinition.h"

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

// This function loads the next room by getting the next room definition from the game instance and opening the level specified in that room definition.
void ATDSGameMode::LoadNextRoom()
{
	// Get the game instance and ensure it is of type UTDSGameInstance. If the game instance is null, log a warning and return.
    UTDSGameInstance* GI = GetGameInstance<UTDSGameInstance>();
    if (!GI)
    {
        UE_LOG(LogTemp, Warning, TEXT("LoadNextRoom: GameInstance is null"));
        return;
    }

	// Get the next room definition from the game instance. If the next room definition is null, log a warning and return.
    UTDSRoomDefinition* NextRoom = GI->GetNextRoomDefinition();
    if (!NextRoom)
    {
        UE_LOG(LogTemp, Warning, TEXT("LoadNextRoom: NextRoom is null"));
        return;
    }
	// Check if the level name in the next room definition is valid. If it is None, log a warning and return.
    if (NextRoom->LevelName.IsNone())
    {
        UE_LOG(LogTemp, Warning, TEXT("LoadNextRoom: LevelName is None"));
        return;
    }

	// Open the level specified in the next room definition using UGameplayStatics::OpenLevel. This will load the new level and transition the player to it.
    UGameplayStatics::OpenLevel(this, NextRoom->LevelName);
}
