﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/GMCProjectileComponent.h"

#include "GMCProjectiles.h"
#include "ProjectileData.h"
#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
UGMCProjectileComponent::UGMCProjectileComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UGMCProjectileComponent::SetProjectileData(const FInstancedStruct& _ProjectileData)
{
	Data = _ProjectileData;
	if (!Data.IsValid())
	{
		UE_LOG(LogGMCProjectiles, Error, TEXT("UGMCProjectileComponent::SetProjectileData: ProjectileData is invalid"));
		return;
	}
	
	ProjectileData = Data.Get<FGMCProjectileData>();
	
	InitializeProjectile();
}

void UGMCProjectileComponent::InitializeProjectile()
{
	GetOwner()->SetActorTransform(ProjectileData.StartTransform);
	if (ProjectileData.LifeTime > 0)
	{
		bTickLifetime = true;
		LifeTime = ProjectileData.LifeTime;
	}
	
	TimeBehind = GetTime() - ProjectileData.StartTime;
	InitializeRouter();
	// UE_LOG(LogTemp, Warning, TEXT("Server: %hhd | LocalProjectileStartTime: %f"), GetOwner()->HasAuthority(), LocalProjectileStartTime);

	if (InterpolationType == EInterpolationType::Snap)
	{
		SnapToServerState();
	}
}

void UGMCProjectileComponent::InitializeRouter()
{
	switch (ProjectileData.ProjectileType)
	{
		case EProjectileType::Dumb:
			{
				const FGMCProjectileDataDumb DumbData = Data.Get<FGMCProjectileDataDumb>();
				Velocity = GetOwner()->GetActorForwardVector() * DumbData.Speed;
				// TickDumbProjectile(0);
				break;
			}
		case EProjectileType::Homing:
			// Do homing stuff
			break;
		default:
			break;
	}
}

void UGMCProjectileComponent::SnapToServerState()
{
	GetOwner()->SetActorTransform(ProjectileData.StartTransform, false);
	TickComponent(GetTime() - ProjectileData.StartTime, ELevelTick::LEVELTICK_All, nullptr);
	bProjectileCaughtUp = true;
}

float UGMCProjectileComponent::SmoothToServerStateStep(float DeltaTime)
{
	float Step = 0.f;
	
	if (TimeBehind > 0)
	{
		Step = TimeBehind * SmoothingInterpSpeed;
		TimeBehind -= Step;
		
		if (TimeBehind <= DeltaTime / 2.f)
		{
			Step += TimeBehind;
			TimeBehind = 0.f;
			return Step;
		}
	}

	return Step;
}

// Called when the game starts
void UGMCProjectileComponent::BeginPlay()
{
	Super::BeginPlay();
}

float UGMCProjectileComponent::GetTime() const
{
	if (!ProjectileData.OwnerActor->HasAuthority())
	{
		 if(const AGMC_PlayerController* PC = Cast<AGMC_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
		 {
		 	return PC->CL_GetSyncedWorldTimeSeconds();
		 }
	}
	else
	{
		return GetWorld()->GetRealTimeSeconds();
	}

	return 0;
}

// Called every frame
void UGMCProjectileComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                            FActorComponentTickFunction* ThisTickFunction)
{
	if (bTickLifetime)
	{
		LifeTime -= DeltaTime;
		if (LifeTime <= 0)
		{
			GetOwner()->Destroy();
			return;
		}
	}
	
	// Snap to correct position
	switch(InterpolationType)
	{
		case EInterpolationType::Snap:
			break;
		case EInterpolationType::Smooth:
			DeltaTime += SmoothToServerStateStep(DeltaTime);
			break;
		default:
			break;
	}

	TickRouter(DeltaTime);
	
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UGMCProjectileComponent::TickRouter(float DeltaTime)
{	
	switch (ProjectileData.ProjectileType)
	{
		case EProjectileType::Dumb:
			TickDumbProjectile(DeltaTime);
			break;
		case EProjectileType::Homing:
			// Do homing stuff
			break;
	default:
		break;
	}
}

void UGMCProjectileComponent::TickDumbProjectile(float DeltaTime)
{
	// Do nothing, it's dumb! Just use the given velocity.
}

