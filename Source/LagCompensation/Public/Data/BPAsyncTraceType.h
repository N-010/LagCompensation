// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class BPAsyncTraceType : uint8
{
	/** Return whether the trace succeeds or fails (using bBlockingHit flag on FHitResult), but gives no info about what you hit or where. Good for fast occlusion tests. */
	Test,
	/** Returns a single blocking hit */
	Single,
	/** Returns a single blocking hit, plus any overlapping hits up to that point */
	Multi
};
