// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/RightButtonUserWidget.h"


void URightButtonUserWidget::ProcessOnTouchStarted(const FVector2D& _NormTouchStartPos)
{
	Super::ProcessOnTouchStarted(_NormTouchStartPos);

	NormTouchStartPos = _NormTouchStartPos;
}

void URightButtonUserWidget::ProcessOnTouching(const FVector2D& _NormTouchingPos, const FVector2D& _NormTouchingVel)
{
	Super::ProcessOnTouching(_NormTouchingPos, _NormTouchingVel);

	FVector2D NormSlideVector = _NormTouchingPos - NormTouchStartPos;
	NormSlideVector /= MaxSlideRadius;
	if (NormSlideVector.Size() > 1.f)
		NormSlideVector /= NormSlideVector.Size();
	if (OnButtonSlided.IsBound())
		OnButtonSlided.Broadcast(NormSlideVector);
}
