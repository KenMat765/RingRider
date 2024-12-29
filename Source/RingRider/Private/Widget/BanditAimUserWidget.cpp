// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/BanditAimUserWidget.h"

#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/PanelWidget.h"

#include "Rider/Rider.h"
#include "Rider/Bandit/BanditBand.h"


UBanditAimUserWidget::UBanditAimUserWidget(const FObjectInitializer& ObjectInitializer):
	UProjectableUserWidget(ObjectInitializer)
{
}


void UBanditAimUserWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}


void UBanditAimUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ARider* Rider = Cast<ARider>(PlayerController->GetPawn()))
	{
		if (UBanditBand* BanditBand = Rider->FindComponentByClass<UBanditBand>())
		{
			auto OnBanditStartAimDelegate = [this](const FVector& TargetPos) { MoveAimMark(TargetPos); ShowAimMark(); };
			OnStartAimDelegateHandle = BanditBand->AddOnStartAimAction(OnBanditStartAimDelegate);

			auto OnBanditAimingDelegate = [this](const FVector& TargetPos) { MoveAimMark(TargetPos); };
			OnAimingDelegateHandle = BanditBand->AddOnAimingAction(OnBanditAimingDelegate);

			auto OnBanditEndAimDelegate = [this]() { HideAimMark(); };
			OnEndAimDelegateHandle = BanditBand->AddOnEndAimAction(OnBanditEndAimDelegate);
		}
		else
			UE_LOG(LogTemp, Warning, TEXT("Could not get BanditBand from Rider"));
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("Could not get Rider"));

	HideAimMark();
}


void UBanditAimUserWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (ARider* Rider = Cast<ARider>(PlayerController->GetPawn()))
	{
		if (UBanditBand* BanditBand = Rider->FindComponentByClass<UBanditBand>())
		{
			BanditBand->RemoveOnStartAimAction(OnStartAimDelegateHandle);
			BanditBand->RemoveOnAimingAction(OnAimingDelegateHandle);
			BanditBand->RemoveOnEndAimAction(OnEndAimDelegateHandle);
		}
	}
}


void UBanditAimUserWidget::ShowAimMark()
{
	AimMark->SetVisibility(ESlateVisibility::Visible);
}

void UBanditAimUserWidget::MoveAimMark(const FVector& WorldPos)
{
	FVector2D ScreenPos;
	bool bProjected = GetScreenPos(WorldPos, ScreenPos);
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

