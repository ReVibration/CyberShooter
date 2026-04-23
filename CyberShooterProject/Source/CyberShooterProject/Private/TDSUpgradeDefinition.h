#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TDSUpgradeTypes.h"
#include "TDSUpgradeDefinition.generated.h"

UCLASS(BlueprintType)
class UTDSUpgradeDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// Unique identifier for this upgrade, used for comparison and saving. Should be in the format "Namespace.UpgradeName"
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Upgrade")
	FName UpgradeId;

	// Display name for this upgrade, shown to the player when granting the upgrade and in the UI
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Upgrade")
	FText DisplayName;

	// Description of this upgrade, shown to the player when granting the upgrade and in the UI
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Upgrade", meta = (MultiLine = "true"))
	FText Description;

	// Modifiers applied by this upgrade. These are applied in order, so the order of entries in this array matters (e.g. AddPercent should generally come after AddFlat)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Upgrade")
	TArray<FTDSStatModifier> Modifiers;

	// Maximum number of stacks for this upgrade. If the player tries to gain this upgrade when they already have MaxStacks stacks, the upgrade will not be granted. Should be at least 1.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Upgrade", meta = (ClampMin = "1"))
	int32 MaxStacks = 1;
};