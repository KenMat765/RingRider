// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/RightButtonUserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"


void URightButtonUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	RightButton->SetColorAndOpacity(DefaultColor);
	RightButtonText->SetColorAndOpacity(DefaultColor);
}

void URightButtonUserWidget::ProcessOnTouchStarted(const FVector2D& _NormTouchStartPos)
{
	Super::ProcessOnTouchStarted(_NormTouchStartPos);

	NormTouchStartPos = _NormTouchStartPos;
	RightButton->SetColorAndOpacity(PressedColor);
	RightButtonText->SetColorAndOpacity(PressedColor);
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

void URightButtonUserWidget::ProcessOnTouchEnded(const FVector2D& _NormTouchLatestPos, const FVector2D& _NormTouchLatestVel)
{
	Super::ProcessOnTouchEnded(_NormTouchLatestPos, _NormTouchLatestVel);

	RightButton->SetColorAndOpacity(DefaultColor);
	RightButtonText->SetColorAndOpacity(DefaultColor);
}
