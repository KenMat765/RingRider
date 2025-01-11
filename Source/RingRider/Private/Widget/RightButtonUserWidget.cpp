// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/RightButtonUserWidget.h"


void URightButtonUserWidget::NativeConstruct()
{
}

void URightButtonUserWidget::NativeDestruct()
{
}

void URightButtonUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
}

FReply URightButtonUserWidget::NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	UE_LOG(LogTemp, Log, TEXT("Touch Start"));
	return FReply::Handled();
}

FReply URightButtonUserWidget::NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	UE_LOG(LogTemp, Log, TEXT("Touch Move"));
	return FReply::Handled();
}

FReply URightButtonUserWidget::NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	UE_LOG(LogTemp, Log, TEXT("Touch End"));
	return FReply::Handled();
}
