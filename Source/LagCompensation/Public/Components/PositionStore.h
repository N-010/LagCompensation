// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "BaseBodyComponent.h"
#include "Asyncs/LineWeaponShotCheck.h"
#include "Data/ProjectileData.h"
#include "Data/StoredPositions.h"

#include "PositionStore.generated.h"

class UCharacterMovementComponent;


UCLASS(ClassGroup=(LagCompensation), meta=(BlueprintSpawnableComponent))
class LAGCOMPENSATION_API UPositionStore : public UBaseBodyComponent
{
	GENERATED_BODY()

	friend class ULineWeaponShotCheck;

public:
	// Sets default values for this component's properties
	UPositionStore(const FObjectInitializer& ObjectInitializer);

protected:
	UFUNCTION()
	void OnCallMovementUpdateDelegate(float DeltaTime, const FVector& OldLocation, const FVector& OldVelocity);

public:
	//~ Begin UActorComponent Interfaces
	virtual bool GetComponentClassCanReplicate() const override;
	virtual void InitializeComponent() override;
	virtual void PostInitProperties() override;
	//~ End UActorComponent Interfaces

	//~ Begin UObject Interfaces
	virtual bool NeedsLoadForClient() const override;
	//~ End UObject Interfaces

	ACharacter* GetCharacterOwner() const;

	UCharacterMovementComponent* GetCharacterMovement() const;

	APlayerState* GetPlayerStateOwner() const;

	FVector GetOwnerLocation() const;
	
	FTransform GetOwnerTransform() const;

	FShapeData GetOwnerShapeData() const;

	FRotator GetOwnerViewRotation() const;

	const FVector& GetOwnerVelocity() const;

	bool GetOwnerJustTeleported() const;

	float GetCurrentMovementTime() const;

	FStoredPositions GetCurrentStoredPosition() const;

	FORCEINLINE LagCompensationTypes::FValidActors& GetMutableValidActors();
	FORCEINLINE  const LagCompensationTypes::FValidActors& GetValidActors() const;

	UFUNCTION(BlueprintPure, Category="Network|Time")
	float GetCurrentSyncTime() const;

	UFUNCTION(BlueprintCallable)
	const TArray<FStoredPositions>& GetStoredPositions() const;

	UFUNCTION(BlueprintPure, Category="Network|Time")
	float GetPredictionTime(const bool bIsUseExactPingV2 = false) const;

	UFUNCTION(BlueprintPure, Category="Network|Prediction")
	float GetPredictionFudgeFactor() const;

	UFUNCTION(BlueprintCallable, Category="Network|Prediction")
	void SetPredictionFudgeFactor(const float NewPredictionFudgeFactor);

	UFUNCTION(BlueprintCallable, Category="Network|Prediction")
	FVector GetPredictionOwnerLocation(const float PredictionTime = 0.f);

	UFUNCTION(BlueprintCallable, Category="Network|Prediction")
	FStoredPositions GetStoredPositionInTime(const float PredictionTime = 0.f);

	FShapeData GetPredictionShapeData(const float PredictionTime = 0.f);

	UFUNCTION(BlueprintPure, Category="Test")
	bool HitByPrediction(const FVector& StartPos, const FVector& Dir, const float MaxDist, FHitResult& OutHit, const float& PredictionTimes = 0.f);

	UFUNCTION(BlueprintPure, Category="Test")
	bool ProjectileHitByPrediction(const FProjectileData& ProjectileData, FHitResult& OutHit, const float& PredictionTimes = 0.f);

	virtual bool Sweep(const FShapeData& A, const FShapeData& B, const FVector& UnitDirection, const float MaxDist, FHitSweep& HitSweep) const;

protected:
	UPROPERTY(Transient)
	TArray<FStoredPositions> StoredPositions;

private:
	UPROPERTY(Transient, BlueprintReadWrite, meta=(AllowPrivateAccess="true"), Category="Settings")
	float MaxSavedPositionAge;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category="Settings|Prediction")
	float PredictionFudgeFactor = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category="Settings|Prediction")
	float MaxPredictionPing = 300.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category="Settings|Prediction")
	float BotSavedPositionInterval = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category="ComponentReplication")
	bool bNetLoadOnClient = false;

	/**
	 * Cache
	 */
	LagCompensationTypes::FValidActors ValidActors;
	
};
