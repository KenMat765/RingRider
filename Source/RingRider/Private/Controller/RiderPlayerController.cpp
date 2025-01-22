// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/RiderPlayerController.h"
#include "Rider/Rider.h"
#include "Rider/Bandit/BanditBand.h"
#include "Widget/LeftStickUserWidget.h"
#include "Widget/RightButtonUserWidget.h"
#include "Widget/BanditAimUserWidget.h"
#include "Utility/TransformUtility.h"
#include "Utility/WidgetUtility.h"


void ARiderPlayerController::OnPossess(APawn* _Pawn)
{
	Super::OnPossess(_Pawn);

	Rider = Cast<ARider>(_Pawn);
	ensureMsgf(Rider, TEXT("Could not cast Pawn to Rider"));

	BanditBand = _Pawn->FindComponentByClass<UBanditBand>();
	ensureMsgf(BanditBand, TEXT("Could not get BanditBand from Player Pawn"));

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

	UClass* BanditAimWidgetClass = LoadClass<UBanditAimUserWidget>(nullptr, TEXT("/Game/UI/WB_BanditAim.WB_BanditAim_C"));
	ensureMsgf(BanditAimWidgetClass, TEXT("Could not load WB_BanditAim"));
	BanditAimWidget = CreateWidget<UBanditAimUserWidget>(this, BanditAimWidgetClass);
	ensureMsgf(BanditAimWidget, TEXT("Could not create BanditAimWidget"));
	BanditAimWidget->AddToViewport();
	BanditAimWidget->HideAimMark();
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
		if		(-30.f	<= SwipeDeg && SwipeDeg <= 30.f)  OnSwipe(ESwipeDirection::UP);
		else if (30.f	<  SwipeDeg && SwipeDeg <  150.f) OnSwipe(ESwipeDirection::RIGHT);
		else if (-150.f	<  SwipeDeg && SwipeDeg <  -30.f) OnSwipe(ESwipeDirection::LEFT);
		else											  OnSwipe(ESwipeDirection::DOWN);
	}
	Touches.Remove(TouchId);
}

void ARiderPlayerController::OnLeftStickSlided(float _XAxisValue)
{
	Rider->TiltBike(_XAxisValue);
}

void ARiderPlayerController::OnLeftStickExit(const FVector2D& _NormTouchLatestPos, const FVector2D& _NormTouchLatestVel)
{
	Rider->TiltBike(0.f);
}

void ARiderPlayerController::OnRightButtonEnter(const FVector2D& _NormTouchStartPos)
{
	if (BanditBand->IsSticked())
		return;

	FVector AimTarget = Rider->GetActorLocation() + Rider->GetActorForwardVector() * BanditBand->MaxLength;
	BanditAimWidget->MoveAimMark(AimTarget);
	BanditAimWidget->ShowAimMark();
}

void ARiderPlayerController::OnRightButtonSlided(const FVector2D& _NormSlideVector)
{
	if (BanditBand->IsSticked())
		return;

	float MaxBanditShootRad = FMath::DegreesToRadians(MaxBanditShootDeg);
	// ボタン中央付近の感度を下げ、遠くに行くほど感度を上げる
	FVector2D AdjustedSlideVector = _NormSlideVector.GetSafeNormal() * FMath::Pow(_NormSlideVector.Size(), AimSensitivity);

	FVector CameraForward = PlayerCameraManager->GetCameraRotation().Vector();
	// Z軸方向は潰して水平にする
	CameraForward = FVector(CameraForward.X, CameraForward.Y, 0).GetSafeNormal();
	FVector CameraUp = FVector::UpVector;
	FVector CameraRight = FVector::CrossProduct(CameraUp, CameraForward);

	float CircleDistance = 1.f / FMath::Tan(MaxBanditShootRad);
	// 縦軸方向は画面外に出ないようにYAttenuationで移動量を抑制
	FVector AimDirection =
		CircleDistance * CameraForward +
		AdjustedSlideVector.X * CameraRight +
		-AdjustedSlideVector.Y * YAttenuation * CameraUp;
	// 縦軸マイナス方向へは行かないようにする
	AimDirection.Z = FMath::Max(AimDirection.Z, 0.f);
	AimDirection = AimDirection.GetSafeNormal();
	BanditAimTarget = Rider->GetActorLocation() + AimDirection * BanditBand->MaxLength;

	FVector SnapPos;
	bool bSnapped = CheckBanditSnap(BanditAimTarget, SnapPos);
	if (bSnapped)
		BanditAimTarget = SnapPos;

	BanditAimWidget->MoveAimMark(BanditAimTarget);
}

void ARiderPlayerController::OnRightButtonExit(const FVector2D& _NormTouchLatestPos, const FVector2D& _NormTouchLatestVel)
{
	if (BanditBand->IsSticked())
	{
		BanditBand->CutBand();
	}
	else
	{
		BanditAimWidget->HideAimMark();
		if (BanditBand->bCanShoot)
			BanditBand->ShootBand(BanditAimTarget);
	}
}

void ARiderPlayerController::OnSwipe(ESwipeDirection _SwipeDirection)
{
	switch (_SwipeDirection)
	{
	case ESwipeDirection::UP: {
		if(Rider->IsGrounded())
			Rider->Jump();				 
	} break;

	case ESwipeDirection::DOWN: {
		BanditBand->PullBand();
	} break;

	case ESwipeDirection::LEFT: {
		ARider::EDriftDirection DriftDirection;
		bool bRiderDrifting = Rider->IsDrifting(DriftDirection);
		if (bRiderDrifting)
		{
			if(DriftDirection == ARider::EDriftDirection::LEFT)
				Rider->StopDrift();
		}
		else if (LeftStickWidget->GetXAxisAmount() < 0.f)
		{
			Rider->StartDrift(ARider::EDriftDirection::LEFT);
		}
	} break;

	case ESwipeDirection::RIGHT: {
		ARider::EDriftDirection DriftDirection;
		bool bRiderDrifting = Rider->IsDrifting(DriftDirection);
		if (bRiderDrifting)
		{
			if(DriftDirection == ARider::EDriftDirection::RIGHT)
				Rider->StopDrift();
		}
		else if (LeftStickWidget->GetXAxisAmount() > 0.f)
		{
			Rider->StartDrift(ARider::EDriftDirection::RIGHT);
		}
	} break;
	}
}


bool ARiderPlayerController::CheckBanditSnap(const FVector& _AimTarget, FVector& _SnapPos)
{
	FCollisionObjectQueryParams ObjQueryParam;
	ObjQueryParam.AddObjectTypesToQuery(BanditSnapChannel);
	FCollisionQueryParams QueryParam;
	QueryParam.AddIgnoredActor(Rider);
	FHitResult Hit;
	bool bHit = GetWorld()->LineTraceSingleByObjectType(Hit, BanditBand->GetComponentLocation(), _AimTarget, ObjQueryParam, QueryParam);
	if (bHit)
		_SnapPos = Hit.Component->GetComponentLocation();
	return bHit;
}
