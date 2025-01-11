// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/LeftStickUserWidget.h"
#include "Components/Image.h"


void ULeftStickUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	static FVector2D TouchCurrentPos;
	if (bIsTouching)
	{
		bool bFoundTouch = GetTouchPosition(TouchId, TouchCurrentPos);
		if (bFoundTouch)
		{
			FVector2D SlideVector = TouchCurrentPos - TouchStartPos;
			FVector2D NormSlideVector = GetNormalizedScreenPosition(SlideVector);
			if (OnStickSlided.IsBound())
				OnStickSlided.Broadcast(NormSlideVector);

			UE_LOG(LogTemp, Log, TEXT("SlideVector: %f, %f"), NormSlideVector.X, NormSlideVector.Y);
		}
		else
		{
			bIsTouching = false;
			if (OnStickReleased.IsBound())
				OnStickReleased.Broadcast(TouchCurrentPos);

			UE_LOG(LogTemp, Log, TEXT("TouchEndPos: %f, %f"), TouchCurrentPos.X, TouchCurrentPos.Y);
		}
	}
}

FReply ULeftStickUserWidget::NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	bIsTouching = true;
	TouchId = InGestureEvent.GetPointerIndex();

	// GetScreenSpacePosition()はディスプレイの左上を原点とした座標を返す
	// ウィンドウの左上の位置(GetWindow()->GetPositionInScreen())を引き、ウィンドウ相対の座標に変換する
	TouchStartPos = InGestureEvent.GetScreenSpacePosition() - InGestureEvent.GetWindow()->GetPositionInScreen();
	FVector2D NormTouchStartPos = GetNormalizedScreenPosition(TouchStartPos);
	if (OnStickPressed.IsBound())
		OnStickPressed.Broadcast(NormTouchStartPos);

	return FReply::Handled();
}
