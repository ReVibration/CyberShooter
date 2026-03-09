// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSRoomManager.h"
#include "Kismet/GameplayStatics.h"
#include "TDSGameInstance.h"
#include "TDSEnemyCharacter.h"

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