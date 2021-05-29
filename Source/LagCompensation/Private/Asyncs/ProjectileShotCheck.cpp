// Fill out your copyright notice in the Description page of Project Settings.


#include "Asyncs/ProjectileShotCheck.h"

#include "PositionStore.h"

UProjectileShotCheck* UProjectileShotCheck::ProjectileShotCheck(UObject* WorldContextObject,
                                                                FProjectileData& InProjectileData,
                                                                UPositionStore* InOwnerPositionStore,
                                                                ETraceTypeQuery InTraceChannel,
                                                                bool bInTraceComplex, const float& InPredictionTime)
{
	UProjectileShotCheck* AsyncAction = nullptr;

	if (IsValid(WorldContextObject) && IsValid(InOwnerPositionStore) && FProjectileData::IsValidProjectileData(
		InProjectileData))
	{
		AsyncAction = NewObject<UProjectileShotCheck>();
		AsyncAction->RegisterWithGameInstance(WorldContextObject);

		AsyncAction->WorldContext = WorldContextObject;
		AsyncAction->OwnerPositionStore = InOwnerPositionStore;
		AsyncAction->ProjectileData = InProjectileData;
		AsyncAction->TraceChannel = InTraceChannel;
		AsyncAction->bTraceComplex = bInTraceComplex;
		AsyncAction->PredictionTime = InPredictionTime;
		AsyncAction->MaxDist = ((InProjectileData.StartPosition + (InProjectileData.Velocity * InPredictionTime * 2)) -
			InProjectileData.StartPosition).Size();
	}

	return AsyncAction;
}

void UProjectileShotCheck::OnEnd(const FHitResult& HitResult, bool bResult)
{
	if (OnFinish.IsBound())
	{
		OnFinish.Broadcast(HitResult, bResult);
	}

	SetReadyToDestroy();
}


void UProjectileShotCheck::Activate()
{
	Super::Activate();

	TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("Async ProjectileShotCheck"));

	TWeakObjectPtr<UWorld> OwnerWorld = OwnerPositionStore.IsValid() ? OwnerPositionStore->GetWorld() : nullptr;
	if (!OwnerWorld.IsValid() || !ProjectileData.IsSet())
	{
		OnEnd(FHitResult(), false);
		return;
	}

	LagCompensationTypes::FValidActors& ValidActors = OwnerPositionStore->GetMutableValidActors();

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
					            const auto FoundBodyInstance = PositionStore->GetBodyInstances().FindByPredicate(
						            [&](const FBodyInstance* BodyInstance)
						            {
							            return BodyInstance->GetCollisionEnabled(false) && BodyInstance->
								            GetCollisionResponse().GetResponse(ShotCollisionChannel) == ECR_Block;
						            });

					            if (FoundBodyInstance)
					            {
						            FHitResult HitResult;
						            if (false)
						            {
							            HitMutex.Lock();
							            HitLocation.Add(HitResult.Location);
							            HitMutex.Unlock();
						            }
					            }

					            /* ValidActorMutex.Lock();
					             if (++LastIndex >= ValidActors.Max())
					             {
						             ValidActors.AddDefaulted(
							             ValidActors.Max() + LagCompensationTypes::NumInlinedValidActors);
					             }
 
					             ValidActors[LastIndex] = Actor;
					             ValidActorMutex.Unlock();*/
				            }
			            }
		            }
		);
	}

	/*if (HitLocation.Num() - 1 > 0)
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
		OnEnd({}, false);
	}*/
}
