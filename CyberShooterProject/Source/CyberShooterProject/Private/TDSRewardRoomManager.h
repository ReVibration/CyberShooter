#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TDSRewardRoomManager.generated.h"

class ATDSUpgradePickup;
class ATDSRewardExit;
class UTDSUpgradeDefinition;

UCLASS()
class ATDSRewardRoomManager : public AActor
{
	GENERATED_BODY()

public:
	ATDSRewardRoomManager();

protected:
	virtual void BeginPlay() override;

private:
	// Callback for when the player collects the upgrade pickup
	UFUNCTION()
	void HandleUpgradeCollected(UTDSUpgradeDefinition* CollectedUpgrade);

	// Spawns the upgrade pickup in the reward room
	void SpawnRewardPickup();
	// Chooses a random upgrade from the list of possible upgrades
	UTDSUpgradeDefinition* ChooseRandomUpgrade() const;

private:
	// The class of the upgrade pickup to spawn
	UPROPERTY(EditAnywhere, Category = "Reward Room")
	TSubclassOf<ATDSUpgradePickup> UpgradePickupClass;

	// List of possible upgrades to choose from when spawning a pickup
	UPROPERTY(EditAnywhere, Category = "Reward Room")
	TArray<TObjectPtr<UTDSUpgradeDefinition>> PossibleUpgrades;

	// Reference to the reward exit actor in the level
	UPROPERTY(EditAnywhere, Category = "Reward Room")
	TObjectPtr<ATDSRewardExit> RewardExit;

	// Location and rotation for spawning the upgrade pickup
	UPROPERTY(EditAnywhere, Category = "Reward Room")
	FVector PickupSpawnLocation = FVector::ZeroVector;

	// Rotation for the spawned upgrade pickup
	UPROPERTY(EditAnywhere, Category = "Reward Room")
	FRotator PickupSpawnRotation = FRotator::ZeroRotator;

	// Reference to the currently spawned upgrade pickup
	UPROPERTY()
	TObjectPtr<ATDSUpgradePickup> SpawnedPickup = nullptr;

	// Flag to track if the reward has already been collected
	bool bRewardCollected = false;
};