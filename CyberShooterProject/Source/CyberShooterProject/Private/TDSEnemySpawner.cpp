// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSEnemySpawner.h"
#include "Engine/World.h"
#include "TDSEnemyCharacter.h"

// Sets default values
ATDSEnemySpawner::ATDSEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Create a default scene component and set it as the root component
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

}


ATDSEnemyCharacter* ATDSEnemySpawner::SpawnEnemy(TSubclassOf<ATDSEnemyCharacter> EnemyClass)
{
	// Check if the world context is valid and the EnemyClass is set
    if (!GetWorld() || !EnemyClass)
    {
        return nullptr;
    }

	// Set up spawn parameters to ensure the enemy spawns even if there are collisions at the spawn location
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride =
        ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// Spawn the enemy actor at the spawner's location and rotation
    return GetWorld()->SpawnActor<ATDSEnemyCharacter>(
        EnemyClass,
        GetActorTransform(),
        SpawnParams
    );
}

