// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/StoredPositions.h"
#include "ProjectileData.generated.h"

USTRUCT(BlueprintType)
struct LAGCOMPENSATION_API FProjectileData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector StartPosition = FVector(FLT_MAX);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Velocity = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FShapeData ProjectileShapeData;

	static bool IsValidProjectileData(const FProjectileData& ProjectileData);
};
