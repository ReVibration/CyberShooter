// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSRoomManager.h"
#include "Kismet/GameplayStatics.h"
#include "TDSGameInstance.h"
#include "TDSEnemyCharacter.h"
#include "TDSEnemySpawner.h"

// Sets default values
ATDSRoomManager::ATDSRoomManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATDSRoomManager::BeginPlay()
{
	Super::BeginPlay();
	

	// Get all enemy actors in the room and bind to their death events
    TArray<AActor*> FoundEnemies;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATDSEnemyCharacter::StaticClass(), FoundEnemies);

	// Set the initial count of alive enemies
    AliveEnemyCount = FoundEnemies.Num();

	// Bind to the death event of each enemy to track when they die
    for (AActor* Actor : FoundEnemies)
    {
        ATDSEnemyCharacter* Enemy = Cast<ATDSEnemyCharacter>(Actor);
        if (Enemy)
        {
            Enemy->OnEnemyDied.AddDynamic(this, &ATDSRoomManager::HandleEnemyDied);
        }
    }

	// If there are no enemies in the room, consider it cleared immediately
    if (AliveEnemyCount == 0)
    {
        OnRoomCleared();
    }
}

int32 ATDSRoomManager::CalculateSpawnCount(int32 RoomIndex, int32 AvailableSpawnerCount) const
{
	// Start with the base enemy count and add additional enemies based on the room index and the configured scaling
    int32 SpawnCount = BaseEnemyCount;

	// Add additional enemies for every X rooms cleared, allowing the difficulty to scale as the player progresses
    if (AdditionalEnemyEveryXRooms > 0)
    {
        SpawnCount += RoomIndex / AdditionalEnemyEveryXRooms;
    }

	// Clamp the spawn count to ensure it does not exceed the maximum allowed or the number of available spawners
    SpawnCount = FMath::Clamp(SpawnCount, 1, MaxEnemyCount);
    SpawnCount = FMath::Min(SpawnCount, AvailableSpawnerCount);

    return SpawnCount;
}


void ATDSRoomManager::SpawnRoomEnemies()
{
	// Get all enemy spawner actors in the room
    TArray<AActor*> FoundSpawners;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATDSEnemySpawner::StaticClass(), FoundSpawners);

	// Convert the found actors to their specific spawner class and store them in a separate array for easier access
    TArray<ATDSEnemySpawner*> Spawners;
    for (AActor* Actor : FoundSpawners)
    {
		// Cast the actor to the specific spawner class and add it to the Spawners array if the cast is successful
        if (ATDSEnemySpawner* Spawner = Cast<ATDSEnemySpawner>(Actor))
        {
            Spawners.Add(Spawner);
        }
    }

	// If there are no spawners found or the default enemy class is not set, log a warning and exit the function to prevent errors
    if (Spawners.Num() == 0 || !DefaultEnemyClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnRoomEnemies failed: No spawners or DefaultEnemyClass missing."));
        return;
    }


    // Get the game instance and load the next room
    UTDSGameInstance* GI = Cast<UTDSGameInstance>(GetGameInstance());
    if (!GI)
    {
        return;
    }
	// Calculate the number of enemies to spawn based on the current room index and the number of available spawners
	const int32 RoomIndex = GI->CurrentRoomIndex;
    const int32 SpawnCount = CalculateSpawnCount(RoomIndex, Spawners.Num());

    for (int32 i = 0; i < Spawners.Num(); ++i)
    {
        const int32 SwapIndex = FMath::RandRange(i, Spawners.Num() - 1);
        Spawners.Swap(i, SwapIndex);
    }


	// Spawn enemies at the spawners and bind to their death events to track when they die
    for (int32 i = 0; i < SpawnCount; ++i)
    {
        ATDSEnemyCharacter* SpawnedEnemy = Spawners[i]->SpawnEnemy(DefaultEnemyClass);
        if (SpawnedEnemy)
        {
            SpawnedEnemy->OnEnemyDied.AddDynamic(this, &ATDSRoomManager::HandleEnemyDied);
            AliveEnemyCount++;
        }
    }

	// If no enemies were spawned (e.g., due to spawner issues), consider the room cleared immediately to prevent soft-locking the player
    if (AliveEnemyCount <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No enemies were spawned. Completing room immediately."));
        OnRoomCleared();
    }

    UE_LOG(LogTemp, Log, TEXT("Spawned %d enemies for room index %d"), AliveEnemyCount, RoomIndex);
}


// Handle the death of an enemy and update the alive enemy count
void ATDSRoomManager::HandleEnemyDied(AActor* DeadEnemy)
{
	// Decrease the count of alive enemies
    AliveEnemyCount--;

	// Log the remaining number of alive enemies for debugging purposes
    UE_LOG(LogTemp, Log, TEXT("Enemy died. Remaining enemies: %d"), AliveEnemyCount);

	// If there are no more alive enemies, the room is cleared
    if (AliveEnemyCount <= 0)
    {
        OnRoomCleared();
    }
}

// When the room is cleared, this function will be called to perform any necessary actions (e.g., opening doors, spawning rewards)
void ATDSRoomManager::OnRoomCleared()
{
	// Log that the room has been cleared for debugging purposes
    UE_LOG(LogTemp, Log, TEXT("Room cleared"));

	// Get the game instance and load the next room
    UTDSGameInstance* GI = Cast<UTDSGameInstance>(GetGameInstance());
    if (!GI)
    {
        return;
    }

	// Increment the current room index and load the next room
    GI->CurrentRoomIndex++;
    GI->LoadNextRoom();
}