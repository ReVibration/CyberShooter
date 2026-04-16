#include "TDSUpgradeComponent.h"
#include "TDSGameInstance.h"
#include "TDSRunData.h"
#include "TDSUpgradeDefinition.h"
#include "TDSUpgradeTypes.h"
#include "TDSCharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UTDSUpgradeComponent::UTDSUpgradeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTDSUpgradeComponent::BeginPlay()
{
	Super::BeginPlay();
	// Cache the base move speed from the character's movement component to use as a reference for calculating modified move speed based on upgrades.
	if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		if (UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement())
		{
			BaseMoveSpeed = MoveComp->MaxWalkSpeed;
		}
	}

	// Bind to the run data to listen for changes in owned upgrades and update the modifier totals accordingly, 
	// then refresh the modified stats based on the current owned upgrades.
	BindToRunData();
	RefreshFromRunData();
}

void UTDSUpgradeComponent::BindToRunData()
{
	// Cache a reference to the run data from the game instance.
	UTDSGameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance<UTDSGameInstance>() : nullptr;
	if (!GI)
	{
		return;
	}

	// Get the run data from the game instance and bind to its OnRunUpgradesChanged event to listen for changes in owned upgrades and update the modifier totals when they change.
	UTDSRunData* RunData = GI->GetRunData();
	if (!RunData)
	{
		return;
	}

	// Cache the run data reference and bind to its OnRunUpgradesChanged event to listen for changes in owned upgrades and update the modifier totals when they change.
	CachedRunData = RunData;
	RunData->OnRunUpgradesChanged.AddUObject(this, &UTDSUpgradeComponent::HandleRunUpgradesChanged);
}

void UTDSUpgradeComponent::HandleRunUpgradesChanged()
{
	RefreshFromRunData();
}

void UTDSUpgradeComponent::RefreshFromRunData()
{
	RebuildModifierTotals();
	ApplyImmediateStats();
}

void UTDSUpgradeComponent::RebuildModifierTotals()
{
	// Clear the existing modifier totals before rebuilding them based on the current owned upgrades.
	ModifierTotals.Empty();

	// Get the current run data from the cached reference and iterate through the owned upgrades to calculate the total flat and percent modifiers for each stat type
	// then store them in the ModifierTotals map for efficient retrieval when calculating modified stat values.
	const UTDSRunData* RunData = CachedRunData.Get();
	if (!RunData)
	{
		return;
	}

	// Iterate through the owned upgrades and calculate the total flat and percent modifiers for each stat type, 
	// then store them in the ModifierTotals map for efficient retrieval when calculating modified stat values.
	for (const FTDSOwnedUpgrade& OwnedUpgrade : RunData->GetOwnedUpgrades())
	{
		if (!IsValid(OwnedUpgrade.Definition) || OwnedUpgrade.StackCount <= 0)
		{
			continue;
		}

		// Iterate through the modifiers of the owned upgrade and accumulate the total flat and percent modifiers for each stat type in the ModifierTotals map.
		for (const FTDSStatModifier& Modifier : OwnedUpgrade.Definition->Modifiers)
		{
			FTDSModifierTotals& Totals = ModifierTotals.FindOrAdd(Modifier.Stat);

			const float TotalMagnitude = Modifier.Magnitude * OwnedUpgrade.StackCount;

			// Depending on the operation type of the modifier, accumulate the total flat or percent modifiers for the stat type in the ModifierTotals map.
			if (Modifier.Operation == ETDSModifierOp::AddFlat)
			{
				Totals.Flat += TotalMagnitude;
			}
			else if (Modifier.Operation == ETDSModifierOp::AddPercent)
			{
				Totals.Percent += TotalMagnitude;
			}
		}
	}
}

float UTDSUpgradeComponent::GetModifiedValue(ETDSStatType Stat, float BaseValue) const
{
	// Retrieve the total flat and percent modifiers for the specified stat type from the ModifierTotals map. If there are no modifiers for this stat type, return the base value.
	const FTDSModifierTotals* Totals = ModifierTotals.Find(Stat);
	if (!Totals)
	{
		return BaseValue;
	}

	return (BaseValue + Totals->Flat) * (1.0f + Totals->Percent);
}

void UTDSUpgradeComponent::ApplyImmediateStats()
{
	ATDSCharacter* Character = Cast<ATDSCharacter>(GetOwner());
	if (!Character)
	{
		return;
	}

	const float NewMaxHealth =
		GetModifiedValue(ETDSStatType::MaxHealth, Character->GetBaseMaxHealth());

	const float NewMoveSpeed =
		GetModifiedValue(ETDSStatType::MoveSpeed, Character->GetBaseMoveSpeed());

	const float FireRateMultiplier =
		GetModifiedValue(ETDSStatType::FireRateMultiplier, 1.0f);

	const float NewFireInterval =
		Character->GetBaseFireInterval() / FMath::Max(FireRateMultiplier, 0.01f);

	const float NewProjectileDamage =
		GetModifiedValue(ETDSStatType::ProjectileDamage, Character->GetBaseProjectileDamage());

	const float NewProjectileSpeed =
		GetModifiedValue(ETDSStatType::ProjectileSpeed, Character->GetBaseProjectileSpeed());

	Character->ApplyRecalculatedStats(
		NewMaxHealth,
		NewMoveSpeed,
		NewFireInterval,
		NewProjectileDamage,
		NewProjectileSpeed);
}