// Fill out your copyright notice in the Description page of Project Settings.


#include "LagCompensationFunctionLibrary.h"


UObject* ULagCompensationFunctionLibrary::GetCDO(UClass* ObjectClass)
{
	return IsValid(ObjectClass) ? ObjectClass->GetDefaultObject() : nullptr;
}

FShapeData ULagCompensationFunctionLibrary::GetShapeDataFromPrimitiveComponent(
	const UPrimitiveComponent* PrimitiveComponent)
{
	return IsValid(PrimitiveComponent) ? FShapeData(PrimitiveComponent->GetBodyInstance(), true) : FShapeData();
}
