// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileComponent.h"


// Sets default values for this component's properties
UProjectileComponent::UProjectileComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
}

