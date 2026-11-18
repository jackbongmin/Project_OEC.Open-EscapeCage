// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Room/Event/OecEventHazard.h"
#include "Actors/Room/Event/OecRoomEventBase.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "UObject/ConstructorHelpers.h"

AOecEventHazard::AOecEventHazard()
{
	PrimaryActorTick.bCanEverTick = false;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->InitSphereRadius(30.0f);
	Collision->SetCollisionObjectType(ECC_WorldDynamic);
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Collision->SetCollisionResponseToAllChannels(ECR_Ignore);
	Collision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	Collision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Collision->SetGenerateOverlapEvents(true);
	RootComponent = Collision;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Collision);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetRelativeScale3D(FVector(0.6f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> sphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (sphereMesh.Succeeded())
	{
		Mesh->SetStaticMesh(sphereMesh.Object);
	}

	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	Movement->UpdatedComponent = Collision;
	Movement->InitialSpeed = 0.0f;
	Movement->ProjectileGravityScale = 0.0f;
	Movement->bShouldBounce = true;
	Movement->Bounciness = 1.0f;
	Movement->Friction = 0.0f;

	Collision->OnComponentBeginOverlap.AddDynamic(this, &AOecEventHazard::OnHazardBeginOverlap);
}

void AOecEventHazard::BeginPlay()
{
	Super::BeginPlay();

	AOecRoomEventBase::SetBasicShapeColor(Mesh, HazardColor);
	Movement->OnProjectileStop.AddDynamic(this, &AOecEventHazard::OnProjectileStop);
}

void AOecEventHazard::InitHazard(AOecRoomEventBase* InOwnerEvent, float InDamage, const FVector& InVelocity, bool bInBounce, float InGravityScale, bool bInDestroyOnStop)
{
	OwnerEvent = InOwnerEvent;
	Damage = InDamage;
	bDestroyOnStop = bInDestroyOnStop;

	Movement->bShouldBounce = bInBounce;
	Movement->ProjectileGravityScale = InGravityScale;
	Movement->Velocity = InVelocity;
}

void AOecEventHazard::OnHazardBeginOverlap(UPrimitiveComponent* InOverlappedComponent, AActor* InOtherActor, UPrimitiveComponent* InOtherComp, int32 InOtherBodyIndex, bool bInFromSweep, const FHitResult& InSweepResult)
{
	const APawn* pawn = Cast<APawn>(InOtherActor);
	if (!pawn || !pawn->IsPlayerControlled() || !OwnerEvent.IsValid()) return;

	const float currentTime = GetWorld()->GetTimeSeconds();
	if (currentTime - LastHitTime < HitCooldown) return;

	LastHitTime = currentTime;
	OwnerEvent->ApplyDamageToPlayer(Damage);
}

void AOecEventHazard::OnProjectileStop(const FHitResult& InImpactResult)
{
	if (bDestroyOnStop)
	{
		Destroy();
	}
}
