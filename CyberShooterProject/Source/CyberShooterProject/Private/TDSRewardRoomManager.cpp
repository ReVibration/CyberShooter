#include "TDSRewardRoomManager.h"
#include "TDSUpgradePickup.h"
#include "TDSRewardExit.h"
#include "TDSUpgradeDefinition.h"
#include "TDSGameInstance.h"

ATDSRewardRoomManager::ATDSRewardRoomManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATDSRewardRoomManager::BeginPlay()
{
	Super::BeginPlay();

	// Lock the exit at the start of the reward room. It will be unlocked when the player collects the reward pickup.
	if (RewardExit)
	{
		RewardExit->LockExit();
	}

	// Spawn the reward pickup in the reward room
	SpawnRewardPickup();
}

void ATDSRewardRoomManager::SpawnRewardPickup()
{
	// Ensure we have a valid class to spawn
	if (!UpgradePickupClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("RewardRoomManager: UpgradePickupClass is null"));
		return;
	}

	// Choose a random upgrade from the list of possible upgrades
	UTDSUpgradeDefinition* ChosenUpgrade = ChooseRandomUpgrade();
	if (!ChosenUpgrade)
	{
		UE_LOG(LogTemp, Warning, TEXT("RewardRoomManager: No valid upgrade chosen"));
		return;
	}

	// Spawn the pickup actor in the world
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	SpawnedPickup = GetWorld()->SpawnActor<ATDSUpgradePickup>(
		UpgradePickupClass,
		PickupSpawnLocation,
		PickupSpawnRotation,
		Params
	);

	// Ensure the pickup was spawned successfully
	if (!SpawnedPickup)
	{
		UE_LOG(LogTemp, Warning, TEXT("RewardRoomManager: Failed to spawn pickup"));
		return;
	}

	// Set the chosen upgrade on the spawned pickup and bind to its collection event
	SpawnedPickup->SetUpgradeDefinition(ChosenUpgrade);
	SpawnedPickup->OnUpgradeCollected.AddDynamic(this, &ATDSRewardRoomManager::HandleUpgradeCollected);

	UE_LOG(LogTemp, Warning, TEXT("RewardRoomManager: Spawned pickup for %s"), *ChosenUpgrade->GetName());
}


UTDSUpgradeDefinition* ATDSRewardRoomManager::ChooseRandomUpgrade() const
{
	// Filter out any null entries from the PossibleUpgrades array
	TArray<UTDSUpgradeDefinition*> ValidUpgrades;

	// Filter out any null entries from the PossibleUpgrades array
	for (UTDSUpgradeDefinition* Upgrade : PossibleUpgrades)
	{
		if (Upgrade)
		{
			ValidUpgrades.Add(Upgrade);
		}
	}

	// If there are no valid upgrades, return null
	if (ValidUpgrades.Num() == 0)
	{
		return nullptr;
	}

	if (const UTDSGameInstance* GI = Cast<UTDSGameInstance>(GetGameInstance()))
	{
		// Temporary version: use global random for now.
		// Later this should use GI-owned FRandomStream for deterministic runs.
	}

	// Choose a random index from the valid upgrades and return the upgrade at that index
	const int32 Index = FMath::RandRange(0, ValidUpgrades.Num() - 1);
	return ValidUpgrades[Index];
}

void ATDSRewardRoomManager::HandleUpgradeCollected(UTDSUpgradeDefinition* CollectedUpgrade)
{
	// If the reward has already been collected, do nothing
	if (bRewardCollected)
	{
		return;
	}

	// Mark the reward as collected to prevent multiple collections
	bRewardCollected = true;

	UE_LOG(LogTemp, Warning, TEXT("RewardRoomManager: Upgrade collected"));

	// Unlock the exit to allow the player to leave the reward room
	if (RewardExit)
	{
		RewardExit->UnlockExit();
	}
}