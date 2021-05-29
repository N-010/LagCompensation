// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/StoredPositions.h"
#include "LagCompensationFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class LAGCOMPENSATION_API ULagCompensationFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="LagCompensation|Utilities")
	static UObject* GetCDO(UClass* ObjectClass);

	UFUNCTION(BlueprintCallable, Category="LagCompensation|Utilities|Projectile")
	static FShapeData GetShapeDataFromPrimitiveComponent(const UPrimitiveComponent* PrimitiveComponent);
};
