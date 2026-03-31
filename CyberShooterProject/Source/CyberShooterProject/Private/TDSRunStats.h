#pragma once

#include "CoreMinimal.h"
#include "TDSRunStats.generated.h"

USTRUCT(BlueprintType)
struct FTDSRunStats
{
	GENERATED_BODY()

	/// Stats for a single run of the game, such as rooms cleared, enemies eliminated, and time survived.
	UPROPERTY(BlueprintReadOnly)
	int32 RoomsCleared = -1;

	UPROPERTY(BlueprintReadOnly)
	int32 EnemiesEliminated = 0;

};