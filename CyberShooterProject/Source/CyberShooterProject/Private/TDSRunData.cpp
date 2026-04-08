// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSRunData.h"
#include "TDSUpgradeDefinition.h"

bool UTDSRunData::GrantUpgrade(UTDSUpgradeDefinition* UpgradeDefinition)
{
	// Validate the input
	if (!IsValid(UpgradeDefinition))
	{
		return false;
	}

	// Check if the player already has this upgrade. If so, increase the stack count (up to the maximum) and return. Otherwise, add a new entry for this upgrade with a stack count of 1.
	for (FTDSOwnedUpgrade& Entry : OwnedUpgrades)
	{
		if (Entry.Definition == UpgradeDefinition ||
			(Entry.Definition && Entry.Definition->UpgradeId == UpgradeDefinition->UpgradeId))
		{
			if (Entry.StackCount >= UpgradeDefinition->MaxStacks)
			{
				return false;
			}

			Entry.StackCount++;
			OnRunUpgradesChanged.Broadcast();
			return true;
		}
	}

	// The player doesn't have this upgrade yet, so add it with a stack count of 1
	FTDSOwnedUpgrade NewEntry;
	NewEntry.Definition = UpgradeDefinition;
	NewEntry.StackCount = 1;
	OwnedUpgrades.Add(NewEntry);

	// Broadcast the delegate to notify listeners that the owned upgrades have changed
	OnRunUpgradesChanged.Broadcast();
	return true;
}

// Resets the run data, clearing all owned upgrades and resetting any other run-specific data to its initial state. This should be called when starting a new run.
void UTDSRunData::ResetRun()
{
	// Clear the list of owned upgrades
	OwnedUpgrades.Empty();
	OnRunUpgradesChanged.Broadcast();
}

int32 UTDSRunData::GetStackCountForUpgrade(const UTDSUpgradeDefinition* UpgradeDefinition) const
{
	// Validate the input
	if (!IsValid(UpgradeDefinition))
	{
		return 0;
	}

	// Check if the player has this upgrade and return the stack count. If the player doesn't have this upgrade, return 0.
	for (const FTDSOwnedUpgrade& Entry : OwnedUpgrades)
	{
		if (Entry.Definition == UpgradeDefinition ||
			(Entry.Definition && Entry.Definition->UpgradeId == UpgradeDefinition->UpgradeId))
		{
			return Entry.StackCount;
		}
	}

	return 0;
}