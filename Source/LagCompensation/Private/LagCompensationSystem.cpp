// Fill out your copyright notice in the Description page of Project Settings.


#include "LagCompensationSystem.h"

#include "Data/StoredPositions.h"
#include "Kismet/KismetMathLibrary.h"
#include "PhysicsEngine/PhysicsSettings.h"

#if WITH_PHYSX
#include "PhysXInterfaceWrapperCore.h"
#endif //WITH_PHYSX

#if WITH_CHAOS
#include "ChaosInterfaceWrapperCore.h"
#endif //WITH_CHAOS

bool LagCompensationSystem::ConvertQueryImpactHit(const UWorld* World, const FHitLocation& PHit, FHitResult& OutResult,
                                                  const float CheckLength, const FCollisionFilterData& QueryFilter,
                                                  const FVector& StartLoc, const FVector& EndLoc,
                                                  const FPhysicsGeometry* Geom, const FTransform& QueryTM,
                                                  bool bReturnFaceIndex, bool bReturnPhysMat)
{
	const FHitFlags Flags = GetFlags(PHit);
	checkSlow(Flags & EHitFlags::Distance);

	const bool bInitialOverlap = HadInitialOverlap(PHit);

	const float HitTime = GetDistance(PHit) / CheckLength;
	OutResult.Time = HitTime;
	OutResult.Distance = GetDistance(PHit);
	const FVector TraceStartToEnd = EndLoc - StartLoc;
	const FVector SafeLocationToFitShape = StartLoc + (HitTime * TraceStartToEnd);
	OutResult.Location = SafeLocationToFitShape;

	const bool bUseReturnedPoint = ((Flags & EHitFlags::Position) && !bInitialOverlap);
	FVector Position = StartLoc;
	if (bUseReturnedPoint)
	{
		Position = GetPosition(PHit);
		if (Position.ContainsNaN())
		{
			OutResult.Reset();
			return false;
		}
	}
	OutResult.ImpactPoint = Position;

	OutResult.TraceStart = StartLoc;
	OutResult.TraceEnd = EndLoc;

	return true;
}

FCollisionQueryParams LagCompensationSystem::ConfigureCollisionParams(FName TraceTag, bool bTraceComplex,
                                                                      const TArray<TWeakObjectPtr<const AActor>>&
                                                                      ActorsToIgnore, bool bIgnoreSelf,
                                                                      UObject* WorldContextObject)
{
	FCollisionQueryParams Params(TraceTag, SCENE_QUERY_STAT_ONLY(KismetTraceUtils), bTraceComplex);
	Params.bReturnPhysicalMaterial = true;
	Params.bReturnFaceIndex = !UPhysicsSettings::Get()->bSuppressFaceRemapTable;
	// Ask for face index, as long as we didn't disable globally
	Params.AddIgnoredActors(ActorsToIgnore);
	if (bIgnoreSelf)
	{
		AActor* IgnoreActor = Cast<AActor>(WorldContextObject);
		if (IgnoreActor)
		{
			Params.AddIgnoredActor(IgnoreActor);
		}
		else
		{
			// find owner
			UObject* CurrentObject = WorldContextObject;
			while (CurrentObject)
			{
				CurrentObject = CurrentObject->GetOuter();
				IgnoreActor = Cast<AActor>(CurrentObject);
				if (IgnoreActor)
				{
					Params.AddIgnoredActor(IgnoreActor);
					break;
				}
			}
		}
	}

	return Params;
}

template <typename T>
T LagCompensationSystem::TLerp(const T& A, const T& B, const float Alpha)
{
	return FMath::Lerp(A, B, Alpha);
}

template <>
FTransform LagCompensationSystem::TLerp(const FTransform& A, const FTransform& B, const float Alpha)
{
	return UKismetMathLibrary::TLerp(A, B, Alpha);
}

#if WITH_PHYSX
template <>
PxTransform LagCompensationSystem::TLerp(const PxTransform& A, const PxTransform& B, const float Alpha)
{
	const FTransform UEATransform = P2UTransform(A);
	const FTransform UEBTransform = P2UTransform(B);
	return U2PTransform(TLerp(UEATransform, UEBTransform, Alpha));
}
#endif //WITH_PHYSX

template <>
FShapeData LagCompensationSystem::TLerp(const FShapeData& A, const FShapeData& B, const float Alpha)
{
	FShapeData LerpShapeData(A);
	if (A.IsValidShapeData() && B.IsValidShapeData() && A.ShapeTransforms.Num() == B.ShapeTransforms.Num())
	{
		PhysicsInterfaceTypes::FInlineTransformArray LerpShapeTransforms;
		LerpShapeTransforms.Reserve(A.ShapeTransforms.Num());

		for (int32 i = 0; i < A.ShapeTransforms.Num(); ++i)
		{
			LerpShapeTransforms.Add(TLerp(A.ShapeTransforms[i], B.ShapeTransforms[i], Alpha));
		}

		LerpShapeData = LerpShapeTransforms;
	}

	return LerpShapeData;
}
