// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Data/ProjectileData.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "ProjectileShotCheck.generated.h"

class UPositionStore;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnProjectilShotCheckFinish, const FHitResult, HitResult, bool, bSuccess);

UCLASS()
class LAGCOMPENSATION_API UProjectileShotCheck : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
protected:
	FTraceDelegate TraceDelegate;
	FOnProjectilShotCheckFinish OnFinish;

	TWeakObjectPtr<UObject> WorldContext;
	TWeakObjectPtr<UPositionStore> OwnerPositionStore;
	TOptional<FProjectileData> ProjectileData;
	ETraceTypeQuery TraceChannel;
	bool bTraceComplex;
	float PredictionTime;
	float MaxDist;

public:
	UFUNCTION(BlueprintCallable, Category="LagCompensation|Shot",
		meta=(BlueprintInternalUseOnly = "true", WorldContext="WorldContextObject"))
	static UProjectileShotCheck* ProjectileShotCheck(UObject* WorldContextObject, FProjectileData& InProjectileData,
	                                                 UPositionStore* InOwnerPositionStore, ETraceTypeQuery InTraceChannel,
	                                                 bool bInTraceComplex,
	                                                 const float& InPredictionTime = 0.f);

protected:
	virtual void OnEnd(const FHitResult& HitResult, bool bResult);
	
	virtual void Activate() override;
};
