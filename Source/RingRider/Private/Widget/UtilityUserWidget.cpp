// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/UtilityUserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetLayoutLibrary.h"


void UUtilityUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not get PlayerController"));
		return;
	}
}


inline float UUtilityUserWidget::GetViewportScale() const
{
	return UWidgetLayoutLibrary::GetViewportScale(GetWorld());
}

inline FVector2D UUtilityUserWidget::GetViewportSize() const
{
	FVector2D ViewportSize;
	GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);
	return ViewportSize;
}

inline FVector2D UUtilityUserWidget::GetNormalizedScreenPosition(const FVector2D& _ScreenPos) const
{
	FVector2D ViewportSize = GetViewportSize();
	if (ViewportSize.X == 0 || ViewportSize.Y == 0)
		return FVector2D::ZeroVector;

	FVector2D NormScreenPos;
	NormScreenPos.X = _ScreenPos.X / ViewportSize.X;
	NormScreenPos.Y = _ScreenPos.Y / ViewportSize.Y;
	return NormScreenPos;
}

inline FVector2D UUtilityUserWidget::GetRealScreenPosition(const FVector2D& _NormScreenPos) const
{
	FVector2D ViewportSize = GetViewportSize();
	if (ViewportSize.X == 0 || ViewportSize.Y == 0)
		return FVector2D::ZeroVector;

	FVector2D ScreenPos;
	ScreenPos.X = _NormScreenPos.X * ViewportSize.X;
	ScreenPos.Y = _NormScreenPos.Y * ViewportSize.Y;
	return ScreenPos;
}

inline bool UUtilityUserWidget::GetScreenFromWorld(const FVector& _WorldPos, FVector2D& _ScreenPos) const
{
	if (!PlayerController)
		return false;

	bool bProjected = UGameplayStatics::ProjectWorldToScreen(PlayerController, _WorldPos, _ScreenPos);
	_ScreenPos /= GetViewportScale();
	return bProjected;
}
