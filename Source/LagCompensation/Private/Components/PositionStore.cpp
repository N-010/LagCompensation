// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PositionStore.h"

#if WITH_PHYSX
#include "PhysXPublicCore.h"
#include "PxGeometryQuery.h"
#include "PhysXInterfaceWrapperCore.h"
#endif //WITH_PHYSX

#include "Data/Types.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "EngineUtils.h"
#include "LagCompensationSystem.h"

#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UPositionStore::UPositionStore(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(false);
	bWantsInitializeComponent = true;
}

void UPositionStore::OnCallMovementUpdateDelegate(float DeltaTime, const FVector& OldLocation,
                                                  const FVector& OldVelocity)
{
	const float WorldTime = GetWorld()->GetTimeSeconds();

	if (GetCharacterMovement())
	{
		const bool bIsPlayer = GetCharacterOwner()->IsPlayerControlled();
		if (!bIsPlayer && StoredPositions.Num() > 0 && (StoredPositions.Last().WorldTime +
			BotSavedPositionInterval) > WorldTime)
		{
			return;
		}

		StoredPositions.Add(GetCurrentStoredPosition());

		/*if (BodyInstances.Num() > 0)
		{
			new(StoredPositions)FStoredPositions(BodyInstances, GetOwnerLocation(),
			                                     GetOwnerViewRotation(), GetOwnerVelocity(),
			                                     GetOwnerJustTeleported(), WorldTime,
			                                     GetCurrentSyncTime());
		}
		else
		{
			new(StoredPositions)FStoredPositions(GetOwnerLocation(),
			                                     GetOwnerViewRotation(), GetOwnerVelocity(),
			                                     GetOwnerJustTeleported(), WorldTime,
			                                     GetCurrentSyncTime());
		}*/
	}

	// maintain one position beyond MaxSavedPositionAge for interpolation
	if (StoredPositions.Num() > 1 && StoredPositions[1].WorldTime < WorldTime - MaxSavedPositionAge)
	{
		StoredPositions.RemoveAt(0);
	}
}

bool UPositionStore::GetComponentClassCanReplicate() const
{
	return false;
}

void UPositionStore::InitializeComponent()
{
	Super::InitializeComponent();

	if (GetOwnerRole() < ROLE_Authority && !NeedsLoadForClient())
	{
		DestroyComponent();
		return;
	}

	if (ACharacter* CharacterOwner = GetCharacterOwner())
	{
		TScriptDelegate<FWeakObjectPtr> CharacterMovementUpdated;
		CharacterMovementUpdated.BindUFunction(this, "OnCallMovementUpdateDelegate");
		CharacterOwner->OnCharacterMovementUpdated.AddUnique(CharacterMovementUpdated);
	}

	ValidActors.AddDefaulted(LagCompensationTypes::NumInlinedValidActors);
}

void UPositionStore::PostInitProperties()
{
	Super::PostInitProperties();
	MaxSavedPositionAge = MaxPredictionPing / 1000.f; // to seconds 
}

bool UPositionStore::NeedsLoadForClient() const
{
	check(GetOuter());
	// For Component Blueprints, avoid calling into the class to avoid recursion
	const bool bNeedsLoadOuter = HasAnyFlags(RF_ClassDefaultObject) || bNetLoadOnClient;
	return (!IsEditorOnly() && bNeedsLoadOuter && UObject::NeedsLoadForClient());
}

ACharacter* UPositionStore::GetCharacterOwner() const
{
	return Cast<ACharacter>(GetOwner());
}

UCharacterMovementComponent* UPositionStore::GetCharacterMovement() const
{
	ACharacter* Character = GetCharacterOwner();
	return IsValid(Character) ? Character->GetCharacterMovement() : nullptr;
}

APlayerState* UPositionStore::GetPlayerStateOwner() const
{
	return IsValid(GetCharacterOwner()) ? GetCharacterOwner()->GetPlayerState() : nullptr;
}

FVector UPositionStore::GetOwnerLocation() const
{
	const AActor* Owner = GetOwner();
	return IsValid(Owner) ? Owner->GetActorLocation() : LagCompensationTypes::InvalidLocation;
}

FTransform UPositionStore::GetOwnerTransform() const
{
	return IsValid(GetOwner()) ? GetOwner()->GetActorTransform() : FTransform::Identity;
}

FShapeData UPositionStore::GetOwnerShapeData() const
{
	return FShapeData(BodyInstances);
}

FRotator UPositionStore::GetOwnerViewRotation() const
{
	const APawn* PawnOwner = Cast<APawn>(GetOwner());
	if (IsValid(PawnOwner))
	{
		return PawnOwner->GetViewRotation();
	}

	const AActor* ActorOwner = Cast<APawn>(GetOwner());
	return IsValid(ActorOwner) ? ActorOwner->GetActorRotation() : LagCompensationTypes::InvalidRotation;
}

const FVector& UPositionStore::GetOwnerVelocity() const
{
	UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement();
	return IsValid(CharacterMovementComponent)
		       ? CharacterMovementComponent->Velocity
		       : LagCompensationTypes::InvalidVelocity;
}

bool UPositionStore::GetOwnerJustTeleported() const
{
	UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement();
	return IsValid(CharacterMovementComponent)
		       ? CharacterMovementComponent->bJustTeleported
		       : false;
}

float UPositionStore::GetCurrentMovementTime() const
{
	ACharacter* CharacterOwner = GetCharacterOwner();
	const UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement();
	if (!IsValid(CharacterOwner) || !IsValid(CharacterMovementComponent))
	{
		return GetOwner()->GetWorld()->GetTimeSeconds();
	}

	if (((GetOwner()->GetLocalRole() == ROLE_AutonomousProxy) || (GetNetMode() == NM_DedicatedServer) || ((GetNetMode()
		== NM_ListenServer) && !GetCharacterOwner()->IsLocallyControlled())))
	{
		if (FNetworkPredictionData_Server_Character* ServerData = CharacterMovementComponent->
			GetPredictionData_Server_Character())
		{
			return ServerData->CurrentClientTimeStamp;
		}
	}

	return GetOwner()->GetWorld()->GetTimeSeconds();
}

FStoredPositions UPositionStore::GetCurrentStoredPosition() const
{
	const float WorldTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;

	return FStoredPositions(BodyInstances, GetOwnerLocation(),
	                        GetOwnerViewRotation(), GetOwnerVelocity(),
	                        GetOwnerJustTeleported(), WorldTime,
	                        GetCurrentSyncTime());
}

FORCEINLINE LagCompensationTypes::FValidActors& UPositionStore::GetMutableValidActors()
{
	return ValidActors;
}

FORCEINLINE const LagCompensationTypes::FValidActors& UPositionStore::GetValidActors() const
{
	return ValidActors;
}

float UPositionStore::GetCurrentSyncTime() const
{
	const AActor* ActorOwner = GetOwner();
	if (IsValid(ActorOwner))
	{
		if (ActorOwner->GetLocalRole() < ROLE_Authority)
		{
			const UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement();
			if (IsValid(CharacterMovementComponent))
			{
				if (const FNetworkPredictionData_Client_Character* ClientData = CharacterMovementComponent->
					GetPredictionData_Client_Character())
				{
					return ClientData->CurrentTimeStamp;
				}
			}
		}
	}

	return GetCurrentMovementTime();
}

const TArray<FStoredPositions>& UPositionStore::GetStoredPositions() const
{
	return StoredPositions;
}

float UPositionStore::GetPredictionTime(const bool bIsUseExactPingV2) const
{
	const APlayerState* PlayerStateOwner = GetPlayerStateOwner();
	/**
	 * Ping * 0.0005.f - Time in seconds of packet transmission in one direction
	 */
	if (IsValid(PlayerStateOwner) && GetNetMode() != NM_Standalone)
	{
		const float PingValue = bIsUseExactPingV2 ? PlayerStateOwner->ExactPingV2 : PlayerStateOwner->ExactPing;
		return FMath::Clamp(PingValue - PredictionFudgeFactor, 0.f, MaxPredictionPing) * 0.0005f;
	}

	return 0.f;
}

float UPositionStore::GetPredictionFudgeFactor() const
{
	return PredictionFudgeFactor;
}

void UPositionStore::SetPredictionFudgeFactor(const float NewPredictionFudgeFactor)
{
	this->PredictionFudgeFactor = NewPredictionFudgeFactor;
}

FVector UPositionStore::GetPredictionOwnerLocation(const float PredictionTime)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("UPositionStore::GetPredictionOwnerLocation"));

	FVector OwnerLocation = GetOwnerLocation();

	if (PredictionTime > 0.f && StoredPositions.Num() > 0)
	{
		const float Time = GetWorld()->GetTimeSeconds() - PredictionTime;

		const int32 Index = StoredPositions.FindLastByPredicate([&](const FStoredPositions& Item)
		{
			return Item.WorldTime < Time;
		});

		if (Index != INDEX_NONE)
		{
			OwnerLocation = StoredPositions[Index].Position;
			if (!StoredPositions[Index].bJustTeleported && (Index < StoredPositions.Num() - 1))
			{
				const float StoredTime = StoredPositions[Index].WorldTime;
				const float PostStoredTime = StoredPositions[Index + 1].WorldTime;

				if (StoredTime == PostStoredTime)
				{
					OwnerLocation = StoredPositions[Index].Position;
				}
				else
				{
					const float Alpha = (Time - StoredTime) / (PostStoredTime - StoredTime);
					OwnerLocation = FMath::Lerp(StoredPositions[Index].Position, StoredPositions[Index + 1].Position,
					                            Alpha);
				}
			}
		}
		else if (StoredPositions.Num() > 0)
		{
			OwnerLocation = StoredPositions[0].Position;
		}
	}

	return OwnerLocation;
}

FStoredPositions UPositionStore::GetStoredPositionInTime(const float PredictionTime)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("UPositionStore::GetStoredPositionInTime"));

	FStoredPositions StoredPosition = GetCurrentStoredPosition();

	if (PredictionTime > 0.f && StoredPositions.Num() > 0)
	{
		const float Time = GetWorld()->GetTimeSeconds() - PredictionTime;

		const int32 Index = StoredPositions.FindLastByPredicate([&](const FStoredPositions& Item)
		{
			return Item.WorldTime < Time;
		});

		if (Index != INDEX_NONE)
		{
			StoredPosition = StoredPositions[Index];
		}
		else if (StoredPositions.Num() > 0)
		{
			StoredPosition = StoredPositions[0];
		}
	}

	return StoredPosition;
}

FShapeData UPositionStore::GetPredictionShapeData(const float PredictionTime)
{
	using namespace PhysicsInterfaceTypes;

	TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("UPositionStore::GetPredictionShapeData"));

	TOptional<FShapeData> CurrentShapeData;

	if (PredictionTime > 0.f && StoredPositions.Num() > 0)
	{
		const float Time = GetWorld()->GetTimeSeconds() - PredictionTime;

		const int32 Index = StoredPositions.FindLastByPredicate([&](const FStoredPositions& Item)
		{
			return Item.WorldTime < Time;
		});

		if (Index != INDEX_NONE)
		{
			CurrentShapeData = StoredPositions[Index].ShapeData;

			if (!StoredPositions[Index].bJustTeleported && (Index < StoredPositions.Num() - 1))
			{
				const float StoredTime = StoredPositions[Index].WorldTime;
				const float PostStoredTime = StoredPositions[Index + 1].WorldTime;

				if (StoredTime == PostStoredTime)
				{
					CurrentShapeData = StoredPositions[Index].ShapeData;
				}
				else
				{
					const float Alpha = (Time - StoredTime) / (PostStoredTime - StoredTime);
					FInlineTransformArray LerpTransforms;

					for (int i = 0; i < StoredPositions[Index].ShapeData.ShapeTransforms.Num(); ++i)
					{
						LerpTransforms.Add(LagCompensationSystem::TLerp(
							StoredPositions[Index].ShapeData.ShapeTransforms[i],
							StoredPositions[Index + 1].ShapeData.ShapeTransforms[i],
							Alpha));
					}

					*CurrentShapeData = MoveTemp(LerpTransforms);
				}
			}
		}
		else if(StoredPositions.Num() > 0)
		{
			CurrentShapeData = StoredPositions[0].ShapeData;
		}
	}

	return CurrentShapeData.IsSet() ? *CurrentShapeData : FShapeData(BodyInstances);
}

bool UPositionStore::HitByPrediction(const FVector& StartPos, const FVector& Dir, const float MaxDist,
                                     FHitResult& OutHit,
                                     const float& PredictionTimes)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("HitByPrediction"));

	if (StoredPositions.Num() <= 0)
	{
		return false;
	}

	int32 NumHits = 0;
	const PxU32 MaxHits = 1;
	FHitRaycast PHits[MaxHits];
	FPhysicsGeometry* Geometry = nullptr;
	const FPhysicsShapeHandle* ShapeHandle = nullptr;
	FPhysicsTransform WorldTransform;

	const FShapeData PredictShapeData = GetPredictionShapeData(PredictionTimes);

#if WITH_PHYSX
	const PxHitFlags PHitFlags = PxHitFlag::eDEFAULT;

	for (int32 i = 0; i < PredictShapeData.Shapes.Num(); ++i)
	{
		WorldTransform = U2PTransform(PredictShapeData.ShapeTransforms[i]);
		ShapeHandle = &PredictShapeData.Shapes[i];
		if (ShapeHandle)
		{
			Geometry = &(ShapeHandle->Shape->getGeometry().any());

			NumHits = PxGeometryQuery::raycast(U2PVector(StartPos), U2PVector(Dir), *Geometry,
			                                   WorldTransform, MaxDist, PHitFlags, MaxHits,
			                                   &PHits[0]);
			if (NumHits > 0)
			{
				FCollisionFilterData QueryFilter;
				QueryFilter.Word2 = 0xFFFFF;
				const FVector EndPos = StartPos + (Dir * MaxDist);
				LagCompensationSystem::ConvertQueryImpactHit(GetWorld(), PHits[0], OutHit, MaxDist, QueryFilter,
				                                             StartPos,
				                                             EndPos, Geometry, FTransform(StartPos), false, false);

				break;
			}
		}
	}

#endif //WITH_PHYSX

	return NumHits > 0;
}

bool UPositionStore::ProjectileHitByPrediction(const FProjectileData& ProjectileData, FHitResult& OutHit,
                                               const float& PredictionTimes)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("ProjectileHitByPrediction"));

	if (StoredPositions.Num() <= 0 || !FProjectileData::IsValidProjectileData(ProjectileData))
	{
		return false;
	}

	const FStoredPositions StoredPositionInTime = GetStoredPositionInTime(PredictionTimes);
	const FShapeData OwnerPredictionShapeData = GetPredictionShapeData(PredictionTimes);
	const FVector PredictionOwnerVelocity = StoredPositionInTime.Velocity;
	const FVector RelativeVelocity = ProjectileData.Velocity - PredictionOwnerVelocity;
	const float Ping = PredictionTimes * 2;
	float MaxDist = ((ProjectileData.StartPosition + (RelativeVelocity * Ping)) -
		ProjectileData.StartPosition).Size();


	FHitSweep HitSweep;

	FShapeData ProjectileShapeDataGlobalTransform(ProjectileData.ProjectileShapeData);

	if (ProjectileData.ProjectileShapeData.bLocalTransform)
	{
		ProjectileShapeDataGlobalTransform.ComposeTransform(FTransform{ProjectileData.StartPosition});
		ProjectileShapeDataGlobalTransform.bLocalTransform = false;
	}


	if (Sweep(ProjectileShapeDataGlobalTransform, OwnerPredictionShapeData, RelativeVelocity.GetSafeNormal(), MaxDist,
	          HitSweep))
	{
		const float HitAlpha = (HitSweep.distance / MaxDist);
		const FShapeData CurrentOwnerShapeData = GetOwnerShapeData();
		const FShapeData LerpOwnerShapeData = LagCompensationSystem::TLerp(
			OwnerPredictionShapeData, CurrentOwnerShapeData, HitAlpha);

		MaxDist = ((ProjectileData.StartPosition + (ProjectileData.Velocity * Ping)) -
			ProjectileData.StartPosition).Size();
		if (Sweep(ProjectileShapeDataGlobalTransform, LerpOwnerShapeData, ProjectileData.Velocity.GetSafeNormal(),
		          MaxDist, HitSweep))
		{
			return true;
		}
	}

	return false;
}

bool UPositionStore::Sweep(const FShapeData& A, const FShapeData& B, const FVector& UnitDirection, const float MaxDist,
                           FHitSweep& HitSweep) const
{
	if (!A.IsValidShapeData() || !B.IsValidShapeData() || UnitDirection.IsNearlyZero() || FMath::IsNearlyZero(MaxDist))
	{
		return false;
	}

	const FVector NormalUnitDirection = UnitDirection.IsNormalized() ? UnitDirection : UnitDirection.GetSafeNormal();

	bool bSweepResult = false;

	const FPhysicsGeometry* AGeometry = nullptr;
	const FPhysicsGeometry* BGeometry = nullptr;

	for (int32 ShapeAIdx = 0; ShapeAIdx < A.ShapeAdapters.Num(); ++ShapeAIdx)
	{
		const FTransform& ShapeATransform = A.ShapeTransforms[ShapeAIdx];
		AGeometry = &(A.ShapeAdapters[ShapeAIdx]->GetGeometry());
		if (!AGeometry)
		{
			continue;
		}

		for (int32 ShapeBIdx = 0; ShapeBIdx < B.ShapeAdapters.Num(); ++ShapeBIdx)
		{
			const FTransform& ShapeBTransform = B.ShapeTransforms[ShapeBIdx];
			BGeometry = &(B.ShapeAdapters[ShapeBIdx]->GetGeometry());
			if (!BGeometry)
			{
				continue;
			}

			HitSweep = FHitSweep();
#if WITH_PHYSX
			bSweepResult = PxGeometryQuery::sweep(U2PVector(NormalUnitDirection), MaxDist, *AGeometry,
			                                      U2PTransform(ShapeATransform), *BGeometry,
			                                      U2PTransform(ShapeBTransform), HitSweep);
			if (bSweepResult)
			{
				return true;
			}
#endif //WITH_PHYSX
		}
	}

	return false;
}
