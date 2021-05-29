// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/StoredPositions.h"

#include <PxShape.h>

#include "PhysicsInterfaceUtilsCore.h"
#include "PhysXPublicCore.h"
#include "Algo/ForEach.h"
#include "Kismet/KismetMathLibrary.h"

FShapeData::FShapeData()
	: bLocalTransform(false)
{
}

FShapeData::FShapeData(const LagCompensationTypes::FBodyInstances& BodyInstances, const bool bInLocalTransform)
	: bLocalTransform(bInLocalTransform)
{
	if (BodyInstances.Num() > 0)
	{
		for (const FBodyInstance* BodyInstance : BodyInstances)
		{
			if (BodyInstance->IsValidBodyInstance())
			{
				FillDataFromBodyInstance(BodyInstance, bInLocalTransform);
			}
		};
	}
}

FShapeData::FShapeData(const TArray<FBodyInstance>& BodyInstances, const bool bInLocalTransform)
	: bLocalTransform(bInLocalTransform)
{
	if (BodyInstances.Num() > 0)
	{
		for (const FBodyInstance& BodyInstance : BodyInstances)
		{
			if (BodyInstance.IsValidBodyInstance())
			{
				FillDataFromBodyInstance(&BodyInstance, bInLocalTransform);
			}
		};
	}
}

FShapeData::FShapeData(const FBodyInstance* BodyInstance, const bool bInLocalTransform)
	: bLocalTransform(bInLocalTransform)
{
	if (BodyInstance != nullptr && BodyInstance->IsValidBodyInstance())
	{
		FillDataFromBodyInstance(BodyInstance, bInLocalTransform);
	}
}

FShapeData::FShapeData(FShapeData&& ShapeData)
	: Shapes(MoveTemp(ShapeData.Shapes)),
	  ShapeTransforms(MoveTemp(ShapeData.ShapeTransforms)),
	  ShapeAdapters(MoveTemp(ShapeData.ShapeAdapters)),
	  bLocalTransform(MoveTemp(ShapeData.bLocalTransform))
{
}

FShapeData::FShapeData(const FShapeData& ShapeData)
	: Shapes(ShapeData.Shapes),
	  ShapeTransforms(ShapeData.ShapeTransforms),
	  ShapeAdapters(ShapeData.ShapeAdapters),
	  bLocalTransform(ShapeData.bLocalTransform)
{
}

FShapeData& FShapeData::operator=(const FShapeData& ShapeData)
{
	if (this != &ShapeData)
	{
		Shapes = ShapeData.Shapes;
		ShapeTransforms = ShapeData.ShapeTransforms;
		bLocalTransform = ShapeData.bLocalTransform;
		ShapeAdapters = ShapeData.ShapeAdapters;
	}

	return *this;
}

FShapeData& FShapeData::operator=(const PhysicsInterfaceTypes::FInlineTransformArray& OtherShapeTransforms)
{
	if (ShapeTransforms.GetData() != OtherShapeTransforms.GetData())
	{
		ShapeTransforms = OtherShapeTransforms;
	}
	return *this;
}

FShapeData& FShapeData::operator=(PhysicsInterfaceTypes::FInlineTransformArray&& OtherShapeTransforms)
{
	if (ShapeTransforms.GetData() != OtherShapeTransforms.GetData())
	{
		ShapeTransforms = MoveTemp(OtherShapeTransforms);
	}
	return *this;
}

FShapeData& FShapeData::operator=(const PhysicsInterfaceTypes::FInlineShapeArray& OtherShapes)
{
	if (Shapes.GetData() != OtherShapes.GetData())
	{
		Shapes = OtherShapes;
	}

	return *this;
}

FShapeData& FShapeData::operator=(PhysicsInterfaceTypes::FInlineShapeArray&& OtherShapes)
{
	if (Shapes.GetData() != OtherShapes.GetData())
	{
		Shapes = MoveTemp(OtherShapes);
	}

	return *this;
}

void FShapeData::FillDataFromBodyInstance(const FBodyInstance* BodyInstance, const bool bInLocalTransform)
{
	if (BodyInstance->IsValidBodyInstance() && BodyInstance->OwnerComponent.IsValid())
	{
		FPhysicsCommand::ExecuteRead(BodyInstance->ActorHandle, [&](const FPhysicsActorHandle& Actor)
		{
			PhysicsInterfaceTypes::FInlineShapeArray OutShapes;
			FPhysicsInterface::GetAllShapes_AssumedLocked(Actor, OutShapes);
			Shapes.Append(OutShapes);
			const FTransform ShapeTransform = bInLocalTransform
				                                  ? BodyInstance->OwnerComponent->GetRelativeTransform()
				                                  : BodyInstance->OwnerComponent->GetComponentTransform();
			for (const auto& OutShape : OutShapes)
			{
				ShapeTransforms.Add(ShapeTransform);
			};
			FPhysicsShapeAdapter ShapeAdapter(ShapeTransform.GetRotation(),
			                                  BodyInstance->OwnerComponent->GetCollisionShape());
			ShapeAdapters.Add(MakeShared<FPhysicsShapeAdapter>(ShapeAdapter));
		});
	}
}

void FShapeData::ComposeTransform(const FTransform& OtherTransform)
{
	for(FTransform& ShapeTransform : ShapeTransforms)
	{
		ShapeTransform = ShapeTransform * OtherTransform;
	}
}

FORCEINLINE bool FShapeData::IsValidShapeData() const
{
	return (Shapes.Num() > 0 || ShapeAdapters.Num() > 0) && ShapeTransforms.Num() > 0 && (Shapes.Num() == ShapeTransforms.Num() || ShapeAdapters.Num() == ShapeTransforms.Num());
}

FStoredPositions::FStoredPositions(const FVector& InPosition, const FRotator& InRotation, const FVector& InVelocity,
                                   bool bInJustTeleported, const float& InWorldTime, const float& InTimeStamp)
	: Position(InPosition),
	  Rotation(InRotation),
	  Velocity(InVelocity),
	  bJustTeleported(bInJustTeleported),
	  WorldTime(InWorldTime),
	  TimeStamp(InTimeStamp)
{
}

FStoredPositions::FStoredPositions(const TArray<FBodyInstance*>& BodyInstances, const FVector& InPosition,
                                   const FRotator& InRotation, const FVector& InVelocity,
                                   bool bInJustTeleported,
                                   const float& InWorldTime, const float& InTimeStamp)
	: Position(InPosition),
	  Rotation(InRotation),
	  Velocity(InVelocity),
	  bJustTeleported(bInJustTeleported),
	  WorldTime(InWorldTime),
	  TimeStamp(InTimeStamp),
	  ShapeData(BodyInstances)
{
}

FStoredPositions::FStoredPositions()
	: Position(FVector::ZeroVector),
	  Rotation(FRotator::ZeroRotator),
	  Velocity(FVector::ZeroVector),
	  bJustTeleported(false),
	  WorldTime(0.f),
	  TimeStamp(0.f)
{
}
