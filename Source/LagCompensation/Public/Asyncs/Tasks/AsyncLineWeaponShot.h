#pragma once

#include "Async/AsyncWork.h"

class UPositionStore;

class FAsyncLineWeaponShot : public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask<FAsyncLineWeaponShot>;
	using Super = FNonAbandonableTask;

private:
	TWeakObjectPtr<UObject> WorldContext;
	TWeakObjectPtr<UPositionStore> OwnerPositionStore;
	FVector ShootPosition;
	FVector ShootDirection;
	float MaxDist;
	ETraceTypeQuery TraceChannel;
	bool bTraceComplex;
	float PredictionTime;

public:
	FAsyncLineWeaponShot(UObject* WorldContextObject, UPositionStore* InOwnerPositionStore,
											const FVector& InShootPosition,
											const FVector& InShootDirection,
											const float& InMaxDist, ETraceTypeQuery InTraceChannel,
											bool bInTraceComplex,
											const float& InPredictionTime = 0.f);
};
