// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSGameInstance.h"
#include "Kismet/GameplayStatics.h"

// This function loads the main menu level when called.
void UTDSGameInstance::LoadMainMenu()
{
	UGameplayStatics::OpenLevel(this, FName("MainMenuLevel"));
}

// This function starts a new run with a specific seed by loading the main game level when called.
void UTDSGameInstance::StartNewRun(int32 NewSeed)
{
	RunSeed = NewSeed;
	CurrentRoomIndex = 0;
	LastCombatRoomIndex = INDEX_NONE;
	LastRewardRoomIndex = INDEX_NONE;
}

// This function determines the next room definition to use based on the current room index and the type of room that should be generated. 
// It uses a simple rule where every third room is a reward room, and the others are combat rooms. 
// It randomly selects a room definition from the appropriate array based on the type of room that should be generated.
UTDSRoomDefinition* UTDSGameInstance::GetNextRoomDefinition()
{
    // Determine if the next room should be a reward room based on the index
    const bool bShouldUseRewardRoom = ((CurrentRoomIndex + 1) % 3 == 0);

	// If it should be a reward room.
    if (bShouldUseRewardRoom)
    {
		// If there are no reward rooms available, return nullptr to indicate that no room can be generated.
        if (RewardRooms.Num() == 0)
        {
            return nullptr;
        }

		// Randomly select a reward room from the RewardRooms array and return it.
        int32 RewardIndex = FMath::RandRange(0, RewardRooms.Num() - 1);

        if (RewardRooms.Num() > 1)
        {
            while (RewardIndex == LastRewardRoomIndex)
            {
                RewardIndex = FMath::RandRange(0, RewardRooms.Num() - 1);
            }
        }

        LastRewardRoomIndex = RewardIndex;
        return RewardRooms[RewardIndex];
    }

	// If it should be a combat room, check if there are any combat rooms available. If not, return nullptr to indicate that no room can be generated.
    if (CombatRooms.Num() == 0)
    {
        return nullptr;
    }

	// Randomly select a combat room from the CombatRooms array and return it.
    int32 CombatIndex = FMath::RandRange(0, CombatRooms.Num() - 1);
    if (CombatRooms.Num() > 1)
    {
        while (CombatIndex == LastCombatRoomIndex)
        {
            CombatIndex = FMath::RandRange(0, CombatRooms.Num() - 1);
        }
    }

    LastCombatRoomIndex = CombatIndex;
    return CombatRooms[CombatIndex];
}

// This function loads the next room based on the current room index and the type of room that should be generated.
void UTDSGameInstance::LoadNextRoom()
{
	// Get the next room definition using the GetNextRoomDefinition function. If it returns nullptr, log a warning and return without loading a new level.
    UTDSRoomDefinition* NextRoom = GetNextRoomDefinition();
    if (!NextRoom)
    {
        UE_LOG(LogTemp, Warning, TEXT("LoadNextRoom: NextRoom is null"));
        return;
    }

    if (NextRoom->LevelName.IsNone())
    {
        UE_LOG(LogTemp, Warning, TEXT("LoadNextRoom: LevelName is None"));
        return;
    }

	// Load the level specified in the NextRoom's LevelName using UGameplayStatics::OpenLevel. This will transition the player to the new room.
    UGameplayStatics::OpenLevel(GetWorld(), NextRoom->LevelName);
}