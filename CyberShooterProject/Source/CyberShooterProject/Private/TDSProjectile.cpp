// Fill out your copyright notice in the Description page of Project Settings.

#include "TDSProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ATDSProjectile::ATDSProjectile()
{
	// No need to tick every frame
	PrimaryActorTick.bCanEverTick = false;
	
    // Collision
	// Creates a sphere hitbox 10 units in radius
    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    Collision->InitSphereRadius(10.f);

	// This means the projectile will block all other objects
    Collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Collision->SetCollisionObjectType(ECC_WorldDynamic);
    Collision->SetCollisionResponseToAllChannels(ECR_Block);
    Collision->SetNotifyRigidBodyCollision(true);

	// Set as root component
    RootComponent = Collision;

	// Mesh (optional - you can assign in BP) avoids double collision checks
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // Movement
	// Creates a movement component to handle projectile movement
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));

	// Set movement parameters
    ProjectileMovement->InitialSpeed = 2500.f;
    ProjectileMovement->MaxSpeed = 2500.f;
    
	// Bullet rotates to face the direction it's moving
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;

	// No gravity effect
    ProjectileMovement->ProjectileGravityScale = 0.f;
}

// Called when the game starts or when spawned
void ATDSProjectile::BeginPlay()
{
	Super::BeginPlay();

	// When projectile hits something, call OnHit function
    Collision->OnComponentHit.AddDynamic(this, &ATDSProjectile::OnHit);

    // Auto-destroy after LifeSeconds
    SetLifeSpan(LifeSeconds);
	
}

void ATDSProjectile::OnHit(
    UPrimitiveComponent* HitComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    FVector NormalImpulse,
    const FHitResult& Hit)
{
    // Prevent self-hit / weird cases
    if (!OtherActor || OtherActor == this) return;

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red,
            FString::Printf(TEXT("Hit: %s"), *GetNameSafe(OtherActor)));
    }

    // Later: apply damage here
    //Destroy();
}

