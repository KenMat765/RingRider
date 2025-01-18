// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/TouchUserWidget.h"
#include "Utility/TransformUtility.h"


FReply UTouchUserWidget::NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	Super::NativeOnTouchStarted(InGeometry, InGestureEvent);

	TouchId = InGestureEvent.GetPointerIndex();
	// GetScreenSpacePosition()はディスプレイの左上を原点とした座標を返すので、ウィンドウの左上の位置を引き、ウィンドウ相対の座標に変換する
	FVector2D TouchStartPos = InGestureEvent.GetScreenSpacePosition() - InGestureEvent.GetWindow()->GetPositionInScreen();
	FVector2D NormTouchStartPos = GetNormalizedScreenPosition(TouchStartPos);
	ProcessOnTouchStarted(NormTouchStartPos);
	NormTouchPrevPos = NormTouchStartPos;

	return FReply::Handled();
}

void UTouchUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bIsTouching)
	{
		FVector2D TouchCurrPos;
		bool bFoundTouch = GetTouchPosition(TouchId, TouchCurrPos);
		if (bFoundTouch)
		{
			FVector2D NormTouchCurrPos = GetNormalizedScreenPosition(TouchCurrPos);
			FVector2D NormTouchCurrVel = (NormTouchCurrPos - NormTouchPrevPos) / InDeltaTime;
			ProcessOnTouching(NormTouchCurrPos, NormTouchCurrVel);
			NormTouchPrevPos = NormTouchCurrPos;
			NormTouchPrevVel = NormTouchCurrVel;
		}
		else
		{
			ProcessOnTouchEnded(NormTouchPrevPos, NormTouchPrevVel);
		}
	}
}

FReply UTouchUserWidget::NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	Super::NativeOnTouchEnded(InGeometry, InGestureEvent);

	if (bIsTouching)
	{
		ProcessOnTouchEnded(NormTouchPrevPos, NormTouchPrevVel);
	}

	return FReply::Handled();
}


void UTouchUserWidget::ProcessOnTouchStarted(const FVector2D& _NormTouchStartPos)
{
	bIsTouching = true;
	if (OnTouchEnter.IsBound())
		OnTouchEnter.Broadcast(_NormTouchStartPos);
}

void UTouchUserWidget::ProcessOnTouching(const FVector2D& _NormTouchingPos, const FVector2D& _NormTouchingVel)
{
	if (OnTouching.IsBound())
		OnTouching.Broadcast(_NormTouchingPos, _NormTouchingVel);
}

void UTouchUserWidget::ProcessOnTouchEnded(const FVector2D& _NormTouchLatestPos, const FVector2D& _NormTouchLatestVel)
{
	bIsTouching = false;

	if (OnSwipe.IsBound())
	{
		float NormTouchLatestSpd = _NormTouchLatestVel.Size();
		if (NormTouchLatestSpd > SwipeSpeedThresh)
		{
			float SwipeDeg = FMath::RadiansToDegrees(FVectorUtility::RadianBetweenVectors(FVector2D(0, -1), _NormTouchLatestVel));
			if (-45.f <= SwipeDeg && SwipeDeg <= 45.f)
			{
				OnSwipe.Broadcast(ESwipeDirection::UP);
			}
			else if (45.f < SwipeDeg && SwipeDeg < 135.f)
			{
				OnSwipe.Broadcast(ESwipeDirection::RIGHT);
			}
			else if (-135.f < SwipeDeg && SwipeDeg < -45.f)
			{
				OnSwipe.Broadcast(ESwipeDirection::LEFT);
			}
			else
			{
				OnSwipe.Broadcast(ESwipeDirection::DOWN);
			}
		}
	}

	if (OnTouchExit.IsBound())
		OnTouchExit.Broadcast(_NormTouchLatestPos, _NormTouchLatestVel);
}


inline bool UTouchUserWidget::GetTouchPosition(uint32 _TouchId, FVector2D& _TouchPos) const
{
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerController not found"));
		return false;
	}

	bool bIsCurrentlyPressed;
	PlayerController->GetInputTouchState(
		static_cast<ETouchIndex::Type>(_TouchId),
		_TouchPos.X,
		_TouchPos.Y,
		bIsCurrentlyPressed
	);
	return bIsCurrentlyPressed;
}
