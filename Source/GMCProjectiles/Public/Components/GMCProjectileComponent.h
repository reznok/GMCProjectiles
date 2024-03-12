// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GMCPlayerController.h"
#include "InstancedStruct.h"
#include "ProjectileData.h"
#include "Components/ActorComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GMCProjectileComponent.generated.h"

UENUM()
enum class EInterpolationType : uint8
{
	None,
	Snap,
	Smooth
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GMCPROJECTILES_API UGMCProjectileComponent : public UProjectileMovementComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGMCProjectileComponent();

	UFUNCTION(BlueprintCallable)
	void SetProjectileData(const FInstancedStruct& ProjectileData);
	
	virtual void InitializeProjectile();

	virtual void InitializeRouter();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;
	

	virtual void TickRouter(float DeltaTime);

	virtual void TickDumbProjectile(float DeltaTime);
	virtual void TickHomingProjectile(float DeltaTime){};

	// Snaps the projectile immediately to the server state.
	// Will still trigger overlaps along the way
	virtual void SnapToServerState();

	// Get an increased DeltaTime step to catch up to the server state
	// https://fish-networking.gitbook.io/docs/manual/guides/lag-compensation/projectiles
	virtual float SmoothToServerStateStep(float DeltaTime);
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	float GetTime() const;
	
	UPROPERTY()
	AGMC_PlayerController* PlayerController;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EInterpolationType> InterpolationType = EInterpolationType::Smooth;

	UPROPERTY(EditAnywhere)
	float SmoothingInterpSpeed = .1f;
	
	UPROPERTY(EditAnywhere)
	float CatchupStepTime = 1.f;

	bool bTickLifetime;
	float LifeTime;

	// How much time the local version of the projectile is behind the server
	float TimeBehind;
	
	bool bProjectileCaughtUp = false;
	
	FInstancedStruct Data;
	FGMCProjectileData ProjectileData;
};
