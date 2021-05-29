// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "BaseBodyComponent.h"
#include "ProjectileComponent.generated.h"


UCLASS(ClassGroup=(LagCompensation), meta=(BlueprintSpawnableComponent))
class LAGCOMPENSATION_API UProjectileComponent : public UBaseBodyComponent
{
	GENERATED_BODY()

public:
	UProjectileComponent(const FObjectInitializer& ObjectInitializer);
};
