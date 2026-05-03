// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TDSProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class USoundBase;

UCLASS()
class ATDSProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATDSProjectile();

	void InitialiseProjectile(float InDamage, float InSpeed);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Sound to play when the projectile hits something
	UPROPERTY(EditDefaultsOnly, Category = "Audio|Impact")
	TObjectPtr<USoundBase> ImpactSound;

	// Volume to play the impact sound at
	UPROPERTY(EditDefaultsOnly, Category = "Audio|Impact", meta = (ClampMin = "0.0"))
	float ImpactSoundVolume = 1.0f;

private:	
	
	// Hitbox component
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* Collision;
    
	// Visual mesh component
    UPROPERTY(VisibleAnywhere, Category = "Components")
    UStaticMeshComponent* Mesh;

	// Movement component
    UPROPERTY(VisibleAnywhere, Category = "Movement")
    UProjectileMovementComponent* ProjectileMovement;

	// Function called when projectile hits something
    UFUNCTION()
    void OnHit(
        UPrimitiveComponent* HitComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        FVector NormalImpulse,
        const FHitResult& Hit
    );

	// Lifetime of the projectile in seconds
    UPROPERTY(EditDefaultsOnly, Category = "Lifetime")
    float LifeSeconds = 2.0f;

	// Damage dealt by the projectile
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float Damage = 25.f;
};

