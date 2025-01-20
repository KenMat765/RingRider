// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/BanditAimUserWidget.h"

#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/PanelWidget.h"

#include "Rider/Rider.h"
#include "Rider/Bandit/BanditBand.h"


void UBanditAimUserWidget::ShowAimMark()
{
	AimMark->SetVisibility(ESlateVisibility::Visible);
}

void UBanditAimUserWidget::MoveAimMark(const FVector& WorldPos)
{
	FVector2D ScreenPos;
	bool bProjected = GetScreenFromWorld(WorldPos, ScreenPos);
	if (!bProjected)
		return;

	if (UPanelWidget* ParentScaleBox = AimMark->GetParent())
	{
		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(ParentScaleBox->Slot))
		{
			CanvasSlot->SetPosition(ScreenPos);
		}
	}
}

void UBanditAimUserWidget::HideAimMark()
{
	AimMark->SetVisibility(ESlateVisibility::Collapsed);
}

