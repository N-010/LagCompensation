// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseBodyComponent.h"


// Sets default values for this component's properties
UBaseBodyComponent::UBaseBodyComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UBaseBodyComponent::Initialize(const TArray<UPrimitiveComponent*> PrimitiveComponents)
{
	for (const UPrimitiveComponent* BodyInstance : PrimitiveComponents)
	{
		BodyInstances.AddUnique(BodyInstance->GetBodyInstance());
	}
}

bool UBaseBodyComponent::IsValidBodyInstances() const
{
	for (const auto* BodyInstance : BodyInstances)
	{
		if (!BodyInstance || !BodyInstance->IsValidBodyInstance())
		{
			return false;
		}
	}

	return BodyInstances.Num() > 0;
}

FORCEINLINE const LagCompensationTypes::FBodyInstances& UBaseBodyComponent::GetBodyInstances() const
{
	return BodyInstances;
}
