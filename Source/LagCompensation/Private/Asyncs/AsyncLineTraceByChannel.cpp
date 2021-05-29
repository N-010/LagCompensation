// Fill out your copyright notice in the Description page of Project Settings.


#include "Asyncs/AsyncLineTraceByChannel.h"

#include "LagCompensationSystem.h"

void UAsyncTrace::AsyncLineTraceByChannel()
{
	if (GEngine && IgnoredActors != nullptr)
	{
		if (UWorld* World = GEngine->
			GetWorldFromContextObject(WorldContext.Get(), EGetWorldErrorMode::LogAndReturnNull))
		{
			TraceDelegate.BindUObject(this, &ThisClass::OnTraceEnd);
			static const FName ShotCheckLineTraceSingleName(TEXT("AsyncLineTrace"));
			const FCollisionQueryParams QueryParams = LagCompensationSystem::ConfigureCollisionParams(
				ShotCheckLineTraceSingleName, bTraceComplex, *IgnoredActors, true, this);
			const ECollisionChannel CollisionChannel = UEngineTypes::ConvertToCollisionChannel(TraceChannel);
			World->AsyncLineTraceByChannel(EAsyncTraceType::Test, Start, End, CollisionChannel, QueryParams,
			                               FCollisionResponseParams::DefaultResponseParam, &TraceDelegate);

			return;
		}
	}
	
	const FTraceHandle TraceHandle;
	FTraceDatum Datum;
	OnTraceEnd(TraceHandle, Datum);
}
