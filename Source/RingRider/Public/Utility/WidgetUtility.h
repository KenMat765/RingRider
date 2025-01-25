// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

namespace FWidgetUtility
{
	float GetViewportScale(UWorld* _World);
	FVector2D GetViewportSize(UWorld* _World);
	FVector2D GetNormalizedScreenPosition(UWorld* _World, const FVector2D& _ScreenPos);
	FVector2D GetRealScreenPosition(UWorld* _World, const FVector2D& _NormScreenPos);
	bool GetScreenFromWorld(UWorld* _World, const FVector& _WorldPos, FVector2D& _ScreenPos);
}
