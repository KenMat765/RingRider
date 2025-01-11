// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/LeftStickUserWidget.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/CanvasPanelSlot.h"


void ULeftStickUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(SB_StickHandle->Slot))
	{
		DefaultHandlePos = CanvasSlot->GetPosition();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Could not cast to canvas slot"));
	}
}

void ULeftStickUserWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void ULeftStickUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bIsTouching)
	{
		FVector2D TouchCurrentPos;
		bool bFoundTouch = GetTouchPosition(TouchId, TouchCurrentPos);
		if (bFoundTouch)
		{
			FVector2D SlideVector = TouchCurrentPos - TouchStartPos;
			FVector2D NewHandlePos = DefaultHandlePos;
			float XAxisMoveAmount = FMath::Clamp(SlideVector.X, -XAxisHalfRange, XAxisHalfRange);
			NewHandlePos.X += XAxisMoveAmount;
			MoveHandlePosision(NewHandlePos);
			if (OnStickSlided.IsBound())
				OnStickSlided.Broadcast(XAxisMoveAmount / XAxisHalfRange);
		}
		else
		{
			bIsTouching = false;
			MoveHandlePosision(DefaultHandlePos);
			if (OnStickReleased.IsBound())
				OnStickReleased.Broadcast();
		}
	}
}

FReply ULeftStickUserWidget::NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	Super::NativeOnTouchStarted(InGeometry, InGestureEvent);

	bIsTouching = true;
	TouchId = InGestureEvent.GetPointerIndex();

	// GetScreenSpacePosition()はディスプレイの左上を原点とした座標を返す
	// ウィンドウの左上の位置(GetWindow()->GetPositionInScreen())を引き、ウィンドウ相対の座標に変換する
	TouchStartPos = InGestureEvent.GetScreenSpacePosition() - InGestureEvent.GetWindow()->GetPositionInScreen();
	if (OnStickPressed.IsBound())
		OnStickPressed.Broadcast();

	return FReply::Handled();
}


bool ULeftStickUserWidget::MoveHandlePosision(FVector2D _NewHandlePos)
{
	if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(SB_StickHandle->Slot))
	{
		CanvasSlot->SetPosition(_NewHandlePos);
		return true;
	}
	return false;
}
