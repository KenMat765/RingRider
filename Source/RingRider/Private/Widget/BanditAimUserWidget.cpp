// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/BanditAimUserWidget.h"

#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/PanelWidget.h"

#include "Rider/Rider.h"
#include "Rider/Bandit/BanditBand.h"

#include "Utility/WidgetUtility.h"


void UBanditAimUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CurrentMark = Aim_NoLockOn;
}


void UBanditAimUserWidget::ShowAimMark(bool _bLockOn)
{
	ChangeMark(_bLockOn);
	CurrentMark->SetVisibility(ESlateVisibility::Visible);
	bIsShowing = true;
}

void UBanditAimUserWidget::MoveAimMark(const FVector& WorldPos)
{
	FVector2D ScreenPos;
	bool bProjected = FWidgetUtility::GetScreenFromWorld(GetWorld(), WorldPos, ScreenPos);
	if (!bProjected)
		return;

	if (UPanelWidget* ParentScaleBox = CurrentMark->GetParent())
		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(ParentScaleBox->Slot))
			CanvasSlot->SetPosition(ScreenPos);
}

void UBanditAimUserWidget::HideAimMark()
{
	Aim_NoLockOn->SetVisibility(ESlateVisibility::Collapsed);
	Aim_LockOn->SetVisibility(ESlateVisibility::Collapsed);
	bIsShowing = false;
}

void UBanditAimUserWidget::ChangeMark(bool _bLockOn)
{
	if (_bLockOn)
	{
		if (CurrentMark == Aim_LockOn)
			return;
		CurrentMark = Aim_LockOn;
		if (bIsShowing)
		{
			Aim_NoLockOn->SetVisibility(ESlateVisibility::Collapsed);
			Aim_LockOn->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else
	{
		if (CurrentMark == Aim_NoLockOn)
			return;
		CurrentMark = Aim_NoLockOn;
		if (bIsShowing)
		{
			Aim_LockOn->SetVisibility(ESlateVisibility::Collapsed);
			Aim_NoLockOn->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

