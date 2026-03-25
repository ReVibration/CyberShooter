// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "TDSRunStats.h"

// This function loads the main menu level when called.
void UTDSGameInstance::LoadMainMenu()
{
	UGameplayStatics::OpenLevel(this, FName("MainMenuLevel"));
}

// This function starts a new run with a specific seed by loading the main game level when called.
void UTDSGameInstance::StartNewRun(int32 NewSeed)
{
    // Reset the run stats to their default values and set the RunStartTimeSeconds to 0 before starting a new run.
    ResetRunStats();

    // Set the RunStartTimeSeconds to the current time in seconds from the world context, this will be used to calculate the time survived for the current run.
    StartRunStatsTracking();

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
        return;
    }

    if (NextRoom->LevelName.IsNone())
    {
        return;
    }

	// Load the level specified in the NextRoom's LevelName using UGameplayStatics::OpenLevel. This will transition the player to the new room.
    UGameplayStatics::OpenLevel(GetWorld(), NextRoom->LevelName);
}

// This function resets the current run stats to their default values and sets the RunStartTimeSeconds to 0. 
void UTDSGameInstance::ResetRunStats()
{
    CurrentRunStats = FTDSRunStats();
    RunStartTimeSeconds = 0.f;
}

// Increment the EnemiesEliminated count in the CurrentRunStats struct by 1. 
// This function can be called whenever an enemy is eliminated to keep track of the total number of enemies eliminated during the current run.
void UTDSGameInstance::RecordEnemyEliminated()
{
    CurrentRunStats.EnemiesEliminated++;
}

// Increment the RoomsCleared count in the CurrentRunStats struct by 1.
// This function can be called whenever a room is cleared to keep track of the total number of rooms cleared during the current run.
void UTDSGameInstance::RecordRoomCleared()
{
    CurrentRunStats.RoomsCleared++;
}

// This function finalizes the run stats by calculating the total time survived in seconds based on the current time in the world and the RunStartTimeSeconds.
// It updates the TimeSurvivedSeconds field in the CurrentRunStats struct with the calculated value. 
// This function should be called when the run ends to finalize the stats for that run.
void UTDSGameInstance::FinaliseRunStats()
{
    UE_LOG(LogTemp, Warning, TEXT("FinaliseRunStats called. Start time before calc = %f"), RunStartTimeSeconds);

    if (RunStartTimeSeconds <= 0.0)
    {
        CurrentRunStats.TimeSurvivedSeconds = 0.f;
        UE_LOG(LogTemp, Warning, TEXT("Run start time invalid, setting time survived to 0."));
        return;
    }

    CurrentRunStats.TimeSurvivedSeconds =
        static_cast<float>(FPlatformTime::Seconds() - RunStartTimeSeconds);

    UE_LOG(LogTemp, Warning, TEXT("Final time survived = %f"), CurrentRunStats.TimeSurvivedSeconds);
}

// This function returns a const reference to the CurrentRunStats struct, allowing other parts of the code to access the current run stats without modifying them.
const FTDSRunStats& UTDSGameInstance::GetCurrentRunStats() const
{
    return CurrentRunStats;
}

// This function starts tracking the run stats by setting the RunStartRealTimeSeconds to the current time in seconds using FPlatformTime::Seconds().
void UTDSGameInstance::StartRunStatsTracking()
{
    RunStartTimeSeconds = FPlatformTime::Seconds();
    UE_LOG(LogTemp, Warning, TEXT("StartRunStatsTracking called. Start time = %f"), RunStartTimeSeconds);
}