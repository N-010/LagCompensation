// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


namespace LagCompensationSystem
{
	LAGCOMPENSATION_API bool ConvertQueryImpactHit(const UWorld* World, const FHitLocation& PHit, FHitResult& OutResult,
	                                               float CheckLength,
	                                               const FCollisionFilterData& QueryFilter, const FVector& StartLoc,
	                                               const FVector& EndLoc,
	                                               const FPhysicsGeometry* Geom, const FTransform& QueryTM,
	                                               bool bReturnFaceIndex = false,
	                                               bool bReturnPhysMat = false);

	LAGCOMPENSATION_API FCollisionQueryParams ConfigureCollisionParams(FName TraceTag, bool bTraceComplex,
	                                                                   const TArray<TWeakObjectPtr<const AActor>>&
	                                                                   ActorsToIgnore, bool bIgnoreSelf,
	                                                                   UObject* WorldContextObject);

	template <typename T>
	LAGCOMPENSATION_API T TLerp(const T& A, const T& B, const float Alpha);

	template <>
	LAGCOMPENSATION_API FTransform TLerp(const FTransform& A, const FTransform& B, const float Alpha);

#if WITH_PHYSX
	template <>
	LAGCOMPENSATION_API FTransform TLerp(const FTransform& A, const FTransform& B, const float Alpha);
#endif //WITH_PHYSX

	template <>
	LAGCOMPENSATION_API FShapeData TLerp(const FShapeData& A, const FShapeData& B, const float Alpha);
}
