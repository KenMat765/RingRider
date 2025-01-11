// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/RightButtonUserWidget.h"


void URightButtonUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void URightButtonUserWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void URightButtonUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bIsTouching)
	{
		FVector2D TouchCurrentPos;
		bool bFoundTouch = GetTouchPosition(TouchId, TouchCurrentPos);
		if (bFoundTouch)
		{
			FVector2D SlideVector = TouchCurrentPos - TouchStartPos;
			FVector2D NormSlideVector = GetNormalizedScreenPosition(SlideVector);
			if (OnButtonSlided.IsBound())
				OnButtonSlided.Broadcast(NormSlideVector);
		}
		else
		{
			bIsTouching = false;
			if (OnButtonReleased.IsBound())
				OnButtonReleased.Broadcast();
		}
	}
}

FReply URightButtonUserWidget::NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	Super::NativeOnTouchStarted(InGeometry, InGestureEvent);

	bIsTouching = true;
	TouchId = InGestureEvent.GetPointerIndex();

	// GetScreenSpacePosition()�̓f�B�X�v���C�̍�������_�Ƃ������W��Ԃ�
	// �E�B���h�E�̍���̈ʒu(GetWindow()->GetPositionInScreen())�������A�E�B���h�E���΂̍��W�ɕϊ�����
	TouchStartPos = InGestureEvent.GetScreenSpacePosition() - InGestureEvent.GetWindow()->GetPositionInScreen();

	if (OnButtonPressed.IsBound())
		OnButtonPressed.Broadcast();

	return FReply::Handled();
}

FReply URightButtonUserWidget::NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	Super::NativeOnTouchEnded(InGeometry, InGestureEvent);

	if (bIsTouching)
	{
		bIsTouching = false;
		if (OnButtonReleased.IsBound())
			OnButtonReleased.Broadcast();
	}

	return FReply::Handled();
}
