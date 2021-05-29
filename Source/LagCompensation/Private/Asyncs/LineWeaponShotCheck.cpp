// Fill out your copyright notice in the Description page of Project Settings.


#include "Asyncs/LineWeaponShotCheck.h"


#include "LagCompensationSystem.h"
#include "PositionStore.h"
#include "Data/Types.h"
#include "Kismet/KismetSystemLibrary.h"

ULineWeaponShotCheck::ULineWeaponShotCheck(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

ULineWeaponShotCheck* ULineWeaponShotCheck::LineWeaponShotCheck(UObject* WorldContextObject,
                                                                UPositionStore* InOwnerPositionStore,
                                                                const FVector& InShootPosition,
                                                                const FVector& InShootDirection, const float& InMaxDist,
                                                                ETraceTypeQuery InTraceChannel, bool bInTraceComplex,
                                                                const float& InPredictionTime)
{
	using namespace LagCompensationTypes;
	ULineWeaponShotCheck* AsyncAction = nullptr;

	if (IsValid(WorldContextObject) && IsValid(InOwnerPositionStore) && InShootPosition != InvalidLocation &&
		InShootDirection != InvalidDirection &&
		InMaxDist > 0)
	{
		AsyncAction = NewObject<ULineWeaponShotCheck>();
		
		AsyncAction->RegisterWithGameInstance(WorldContextObject);
		AsyncAction->WorldContext = WorldContextObject;
		AsyncAction->OwnerPositionStore = InOwnerPositionStore;
		AsyncAction->ShootPosition = InShootPosition;
		AsyncAction->ShootDirection = InShootDirection;
		AsyncAction->MaxDist = InMaxDist;
		AsyncAction->TraceChannel = InTraceChannel;
		AsyncAction->bTraceComplex = bInTraceComplex;
		AsyncAction->PredictionTime = InPredictionTime;
	}

	return AsyncAction;
}

void ULineWeaponShotCheck::AsyncTrace(const FVector& EndTrace)
{
	if (UWorld* World = OwnerPositionStore->GetWorld())
	{
		TraceDelegate.BindUObject(this, &ULineWeaponShotCheck::OnTraceEnd);

		static const FName ShotCheckLineTraceSingleName(TEXT("ShotCheckLineTraceSingle"));
		const FCollisionQueryParams QueryParams = LagCompensationSystem::ConfigureCollisionParams(
			ShotCheckLineTraceSingleName, bTraceComplex, OwnerPositionStore->ValidActors, true, this);
		const ECollisionChannel CollisionChannel = UEngineTypes::ConvertToCollisionChannel(TraceChannel);

		World->AsyncLineTraceByChannel(EAsyncTraceType::Test, ShootPosition, EndTrace, CollisionChannel, QueryParams,
		                               FCollisionResponseParams::DefaultResponseParam, &TraceDelegate);
	}
}

void ULineWeaponShotCheck::Activate()
{
	Super::Activate();

	TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("Async LineWeaponShotCheck"));

	TWeakObjectPtr<UWorld> OwnerWorld = OwnerPositionStore.IsValid() ? OwnerPositionStore->GetWorld() : nullptr;
	if (!OwnerWorld.IsValid())
	{
		OnEnd(false);
		return;
	}

	LagCompensationTypes::FValidActors& ValidActors = OwnerPositionStore->ValidActors;

	int32 LastIndex = -1;
	TArray<FVector> HitLocation;

	FCriticalSection HitMutex;
	FCriticalSection ValidActorMutex;

	const ECollisionChannel ShotCollisionChannel = UEngineTypes::ConvertToCollisionChannel(TraceChannel);

	for (int32 LevelIdx = 0; LevelIdx < OwnerWorld->GetLevels().Num(); ++LevelIdx)
	{
		TWeakObjectPtr<ULevel> CurrentLevel = OwnerWorld->GetLevels()[LevelIdx];
		ParallelFor(CurrentLevel->Actors.Num(), [&](int PSIdx)
		            {
			            AActor* Actor = CurrentLevel->Actors[PSIdx];
			            if (IsValid(Actor) && Actor->GetIsReplicated() && !Actor->IsHidden())
			            {
				            if (UPositionStore* PositionStore = Cast<UPositionStore>(Actor->GetComponentByClass(
					            UPositionStore::StaticClass())))
				            {
					            const auto FoundBodyInstance = PositionStore->BodyInstances.FindByPredicate(
						            [&](const FBodyInstance* BodyInstance)
						            {
							            return BodyInstance->GetCollisionEnabled(false) && BodyInstance->
								            GetCollisionResponse().GetResponse(ShotCollisionChannel) == ECR_Block;
						            });

					            if (FoundBodyInstance)
					            {
						            FHitResult HitResult;
						            if (PositionStore->HitByPrediction(
							            ShootPosition, ShootDirection, MaxDist, HitResult,
							            PredictionTime))
						            {
							            HitMutex.Lock();
							            HitLocation.Add(HitResult.Location);
							            HitMutex.Unlock();
						            }
					            }

					            ValidActorMutex.Lock();
					            if (++LastIndex >= ValidActors.Max())
					            {
						            ValidActors.AddDefaulted(
							            ValidActors.Max() + LagCompensationTypes::NumInlinedValidActors);
					            }

					            ValidActors[LastIndex] = Actor;
					            ValidActorMutex.Unlock();
				            }
			            }
		            }
		);
	}

	if (HitLocation.Num() - 1 > 0)
	{
		const FTransform OwnerTransform(ShootPosition);
		HitLocation.Sort([&OwnerTransform](const FVector& A, const FVector& B)
		{
			return FVector::DistSquared(OwnerTransform.GetLocation(), A) < FVector::DistSquared(
				OwnerTransform.GetLocation(), B);
		});
	}

	if (HitLocation.Num() > 0)
	{
		TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("LineTraceCheck"));
		FHitResult OutHitResult;
		const FVector& EndTrace = HitLocation[0];

		AsyncTrace(EndTrace);
	}
	else
	{
		OnEnd(false);
	}
}

void ULineWeaponShotCheck::OnEnd(const bool bResult)
{
	if (OnFinish.IsBound())
	{
		OnFinish.Broadcast(bResult);
	}

	SetReadyToDestroy();
}

void ULineWeaponShotCheck::OnTraceEnd(const FTraceHandle& TraceHandle, FTraceDatum& TraceDatum)
{
	OnEnd(TraceDatum.OutHits.Num() <= 0);
}
