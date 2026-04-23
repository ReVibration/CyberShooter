#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TDSUpgradeTypes.h"
#include "TDSUpgradeComponent.generated.h"

// Forward declaration to avoid circular dependency
class UTDSRunData;

// A struct to hold the total flat and percent modifiers for a specific stat type. This is used to efficiently calculate the final modified value for each stat.
struct FTDSModifierTotals
{
	float Flat = 0.0f;
	float Percent = 0.0f;
};

// This component is responsible for managing the character's upgrades and calculating the modified stats based on the owned upgrades.
// It listens for changes in the run data and updates the modifier totals accordingly.
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UTDSUpgradeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTDSUpgradeComponent();

	virtual void BeginPlay() override;

	// Returns the modified value for a given stat type, based on the base value and the total modifiers from owned upgrades.
	float GetModifiedValue(ETDSStatType Stat, float BaseValue) const;
	void RefreshFromRunData();

private:
	// Binds to the run data's OnUpgradesChanged event to listen for changes in owned upgrades and update the modifier totals when they change.
	void BindToRunData();
	void RebuildModifierTotals();
	void ApplyImmediateStats();
	void HandleRunUpgradesChanged();

private:
	// A map to store the total flat and percent modifiers for each stat type, calculated from the owned upgrades.
	// This allows for efficient retrieval of the total modifiers when calculating modified stat values.
	TMap<ETDSStatType, FTDSModifierTotals> ModifierTotals;

	// A weak pointer to the run data, which contains the owned upgrades and is used to calculate the modifier totals. 
	// We use a weak pointer to avoid circular references and potential memory leaks.
	TWeakObjectPtr<UTDSRunData> CachedRunData;

	// Example base values for stats that can be modified by upgrades. In a real implementation, these would likely come from the character or other game data.
	float BaseMoveSpeed = 0.0f;
};