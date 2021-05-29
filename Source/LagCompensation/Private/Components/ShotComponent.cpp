// Fill out your copyright notice in the Description page of Project Settings.


#include "ShotComponent.h"


UShotComponent::UShotComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(false);
	bWantsInitializeComponent = true;
}

void UShotComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (GetOwnerRole() < ROLE_Authority && !NeedsLoadForClient())
	{
		DestroyComponent();
		return;
	}

	ValidActors.AddDefaulted(LagCompensationTypes::NumInlinedValidActors);
}

bool UShotComponent::NeedsLoadForClient() const
{
	check(GetOuter());
	// For Component Blueprints, avoid calling into the class to avoid recursion
	const bool bNeedsLoadOuter = HasAnyFlags(RF_ClassDefaultObject) || bNetLoadOnClient;
	return (!IsEditorOnly() && bNeedsLoadOuter && UObject::NeedsLoadForClient());
}

