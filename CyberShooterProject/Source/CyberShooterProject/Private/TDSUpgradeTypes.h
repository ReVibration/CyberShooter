#pragma once

#include "CoreMinimal.h"
#include "TDSUpgradeTypes.generated.h"

// Types of stats that can be modified by upgrades. This is used to specify which stat an upgrade modifier should apply to.
UENUM(BlueprintType)
enum class ETDSStatType : uint8
{
	MaxHealth,
	MoveSpeed,
	FireRateMultiplier,
	ProjectileDamage,
	ProjectileSpeed
};

// How the modifier should be applied to the stat. AddFlat adds a flat amount (e.g. +20 health), while AddPercent adds a percentage (e.g. 0.15 = +15% fire rate)
UENUM(BlueprintType)
enum class ETDSModifierOp : uint8
{
	AddFlat,
	AddPercent
};

USTRUCT(BlueprintType)
struct FTDSStatModifier
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Upgrade")
	ETDSStatType Stat = ETDSStatType::MaxHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Upgrade")
	ETDSModifierOp Operation = ETDSModifierOp::AddFlat;

	// AddFlat: raw amount, e.g. +20 health
	// AddPercent: 0.15 = +15%
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Upgrade")
	float Magnitude = 0.0f;
};

USTRUCT(BlueprintType)
struct FTDSOwnedUpgrade
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<class UTDSUpgradeDefinition> Definition = nullptr;

	UPROPERTY()
	int32 StackCount = 0;
};