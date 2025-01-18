// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/RiderPlayerController.h"
#include "Rider/Rider.h"
#include "Rider/Bandit/BanditBand.h"
#include "Widget/LeftStickUserWidget.h"
#include "Widget/RightButtonUserWidget.h"
#include "Utility/TransformUtility.h"
#include "Utility/WidgetUtility.h"


void ARiderPlayerController::BeginPlay()
{
	Super::BeginPlay();

	Rider = Cast<ARider>(GetPawn());
	ensureMsgf(Rider, TEXT("Could not cast Pawn to Rider"));

	BanditBand = Rider->FindComponentByClass<UBanditBand>();
	ensureMsgf(BanditBand, TEXT("Could not get BanditBand from Rider"));

	InputComponent->BindTouch(IE_Pressed, this, &ARiderPlayerController::OnTouchEnter);
	InputComponent->BindTouch(IE_Released, this, &ARiderPlayerController::OnTouchExit);

	UClass* LeftStickWidgetClass = LoadClass<ULeftStickUserWidget>(nullptr, TEXT("/Game/UI/WB_LeftStick.WB_LeftStick_C"));
	ensureMsgf(LeftStickWidgetClass, TEXT("Could not load WB_LeftStick"));
	LeftStickWidget = CreateWidget<ULeftStickUserWidget>(this, LeftStickWidgetClass);
	ensureMsgf(LeftStickWidget, TEXT("Could not create LeftStickWidget"));
	LeftStickWidget->AddToViewport();
	LeftStickWidget->OnStickSlided.AddDynamic(this, &ARiderPlayerController::OnLeftStickSlided);
	LeftStickWidget->OnTouchExit.AddDynamic(this, &ARiderPlayerController::OnLeftStickExit);

	UClass* RightButtonWidgetClass = LoadClass<URightButtonUserWidget>(nullptr, TEXT("/Game/UI/WB_RightButton.WB_RightButton_C"));
	ensureMsgf(RightButtonWidgetClass, TEXT("Could not load WB_RightButton"));
	RightButtonWidget = CreateWidget<URightButtonUserWidget>(this, RightButtonWidgetClass);
	ensureMsgf(RightButtonWidget, TEXT("Could not create RightButtonWidget"));
	RightButtonWidget->AddToViewport();
	RightButtonWidget->OnTouchEnter.AddDynamic(this, &ARiderPlayerController::OnRightButtonEnter);
	RightButtonWidget->OnButtonSlided.AddDynamic(this, &ARiderPlayerController::OnRightButtonSlided);
	RightButtonWidget->OnTouchExit.AddDynamic(this, &ARiderPlayerController::OnRightButtonExit);
}

void ARiderPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Touches.Num() > 0)
	{
		TArray<uint32> RemoveIds;
		for (TPair<uint32, FPosAndVel>& Touch : Touches)
		{
			FVector2D TouchCurrPos;
			bool bFoundTouch;
			GetInputTouchState(static_cast<ETouchIndex::Type>(Touch.Key), TouchCurrPos.X, TouchCurrPos.Y, bFoundTouch);
			if (bFoundTouch)
			{
				FVector2D NormTouchCurrPos = FWidgetUtility::GetNormalizedScreenPosition(GetWorld(), TouchCurrPos);
				FVector2D NormTouchCurrVel = (NormTouchCurrPos - Touch.Value.Key) / DeltaTime;
				Touch.Value.Key = NormTouchCurrPos;
				Touch.Value.Value = NormTouchCurrVel;
			}
			else
			{
				RemoveIds.Add(Touch.Key);
			}
		}
		if (RemoveIds.Num() > 0)
		{
			for (uint32 RemoveId : RemoveIds)
			{
				Touches.Remove(RemoveId);
			}
		}
	}
}


void ARiderPlayerController::OnTouchEnter(ETouchIndex::Type FingerIndex, FVector Location)
{
	uint32 TouchId = static_cast<uint32>(FingerIndex);
	FVector2D TouchStartPos = FVector2D(Location.X, Location.Y);
	FVector2D NormTouchStartPos = FWidgetUtility::GetNormalizedScreenPosition(GetWorld(), TouchStartPos);
	FPosAndVel PosAndVel(NormTouchStartPos, FVector2D::ZeroVector);
	if (Touches.Contains(TouchId))
	{
		Touches[TouchId] = PosAndVel;
	}
	else
	{
		Touches.Add(TouchId, PosAndVel);
	}
}

void ARiderPlayerController::OnTouchExit(ETouchIndex::Type FingerIndex, FVector Location)
{
	uint32 TouchId = static_cast<uint32>(FingerIndex);
	if (!Touches.Contains(TouchId))
		return;

	FVector2D NormTouchLatestVel = Touches[TouchId].Value;
	float NormTouchLatestSpd = NormTouchLatestVel.Size();
	if (NormTouchLatestSpd > SwipeSpeedThresh)
	{
		float SwipeDeg = FMath::RadiansToDegrees(FVectorUtility::RadianBetweenVectors(FVector2D(0, -1), NormTouchLatestVel));
		if (-45.f <= SwipeDeg && SwipeDeg <= 45.f)
		{
			UE_LOG(LogTemp, Log, TEXT("UP"));
		}
		else if (45.f < SwipeDeg && SwipeDeg < 135.f)
		{
			UE_LOG(LogTemp, Log, TEXT("RIGHT"));
		}
		else if (-135.f < SwipeDeg && SwipeDeg < -45.f)
		{
			UE_LOG(LogTemp, Log, TEXT("LEFT"));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("DOWN"));
		}
	}
	Touches.Remove(TouchId);
}

void ARiderPlayerController::OnLeftStickSlided(float _XAxisValue)
{
	Rider->TiltBike(_XAxisValue);
}

void ARiderPlayerController::OnLeftStickExit(const FVector2D& _NormTouchLatestPos, const FVector2D& _NormTouchLatestVel)
{
	UE_LOG(LogTemp, Log, TEXT("Leftstick Exit"));
	Rider->TiltBike(0.f);
}

void ARiderPlayerController::OnRightButtonEnter(const FVector2D& _NormTouchStartPos)
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
	// 縦軸方向は画面外に出ないようにYAttenuationで移動量を抑制
	FVector AimTarget_Local = FVector(CircleDistance, AdjustedSlideVector.X, -AdjustedSlideVector.Y * YAttenuation)
		.GetSafeNormal() * BanditBand->MaxLength;
	// 縦軸マイナス方向へは行かないようにする
	AimTarget_Local.Z = FMath::Max(AimTarget_Local.Z, 0.f);
	FVector AimTarget_World = Rider->GetActorTransform().TransformPosition(AimTarget_Local);
	BanditBand->SetAimTarget(AimTarget_World);
}

void ARiderPlayerController::OnRightButtonExit(const FVector2D& _NormTouchLatestPos, const FVector2D& _NormTouchLatestVel)
{
	UE_LOG(LogTemp, Log, TEXT("RightButton Exit"));
	BanditBand->EndAim();
	if (BanditBand->bCanShoot)
	{
		BanditBand->ShootBand();
	}
}
