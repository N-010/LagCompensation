// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
/*#if WITH_PHYSX
#include "PhysicsInterfaceUtilsCore.h"
#endif //WIT_PHYSX
#if WITH_CHAOS
#include "PhysI"
#endif //WITH_CHAOS*/
#include "Physics/PhysicsInterfaceCore.h"

class UPositionStore;
/**
 * 
 */
namespace LagCompensationTypes
{
	static const FRotator InvalidRotation = FRotator(FLT_MAX);
	static const FQuat InvalidOrientation = FQuat(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
	static const FVector InvalidLocation = FVector(FLT_MAX);
	static const FVector InvalidDirection = FVector::ZeroVector;
	static const FVector InvalidVelocity = FVector::ZeroVector;
	static const float InvalidRange = -1.f;
	static const float InvalidTime = -FLT_MAX;
	static const float InfiniteInterval = -FLT_MAX;
	static const uint32 InvalidUnsignedID = uint32(INDEX_NONE);

	static const uint16 NumInlinedValidActors = 120;

	using FValidPositionStores = TArray<TWeakObjectPtr<UPositionStore>, TInlineAllocator<LagCompensationTypes::NumInlinedValidActors>>;
	using FValidActors = TArray<TWeakObjectPtr<const AActor>>;
	using FIgnoredActors = TArray<TWeakObjectPtr<const AActor>>;
	using FBodyInstances = TArray<FBodyInstance*>;
	using FPhysicsShapeAdapters = TArray<TSharedRef<FPhysicsShapeAdapter>>;
}

namespace PhysicsInterfaceTypes
{
	using FInlineTransformArray = TArray<FTransform, TInlineAllocator<NumInlinedPxShapeElements>>;
}
