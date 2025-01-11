// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/TouchUserWidget.h"


void UTouchUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UTouchUserWidget::NativeDestruct()
{
	Super::NativeDestruct();
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
