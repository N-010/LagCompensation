// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Types.h"
#include "StoredPositions.generated.h"



USTRUCT(BlueprintType)
struct LAGCOMPENSATION_API FShapeData
{
	GENERATED_BODY()

	explicit FShapeData();
	explicit FShapeData(const LagCompensationTypes::FBodyInstances& BodyInstances, const bool bInLocalTransform = false);
	explicit FShapeData(const TArray<FBodyInstance>& BodyInstances, const bool bInLocalTransform = false);
	explicit FShapeData(const FBodyInstance* BodyInstance, const bool bInLocalTransform = false);
	
	FShapeData(FShapeData&& ShapeData);
	FShapeData(const FShapeData& ShapeData);

	FShapeData& operator=(const FShapeData& ShapeData);
	FShapeData& operator=(const PhysicsInterfaceTypes::FInlineTransformArray& OtherShapeTransforms);
	FShapeData& operator=(PhysicsInterfaceTypes::FInlineTransformArray&& OtherShapeTransforms);
	
	FShapeData& operator=(const PhysicsInterfaceTypes::FInlineShapeArray& OtherShapes);
	FShapeData& operator=(PhysicsInterfaceTypes::FInlineShapeArray&& OtherShapes);

	void FillDataFromBodyInstance(const FBodyInstance* BodyInstance, const bool bInLocalTransform = false);

	void ComposeTransform(const FTransform& OtherTransform);

	PhysicsInterfaceTypes::FInlineShapeArray Shapes;
	PhysicsInterfaceTypes::FInlineTransformArray ShapeTransforms;
	LagCompensationTypes::FPhysicsShapeAdapters ShapeAdapters;

	bool bLocalTransform;

	FORCEINLINE bool IsValidShapeData() const;
};

USTRUCT(BlueprintType)
struct LAGCOMPENSATION_API FStoredPositions
{
	GENERATED_BODY()

	FStoredPositions(const FVector& InPosition, const FRotator& InRotation, const FVector& InVelocity,
	                 bool bInJustTeleported, const float& InWorldTime, const float& InTimeStamp);

	FStoredPositions(const TArray<FBodyInstance*>& BodyInstances, const FVector& InPosition, const FRotator& InRotation, const FVector& InVelocity, bool bInJustTeleported,
	                 const float& InWorldTime, const float& InTimeStamp);

	FStoredPositions();

	UPROPERTY(BlueprintReadWrite)
	FVector Position;

	UPROPERTY(BlueprintReadWrite)
	FRotator Rotation;

	UPROPERTY(BlueprintReadWrite)
	FVector Velocity;

	UPROPERTY(BlueprintReadWrite)
	bool bJustTeleported;

	UPROPERTY(BlueprintReadWrite)
	float WorldTime;

	UPROPERTY(BlueprintReadWrite)
	float TimeStamp;

	FShapeData ShapeData;
};
