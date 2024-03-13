// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileData.generated.h"

UENUM()
enum EProjectileType : uint8
{
 Dumb,
 Homing
};

/**
 * 
 */
USTRUCT(Blueprintable)
struct GMCPROJECTILES_API FGMCProjectileData
{
 GENERATED_BODY()

 UPROPERTY(EditAnywhere, BlueprintReadWrite)
 TObjectPtr<AActor> OwnerActor{nullptr};

 UPROPERTY(EditAnywhere, BlueprintReadWrite)
 UClass* ProjectileClass{nullptr};

 UPROPERTY(EditAnywhere, BlueprintReadWrite)
 float StartTime{0};

 UPROPERTY(EditAnywhere, BlueprintReadWrite)
 FTransform StartTransform{FTransform::Identity};

 UPROPERTY(EditAnywhere, BlueprintReadWrite)
 FVector StartVelocity{FVector::ZeroVector};

 // How long in seconds projectile is alive. 0 means infinite
 UPROPERTY(EditAnywhere, BlueprintReadWrite)
 float LifeTime{0};

 UPROPERTY(EditAnywhere, BlueprintReadWrite)
 TEnumAsByte<EProjectileType> ProjectileType{EProjectileType::Dumb};

 float SimulatedSpawnDelay;
};

USTRUCT(Blueprintable)
struct GMCPROJECTILES_API FGMCProjectileDataDumb : public FGMCProjectileData
{
 GENERATED_BODY()

 UPROPERTY(EditAnywhere, BlueprintReadWrite)
 float Speed{0};

 UPROPERTY(EditAnywhere, BlueprintReadWrite)
 FVector Direction{FVector::ZeroVector};
};
