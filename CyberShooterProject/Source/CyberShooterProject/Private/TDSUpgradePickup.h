// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TDSUpgradePickup.generated.h"

class UBoxComponent;
class UTDSUpgradeDefinition;
class ATDSCharacter;
class USoundBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpgradeCollected, UTDSUpgradeDefinition*, CollectedUpgrade);

UCLASS()
class ATDSUpgradePickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATDSUpgradePickup();

	// Delegate that is broadcast when the upgrade is collected by the player character. It passes the collected upgrade definition as a parameter.
	UPROPERTY(BlueprintAssignable, Category = "Upgrade")
	FOnUpgradeCollected OnUpgradeCollected;

	// Function to set the upgrade definition for this pickup. This can be called when spawning the pickup to specify which upgrade it should grant.
	void SetUpgradeDefinition(UTDSUpgradeDefinition* InUpgradeDefinition);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Sound to play when the upgrade is collected
	UPROPERTY(EditDefaultsOnly, Category = "Audio|Upgrade")
	TObjectPtr<USoundBase> CollectSound;

	// Volume to play the collect sound at
	UPROPERTY(EditDefaultsOnly, Category = "Audio|Upgrade", meta = (ClampMin = "0.0"))
	float CollectSoundVolume = 1.0f;

public:	

	// Components
	// The root component for the pickup
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> Root;

	// The mesh representing the pickup
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> PickupMesh;

	// The box component used for detecting overlaps with the player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> OverlapBox;

	// The upgrade definition that this pickup grants when collected
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Upgrade", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTDSUpgradeDefinition> UpgradeDefinition;

	// Whether the pickup should be destroyed after being collected
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Upgrade", meta = (AllowPrivateAccess = "true"))
	bool bDestroyOnCollect = true;

	// Whether the pickup has already been collected to prevent multiple collections
	bool bCollected = false;

	// Function to handle the overlap event when the player character overlaps with the pickup
	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	// Function to attempt granting the upgrade to the player character. Returns true if the upgrade was successfully granted.
	bool TryGrantUpgrade(ATDSCharacter* PlayerCharacter);

};
