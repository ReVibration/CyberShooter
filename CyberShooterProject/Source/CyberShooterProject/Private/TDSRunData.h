#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TDSUpgradeTypes.h"
#include "TDSUpgradeDefinition.h"
#include "TDSRunData.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnRunUpgradesChanged);

UCLASS()
class UTDSRunData : public UObject
{
	GENERATED_BODY()

public:
	// Grants the specified upgrade to the player. If the player already has the maximum number of stacks for this upgrade,
	// the upgrade will not be granted and this function will return false. Otherwise, the upgrade will be granted and this function will return true.
	bool GrantUpgrade(UTDSUpgradeDefinition* UpgradeDefinition);
	// Resets the run data, clearing all owned upgrades and resetting any other run-specific data to its initial state. This should be called when starting a new run.
	void ResetRun();

	// Returns a debug string listing the upgrades currently owned by the player in this run, along with their stack counts. 
	// This is intended for debugging purposes and may not be formatted nicely for display to the player.
	UFUNCTION(BlueprintCallable, Category = "Upgrades")
	FString GetOwnedUpgradesDebugString() const;

	// Returns the list of upgrades currently owned by the player in this run, along with their stack counts. 
	// This should generally not be modified directly; use GrantUpgrade and ResetRun to modify the owned upgrades.
	const TArray<FTDSOwnedUpgrade>& GetOwnedUpgrades() const { return OwnedUpgrades; }

	// Returns the number of stacks the player currently has for the specified upgrade. If the player does not have this upgrade, returns 0.
	int32 GetStackCountForUpgrade(const UTDSUpgradeDefinition* UpgradeDefinition) const;

	// Delegate that is broadcast whenever the player's owned upgrades change (e.g. when GrantUpgrade or ResetRun is called). 
	// This can be used to update the UI or trigger other effects in response to upgrade changes.
	FOnRunUpgradesChanged OnRunUpgradesChanged;

private:
	// List of upgrades currently owned by the player in this run, along with their stack counts. This is modified by GrantUpgrade and ResetRun.
	UPROPERTY()
	TArray<FTDSOwnedUpgrade> OwnedUpgrades;

};