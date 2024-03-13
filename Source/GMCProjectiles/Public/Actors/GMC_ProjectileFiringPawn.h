// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GMCMovementUtilityComponent.h"
#include "GMCPawn.h"
#include "ProjectileData.h"
#include "GameFramework/Actor.h"
#include "GMC_ProjectileFiringPawn.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnProjectileFired, FGMCProjectileData, ProjectileData, UGMCProjectileComponent*, ProjectileComponent);

UCLASS()
class GMCPROJECTILES_API AGMC_ProjectileFiringPawn : public AGMC_Pawn
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGMC_ProjectileFiringPawn(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	virtual void SpawnProjectile(FInstancedStruct ProjectileData);

	UFUNCTION(BlueprintCallable)
	virtual void SpawnProjectiles(const TArray<FInstancedStruct>& ProjectilesData);

	virtual void QueueProjectile(FInstancedStruct ProjectileData);

	// Tell sim proxies to spawn projectile
	UFUNCTION(NetMulticast, Reliable)
	virtual void MultiCast_SpawnProjectilesSimProxies(const TArray<FInstancedStruct>& ProjectileData);
	
	virtual void FireProjectile(FInstancedStruct ProjectileData);

	UPROPERTY(BlueprintCallable)
	FOnProjectileFired OnProjectileFired;
private:
	// Projectiles that are queued to be spawned on the SimulatedProxy
	// The float is the simulation delay
	TArray<TTuple<float, FInstancedStruct>> QueuedProjectiles;

	UPROPERTY()
	UGMC_MovementUtilityCmp* GMCMovementComponent;
};
