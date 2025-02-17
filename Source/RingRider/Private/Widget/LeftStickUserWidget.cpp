// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/LeftStickUserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanelSlot.h"
#include "Utility/WidgetUtility.h"


void ULeftStickUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	UWidget* StickHandle = WidgetTree->FindWidget(TEXT("SB_StickHandle"));
	ensureMsgf(StickHandle, TEXT("Could not find SB_StickHandle"));
	StickHandleSlot = Cast<UCanvasPanelSlot>(StickHandle->Slot);
	ensureMsgf(StickHandleSlot, TEXT("Could not cast to canvas slot"));
	NormDefaultHandlePos = FWidgetUtility::GetNormalizedScreenPosition(GetWorld(), StickHandleSlot->GetPosition());
}


void ULeftStickUserWidget::ProcessOnTouchStarted(const FVector2D& _NormTouchStartPos)
{
	Super::ProcessOnTouchStarted(_NormTouchStartPos);

	NormTouchStartPos = _NormTouchStartPos;
}

void ULeftStickUserWidget::ProcessOnTouching(const FVector2D& _NormTouchingPos, const FVector2D& _NormTouchingVel)
{
	Super::ProcessOnTouching(_NormTouchingPos, _NormTouchingVel);

	FVector2D NormSlideVector = _NormTouchingPos - NormTouchStartPos;
	FVector2D NewNormHandlePos = NormDefaultHandlePos;
	float XAxisMoveAmount = FMath::Clamp(NormSlideVector.X, -XAxisHalfRange, XAxisHalfRange);
	NewNormHandlePos.X += XAxisMoveAmount;
	MoveHandlePosition(NewNormHandlePos);
	XAxisAmount = XAxisMoveAmount / XAxisHalfRange;
	if (OnStickSlided.IsBound())
		OnStickSlided.Broadcast(XAxisAmount);
}

void ULeftStickUserWidget::ProcessOnTouchEnded(const FVector2D& _NormTouchLatestPos, const FVector2D& _NormTouchLatestVel)
{
	Super::ProcessOnTouchEnded(_NormTouchLatestPos, _NormTouchLatestVel);

	MoveHandlePosition(NormDefaultHandlePos);
	XAxisAmount = 0.f;
}


void ULeftStickUserWidget::MoveHandlePosition(FVector2D _NewNormHandlePos)
{
	if (StickHandleSlot)
	{
		FVector2D RealScreenPos = FWidgetUtility::GetRealScreenPosition(GetWorld(), _NewNormHandlePos);
		StickHandleSlot->SetPosition(RealScreenPos);
	}
}
