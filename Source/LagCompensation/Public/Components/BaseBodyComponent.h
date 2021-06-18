// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/Types.h"

#include "BaseBodyComponent.generated.h"


UCLASS(Abstract, ClassGroup=(LagCompensation), BlueprintType)
class LAGCOMPENSATION_API UBaseBodyComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	LagCompensationTypes::FBodyInstances BodyInstances;

public:
	// Sets default values for this component's properties
	UBaseBodyComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category="Initialize")
	void Initialize(const TArray<UPrimitiveComponent*> PrimitiveComponents);

	UFUNCTION(BlueprintCallable, Category="Utilities")
	bool IsValidBodyInstances() const;

	FORCEINLINE const LagCompensationTypes::FBodyInstances& GetBodyInstances() const;
};
