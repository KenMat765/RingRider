// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/RiderPlayerController.h"
#include "Rider/PlayerRider.h"
#include "Rider/Bandit/BanditBand.h"
#include "Kismet/GameplayStatics.h"
#include "Widget/LeftStickUserWidget.h"
#include "Widget/RightButtonUserWidget.h"
#include "Widget/BanditAimUserWidget.h"
#include "Widget/RiderInfoUserWidget.h"
#include "Utility/TransformUtility.h"
#include "Utility/WidgetUtility.h"


void ARiderPlayerController::OnPossess(APawn* _Pawn)
{
	Super::OnPossess(_Pawn);

	Rider = Cast<APlayerRider>(_Pawn);
	ensureMsgf(Rider, TEXT("Could not cast Pawn to PlayerRider"));

	BanditBand = _Pawn->FindComponentByClass<UBanditBand>();
	ensureMsgf(BanditBand, TEXT("Could not get BanditBand from Player Pawn"));

	StartTransform = Rider->GetActorTransform();

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

	UClass* RiderInfoWidgetClass = LoadClass<URiderInfoUserWidget>(nullptr, TEXT("/Game/UI/WB_RiderInfo.WB_RiderInfo_C"));
	ensureMsgf(RiderInfoWidgetClass, TEXT("Could not load WB_RiderInfo"));
	RiderInfoWidget = CreateWidget<URiderInfoUserWidget>(this, RiderInfoWidgetClass);
	ensureMsgf(RiderInfoWidget, TEXT("Could not create RiderInfoWidget"));
	RiderInfoWidget->AddToViewport();
	RiderInfoWidget->SetMinMaxSpeed(Rider->GetMinSpeed(), Rider->GetMaxSpeed());
	RiderInfoWidget->ShowEnergyMeter(Rider->GetEnergy() / Rider->GetMaxEnergy());
	Rider->OnSpeedChanged.AddDynamic(this, &ARiderPlayerController::OnRiderSpeedChanged);
	Rider->OnEnergyChanged.AddDynamic(this, &ARiderPlayerController::OnRiderEnergyChanged);
}

void ARiderPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float RiderZ = Rider->GetActorLocation().Z;
	if (RiderZ < KillZ)
	{
		OnRiderFellOff();
	}

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
	if (!BanditBand->CanShoot())
		return;

	bIsBanditAiming = true;
	FVector AimTarget = Rider->GetActorLocation() + Rider->GetActorForwardVector() * BanditBand->GetMaxLength();
	BanditAimWidget->MoveAimMark(AimTarget);
	BanditAimWidget->ShowAimMark();
}

void ARiderPlayerController::OnRightButtonSlided(const FVector2D& _NormSlideVector)
{
	if (!bIsBanditAiming)
		return;

	float MaxBanditShootRad = FMath::DegreesToRadians(MaxBanditShootDeg);
	// ボタン中央付近の感度を下げ、遠くに行くほど感度を上げる
	FVector2D AdjustedSlideVector = _NormSlideVector.GetSafeNormal() * FMath::Pow(_NormSlideVector.Size(), AimSensitivity);

	FVector CameraForward = PlayerCameraManager->GetCameraRotation().Vector();
	// Z軸方向は潰して水平にする
	FVector AimX = FVector(CameraForward.X, CameraForward.Y, 0).GetSafeNormal();
	FVector AimZ = FVector::UpVector;
	FVector AimY = FVector::CrossProduct(AimZ, AimX);

	float CircleDistance = 1.f / FMath::Tan(MaxBanditShootRad);
	// 縦軸方向は画面外に出ないようにYAttenuationで移動量を抑制
	FVector AimDirection =
		AimX * CircleDistance +
		AimY * AdjustedSlideVector.X +
		AimZ * AdjustedSlideVector.Y * -1 * YAttenuation;
	// 縦軸マイナス方向へは行かないようにする
	AimDirection.Z = FMath::Max(AimDirection.Z, 0.f);
	AimDirection = AimDirection.GetSafeNormal();
	FVector BanditRootPos = BanditBand->GetComponentLocation();
	BanditAimTarget = BanditRootPos + AimDirection * BanditBand->GetMaxLength();

	bool bSnapped = false;
	FVector SnappablePos;
	bool bHitSnappable = LineTraceBanditSnappable(BanditAimTarget, SnappablePos);
	if (bHitSnappable)
	{
		// 検出位置がBanditBandの前方にあれば次のステップへ
		FVector BanditRootToSnappable = SnappablePos - BanditRootPos;
		float DotProduct = FVector::DotProduct(BanditRootToSnappable, AimX);
		if (DotProduct > 0)
		{
			// 検出位置が画面内にあればスナップする
			FVector2D SnappablePosInScreen;
			bool bInScreen = UGameplayStatics::ProjectWorldToScreen(this, SnappablePos, SnappablePosInScreen);
			if (bInScreen)
			{
				bSnapped = true;
				BanditAimTarget = SnappablePos;
			}
		}
	}

	BanditAimWidget->ChangeMark(bSnapped);
	BanditAimWidget->MoveAimMark(BanditAimTarget);
}

void ARiderPlayerController::OnRightButtonExit(const FVector2D& _NormTouchLatestPos, const FVector2D& _NormTouchLatestVel)
{
	if(bIsBanditAiming)
	{
		bIsBanditAiming = false;
		BanditAimWidget->HideAimMark();
		BanditBand->ShootBand(BanditAimTarget);
	}
	else if (BanditBand->IsSticked())
		BanditBand->CutBand();
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

void ARiderPlayerController::OnRiderSpeedChanged(float _NewSpeed, float _DefaultSpeed, float _MinSpeed, float _MaxSpeed)
{
	RiderInfoWidget->ShowSpeed(_NewSpeed);
}

void ARiderPlayerController::OnRiderEnergyChanged(float _NewEnergy, float _MaxEnergy)
{
	RiderInfoWidget->ShowEnergyMeter(_NewEnergy / _MaxEnergy);
}

void ARiderPlayerController::OnRiderFellOff()
{
	// TODO:
	// 1. Riderが弾けるエフェクトを再生
	// 2. エフェクトが再生し終わるまで待つ
	Rider->SetActorTransform(StartTransform);
	Rider->SetSpeed(Rider->GetDefaultSpeed());
	if (Rider->IsDrifting())
		Rider->StopDrift();
}


inline bool ARiderPlayerController::LineTraceBanditSnappable(const FVector& _AimTarget, FVector& _SnappablePos)
{
	FCollisionObjectQueryParams ObjQueryParam;
	ObjQueryParam.AddObjectTypesToQuery(BanditSnapChannel);
	FCollisionQueryParams QueryParam;
	QueryParam.AddIgnoredActor(Rider);
	FHitResult Hit;
	bool bHit = GetWorld()->LineTraceSingleByObjectType(Hit, BanditBand->GetComponentLocation(), _AimTarget, ObjQueryParam, QueryParam);
	if (bHit)
		_SnappablePos = Hit.Component->GetComponentLocation();
	return bHit;
}
