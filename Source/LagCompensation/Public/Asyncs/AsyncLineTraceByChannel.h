// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Data/Types.h"
#include "Kismet/BlueprintAsyncActionBase.h"

#include "AsyncLineTraceByChannel.generated.h"

/**
 * 
 */
UCLASS()
class LAGCOMPENSATION_API UAsyncTrace : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

protected:
	LagCompensationTypes::FIgnoredActors* IgnoredActors = nullptr;;

	FTraceDelegate TraceDelegate;

	TWeakObjectPtr<UObject> WorldContext;

	EAsyncTraceType AsyncTraceType;

	FVector Start;

	FVector End;

	ETraceTypeQuery TraceChannel;

	bool bTraceComplex;

protected:
	void AsyncLineTraceByChannel();

	virtual void OnTraceEnd(const FTraceHandle& TraceHandle, FTraceDatum& TraceDatum) PURE_VIRTUAL(UAsyncTrace::OnTraceEnd, );
};
