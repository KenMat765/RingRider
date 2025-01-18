// Fill out your copyright notice in the Description page of Project Settings.


#include "Utility/WidgetUtility.h"
#include "Utility/TransformUtility.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetLayoutLibrary.h"


namespace FWidgetUtility
{
	float GetViewportScale(UWorld* _World)
	{
		return UWidgetLayoutLibrary::GetViewportScale(_World);
	}

	FVector2D GetViewportSize(UWorld* _World)
	{
		FVector2D ViewportSize;
		_World->GetGameViewport()->GetViewportSize(ViewportSize);
		return ViewportSize;
	}

	FVector2D GetNormalizedScreenPosition(UWorld* _World, const FVector2D& _ScreenPos)
	{
		FVector2D ViewportSize = GetViewportSize(_World);
		if (ViewportSize.X == 0 || ViewportSize.Y == 0)
			return FVector2D::ZeroVector;

		FVector2D NormScreenPos;
		NormScreenPos.X = _ScreenPos.X / ViewportSize.X;
		NormScreenPos.Y = _ScreenPos.Y / ViewportSize.Y;
		return NormScreenPos;
	}

	FVector2D GetRealScreenPosition(UWorld* _World, const FVector2D& _NormScreenPos)
	{
		FVector2D ViewportSize = GetViewportSize(_World);
		if (ViewportSize.X == 0 || ViewportSize.Y == 0)
			return FVector2D::ZeroVector;

		FVector2D ScreenPos;
		ScreenPos.X = _NormScreenPos.X * ViewportSize.X;
		ScreenPos.Y = _NormScreenPos.Y * ViewportSize.Y;
		return ScreenPos;
	}

	bool GetScreenFromWorld(UWorld* _World, const FVector& _WorldPos, FVector2D& _ScreenPos)
	{
		APlayerController* PlayerController = _World->GetFirstPlayerController();
		if (!PlayerController)
			return false;

		bool bProjected = UGameplayStatics::ProjectWorldToScreen(PlayerController, _WorldPos, _ScreenPos);
		_ScreenPos /= GetViewportScale(_World);
		return bProjected;
	}
}
