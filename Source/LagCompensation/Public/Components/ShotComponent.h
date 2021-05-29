// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/Types.h"

#include "ShotComponent.generated.h"


UCLASS(ClassGroup=(LagCompensation), meta=(BlueprintSpawnableComponent))
class LAGCOMPENSATION_API UShotComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UShotComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~ Begin UActorComponent Interfaces
	virtual void InitializeComponent() override;
	//~ End UActorComponent Interfaces

	//~ Begin UObject Interfaces
	virtual bool NeedsLoadForClient() const override;
	//~ End UObject Interfaces

protected:
	LagCompensationTypes::FValidActors ValidActors;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category="ComponentReplication")
	bool bNetLoadOnClient = false;
};
