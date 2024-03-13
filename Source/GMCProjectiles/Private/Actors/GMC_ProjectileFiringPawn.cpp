// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/GMC_ProjectileFiringPawn.h"

#include "GMCProjectiles.h"
#include "ProjectileData.h"
#include "Components/GMCProjectileComponent.h"


// Sets default values
AGMC_ProjectileFiringPawn::AGMC_ProjectileFiringPawn(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
	
	GMCMovementComponent = nullptr;
}

// Called when the game starts or when spawned
void AGMC_ProjectileFiringPawn::BeginPlay()
{
	Super::BeginPlay();

	GMCMovementComponent = GetComponentByClass<UGMC_MovementUtilityCmp>();
}

// Called every frame
void AGMC_ProjectileFiringPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TArray<TTuple<float, FInstancedStruct>> ToRemove;
	for (TTuple<float, FInstancedStruct>& SimProjectile : QueuedProjectiles)
	{
		if (!SimProjectile.Get<1>().IsValid()) continue;
		SimProjectile.Get<0>() -= DeltaTime;
		if (SimProjectile.Get<0>() <= 0)
		{
			FireProjectile(SimProjectile.Get<1>());
			ToRemove.Add(SimProjectile);
		}
	}

	for (auto Tuple : ToRemove)
	{
		QueuedProjectiles.Remove(Tuple);
	}
}

void AGMC_ProjectileFiringPawn::SpawnProjectile(FInstancedStruct ProjectileData)
{
	SpawnProjectiles({ProjectileData});
}

void AGMC_ProjectileFiringPawn::SpawnProjectiles(const TArray<FInstancedStruct>& ProjectilesData)
{
	for (auto Data : ProjectilesData)
	{
		if (Data.IsValid())
		{
			QueuedProjectiles.Add(TTuple<float, FInstancedStruct>(0.f, Data));
		}
	}

	if (HasAuthority())
	{
		MultiCast_SpawnProjectilesSimProxies(ProjectilesData);
	}
}

void AGMC_ProjectileFiringPawn::QueueProjectile(FInstancedStruct ProjectileData)
{
	if (ProjectileData.IsValid())
	{
		QueuedProjectiles.Add(TTuple<float, FInstancedStruct>(0.f, ProjectileData));
	}
}

void AGMC_ProjectileFiringPawn::FireProjectile(FInstancedStruct ProjectileData)
{
	if (!ProjectileData.IsValid()) return;
	const FGMCProjectileData InstanceData = ProjectileData.Get<FGMCProjectileData>();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	const AActor* Projectile = GetWorld()->SpawnActor(InstanceData.ProjectileClass, &InstanceData.StartTransform, SpawnParams);

	if (UGMCProjectileComponent* ProjectileComponent = Projectile->FindComponentByClass<UGMCProjectileComponent>())
	{
		ProjectileComponent->SetProjectileData(ProjectileData);
		OnProjectileFired.Broadcast(InstanceData, ProjectileComponent);
	}
}


void AGMC_ProjectileFiringPawn::MultiCast_SpawnProjectilesSimProxies_Implementation(
	const TArray<FInstancedStruct>& ProjectileData)
{
	// Server and AuthProxy has already spawned it by this point
	if (GetLocalRole() != ROLE_SimulatedProxy) return;
	
	for (auto Data : ProjectileData)
	{
		if (Data.IsValid())
		{
			const auto InstanceData = Data.Get<FGMCProjectileData>();
			// How far behind the sim proxy is in time
			float Delay = 0.f;
			if (GMCMovementComponent != nullptr)
			{
				Delay = InstanceData.StartTime - GMCMovementComponent->GetSmoothingTime();
			}
			
			QueuedProjectiles.Add(TTuple<float, FInstancedStruct>(Delay, Data));
		}
	}
}

