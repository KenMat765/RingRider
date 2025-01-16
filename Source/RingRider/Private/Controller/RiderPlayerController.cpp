// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/RiderPlayerController.h"
#include "Widget/LeftStickUserWidget.h"
#include "Widget/RightButtonUserWidget.h"
#include "Rider/Rider.h"
#include "Rider/Bandit/BanditBand.h"
#include "Utility//TransformUtility.h"


void ARiderPlayerController::BeginPlay()
{
	Super::BeginPlay();

	Rider = Cast<ARider>(GetPawn());
	ensureMsgf(Rider, TEXT("Could not cast Pawn to Rider"));

	BanditBand = Rider->FindComponentByClass<UBanditBand>();
	ensureMsgf(BanditBand, TEXT("Could not get BanditBand from Rider"));

	UClass* LeftStickWidgetClass = LoadClass<ULeftStickUserWidget>(nullptr, TEXT("/Game/UI/WB_LeftStick.WB_LeftStick_C"));
	ensureMsgf(LeftStickWidgetClass, TEXT("Could not load WB_LeftStick"));
	LeftStickWidget = CreateWidget<ULeftStickUserWidget>(this, LeftStickWidgetClass);
	ensureMsgf(LeftStickWidget, TEXT("Could not create LeftStickWidget"));
	LeftStickWidget->AddToViewport();
	LeftStickWidget->OnStickSlided.AddDynamic(this, &ARiderPlayerController::OnLeftStickSlided);
	LeftStickWidget->OnStickReleased.AddDynamic(this, &ARiderPlayerController::OnLeftStickReleased);

	UClass* RightButtonWidgetClass = LoadClass<URightButtonUserWidget>(nullptr, TEXT("/Game/UI/WB_RightButton.WB_RightButton_C"));
	ensureMsgf(RightButtonWidgetClass, TEXT("Could not load WB_RightButton"));
	RightButtonWidget = CreateWidget<URightButtonUserWidget>(this, RightButtonWidgetClass);
	ensureMsgf(RightButtonWidget, TEXT("Could not create RightButtonWidget"));
	RightButtonWidget->AddToViewport();
	RightButtonWidget->OnButtonPressed.AddDynamic(this, &ARiderPlayerController::OnRightButtonPressed);
	RightButtonWidget->OnButtonSlided.AddDynamic(this, &ARiderPlayerController::OnRightButtonSlided);
	RightButtonWidget->OnButtonReleased.AddDynamic(this, &ARiderPlayerController::OnRightButtonReleased);

}


void ARiderPlayerController::OnLeftStickSlided(float _XAxisValue)
{
	Rider->TiltBike(_XAxisValue);
}

void ARiderPlayerController::OnLeftStickReleased()
{
	Rider->TiltBike(0.f);
}

void ARiderPlayerController::OnRightButtonPressed()
{
	FVector AimTarget = Rider->GetActorLocation() + Rider->GetActorForwardVector() * BanditBand->MaxLength;
	BanditBand->StartAim(AimTarget);
}

void ARiderPlayerController::OnRightButtonSlided(const FVector2D& _NormSlideVector)
{
	float MaxBanditShootRad = FMath::DegreesToRadians(MaxBanditShootDeg);
	// ボタン中央付近の感度を下げ、遠くに行くほど感度を上げる
	FVector2D AdjustedSlideVector = _NormSlideVector.GetSafeNormal() * FMath::Pow(_NormSlideVector.Size(), AimSensitivity);
	float CircleDistance = 1.f / FMath::Tan(MaxBanditShootRad);
	// 縦軸方向は画面外に出ないように定数(2.5f)で割る
	FVector AimTarget_Local = FVector(CircleDistance, AdjustedSlideVector.X, -AdjustedSlideVector.Y * YAttenuation)
		.GetSafeNormal() * BanditBand->MaxLength;
	// 縦軸マイナス方向へは行かないようにする
	AimTarget_Local.Z = FMath::Max(AimTarget_Local.Z, 0.f);
	FVector AimTarget_World = Rider->GetActorTransform().TransformPosition(AimTarget_Local);
	BanditBand->SetAimTarget(AimTarget_World);
}

void ARiderPlayerController::OnRightButtonReleased()
{
	BanditBand->EndAim();
	if (BanditBand->bCanShoot)
	{
		BanditBand->ShootBand();
	}
}
