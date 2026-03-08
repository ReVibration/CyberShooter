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
        const int32 RewardIndex = FMath::RandRange(0, RewardRooms.Num() - 1);
		// Return the selected reward room definition.
        return RewardRooms[RewardIndex];
    }

	// If it should be a combat room, check if there are any combat rooms available. If not, return nullptr to indicate that no room can be generated.
    if (CombatRooms.Num() == 0)
    {
        return nullptr;
    }

	// Randomly select a combat room from the CombatRooms array and return it.
    const int32 CombatIndex = FMath::RandRange(0, CombatRooms.Num() - 1);
    return CombatRooms[CombatIndex];
}