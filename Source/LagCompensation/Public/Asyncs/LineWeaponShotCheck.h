// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "LineWeaponShotCheck.generated.h"

class UPositionStore;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLineWeaponShotCheck, bool, bSuccess);

UCLASS()
class ULineWeaponShotCheck : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	ULineWeaponShotCheck(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category="LagCompensation|Shot", meta=(BlueprintInternalUseOnly = "true", WorldContext="WorldContextObject"))
	static ULineWeaponShotCheck* LineWeaponShotCheck(UObject* WorldContextObject, UPositionStore* InOwnerPositionStore,
	                                          const FVector& InShootPosition,
	                                          const FVector& InShootDirection,
	                                          const float& InMaxDist, ETraceTypeQuery InTraceChannel,
	                                          bool bInTraceComplex,
	                                          const float& InPredictionTime = 0.f);

	void AsyncTrace(const FVector& EndTrace);

	virtual void Activate() override;

protected:
	virtual void OnEnd(const bool bResult);

	virtual void OnTraceEnd(const FTraceHandle& TraceHandle, FTraceDatum &TraceDatum);

public:
	UPROPERTY(BlueprintAssignable)
	FOnLineWeaponShotCheck OnFinish;

private:
	FTraceDelegate TraceDelegate;

	TWeakObjectPtr<UObject> WorldContext;
	TWeakObjectPtr<UPositionStore> OwnerPositionStore;
	FVector ShootPosition;
	FVector ShootDirection;
	float MaxDist;
	ETraceTypeQuery TraceChannel;
	bool bTraceComplex;
	float PredictionTime;
};
