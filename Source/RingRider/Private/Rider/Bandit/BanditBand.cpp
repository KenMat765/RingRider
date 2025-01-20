// Fill out your copyright notice in the Description page of Project Settings.


#include "Rider/Bandit/BanditBand.h"
#include "NiagaraComponent.h"
#include "GameInfo.h"
#include "Interface/Moveable.h"
#include "Utility/TransformUtility.h"

// Debug
#include "Kismet/KismetSystemLibrary.h"


const FString UBanditBand::BANDIT_BEAM_END	 = TEXT("BeamEnd");
const FString UBanditBand::BANDIT_BEAM_WIDTH = TEXT("BeamWidth");
const FString UBanditBand::BANDIT_COLOR		 = TEXT("Color");
const FString UBanditBand::BANDIT_INTENSITY	 = TEXT("Intensity");


UBanditBand::UBanditBand()
{
	PrimaryComponentTick.bCanEverTick = false;

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> BanditNS(TEXT("/Game/Rider/Bandit/NS_BanditBand"));
	if (BanditNS.Succeeded())
		SetAsset(BanditNS.Object);

	Psm = CreateDefaultSubobject<UPsmComponent>(TEXT("Bandit PSM"));
	AimState = [this](const FPsmInfo& Info) { this->AimStateFunc(Info); };
	Psm->AddState(AimState);
	ExpandState = [this](const FPsmInfo& Info) { this->ExpandStateFunc(Info); };
	Psm->AddState(ExpandState);
	StickState = [this](const FPsmInfo& Info) { this->StickStateFunc(Info); };
	Psm->AddState(StickState);
	PullDashState = [this](const FPsmInfo& Info) { this->PullDashStateFunc(Info); };
	Psm->AddState(PullDashState);
}


void UBanditBand::BeginPlay()
{
	Super::BeginPlay();

	OwnerMoveable = Cast<IMoveable>(GetOwner());
	ensureMsgf(OwnerMoveable, TEXT("Could not get IMoveable from Owner!!"));

	Deactivate();
}



// Aiming ////////////////////////////////////////////////////////////////////////////////////////
void UBanditBand::StartAim(const FVector& _AimTarget)
{
	UE_LOG(LogTemp, Log, TEXT("Start Aim"));

	AimTarget = _AimTarget;
	FVector SnapPos;
	if (CheckSnap(AimTarget, SnapPos))
	{
		AimTarget = SnapPos;
	}
	Psm->TurnOnState(AimState);
}

void UBanditBand::EndAim()
{
	UE_LOG(LogTemp, Log, TEXT("End Aim"));
	Psm->TurnOffState(AimState);
}

FDelegateHandle UBanditBand::AddOnStartAimAction(TFunction<void(const FVector& AimPos)> NewFunc)
{
	auto NewAction = FStartAimDelegate::FDelegate::CreateLambda(NewFunc);
	return OnStartAimActions.Add(NewAction);
}

void UBanditBand::RemoveOnStartAimAction(FDelegateHandle DelegateHandle)
{
	OnStartAimActions.Remove(DelegateHandle);
}

FDelegateHandle UBanditBand::AddOnAimingAction(TFunction<void(const FVector&)> NewFunc)
{
	auto NewAction = FAimingDelegate::FDelegate::CreateLambda(NewFunc);
	return OnAimingActions.Add(NewAction);
}

void UBanditBand::RemoveOnAimingAction(FDelegateHandle DelegateHandle)
{
	OnAimingActions.Remove(DelegateHandle);
}

FDelegateHandle UBanditBand::AddOnEndAimAction(TFunction<void()> NewFunc)
{
	auto NewAction = FEndAimDelegate::FDelegate::CreateLambda(NewFunc);
	return OnEndAimActions.Add(NewAction);
}

void UBanditBand::RemoveOnEndAimAction(FDelegateHandle DelegateHandle)
{
	OnEndAimActions.Remove(DelegateHandle);
}

bool UBanditBand::CheckSnap(const FVector& _AimTarget, FVector& SnapPos)
{
	FCollisionObjectQueryParams ObjQueryParam;
	ObjQueryParam.AddObjectTypesToQuery(ECollisionChannel::ECC_GameTraceChannel2);

	FCollisionQueryParams QueryParam;
	QueryParam.AddIgnoredActor(this->GetOwner());

	FHitResult Hit;
	bool bHit = GetWorld()->LineTraceSingleByObjectType(
		Hit,
		GetComponentLocation(),
		_AimTarget,
		ObjQueryParam,
		QueryParam
	);

	if (bHit)
	{
		SnapPos = Hit.Component->GetComponentLocation();
	}

	return bHit;
}



// Actions ///////////////////////////////////////////////////////////////////////////////////////
void UBanditBand::ShootBand(const FVector* _AimTarget)
{
	UE_LOG(LogTemp, Log, TEXT("Shoot Band"));
	if (_AimTarget)
		AimTarget = *_AimTarget;
	Psm->TurnOnState(ExpandState);
}

void UBanditBand::CutBand()
{
	UE_LOG(LogTemp, Log, TEXT("Cut Band"));
	Deactivate();
	bIsSticked = false;
	StickedPos = FVector::ZeroVector;
	StickedActor = nullptr;
	if (OnCutBand.IsBound())
		OnCutBand.Broadcast();
}

void UBanditBand::StartPullDash()
{
	if (Psm->IsStateOn(StickState))
	{
		Psm->TurnOffState(StickState);
		Psm->TurnOnState(PullDashState);
	}
}



// States /////////////////////////////////////////////////////////////////////////////////////////
void UBanditBand::AimStateFunc(const FPsmInfo& Info)
{
	switch (Info.Condition)
	{
	case EPsmCondition::ENTER:
	{
		if (OnStartAimActions.IsBound())
			OnStartAimActions.Broadcast(AimTarget);
	}
	break;

	case EPsmCondition::STAY:
	{
		FVector SnapPos;
		if (CheckSnap(AimTarget, SnapPos))
		{
			AimTarget = SnapPos;
		}
		if (OnAimingActions.IsBound())
			OnAimingActions.Broadcast(AimTarget);
	}
	break;

	case EPsmCondition::EXIT:
	{
		if (OnEndAimActions.IsBound())
			OnEndAimActions.Broadcast();
	}
	break;
	}
}

void UBanditBand::ExpandStateFunc(const FPsmInfo& Info)
{
	static float CurrentLength = 0;
	static float NextLength = 0;
	static FVector StartWorldPos;
	static FVector ShootWorldDir;

	switch (Info.Condition)
	{
	case EPsmCondition::ENTER:
	{
		CurrentLength = 0;
		NextLength = 0;
		StartWorldPos = GetComponentLocation();
		ShootWorldDir = (AimTarget - StartWorldPos).GetSafeNormal();
		SetNiagaraVariableVec3(BANDIT_BEAM_END, StartWorldPos);
		Activate();
	}
	break;

	case EPsmCondition::STAY:
	{
		// Bandが伸びきっていたら終了
		if (CurrentLength >= MaxLength)
		{
			Psm->TurnOffState(ExpandState);
			CutBand();
			break;
		}

		// 次フレームの先端位置を更新
		FVector CurrentTipWorldPos = StartWorldPos + ShootWorldDir * CurrentLength;
		NextLength += ExpandSpeed * Info.DeltaTime;
		if (NextLength > MaxLength)
			NextLength = MaxLength;
		FVector NextTipWorldPos = StartWorldPos + ShootWorldDir * NextLength;

		// 紐の先端が次フレームに移動する位置までの間にくっつく対象があるかをチェック
		// OnComponentBeginOverlapでは紐の動きが速すぎて検出漏れが発生するため、ここでマニュアルでチェックする
		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this->GetOwner());
		FCollisionObjectQueryParams ObjQueryParam;
		ObjQueryParam.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);
		ObjQueryParam.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldDynamic);
		ObjQueryParam.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn);
		bool bHit = GetWorld()->SweepSingleByObjectType(
			HitResult,
			CurrentTipWorldPos,
			NextTipWorldPos,
			FQuat::Identity,
			ObjQueryParam,
			FCollisionShape::MakeSphere(TipRadius),
			QueryParams
		);

		// くっつき対象に当たったとき
		if (bHit && HitResult.GetComponent()->ComponentHasTag(FTagList::TAG_BANDIT_STICKABLE))
		{
			NextTipWorldPos = HitResult.Location;
			StickedPos = HitResult.Location;
			StickedActor = HitResult.GetActor();
			Psm->TurnOffState(ExpandState);
			Psm->TurnOnState(StickState);
		}

		// Debug
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), NextTipWorldPos, TipRadius, 12, FLinearColor::Green);

		SetNiagaraVariableVec3(BANDIT_BEAM_END, NextTipWorldPos);

		CurrentLength = NextLength;
	}
	break;

	case EPsmCondition::EXIT:
	{
	}
	break;
	}
}

void UBanditBand::StickStateFunc(const FPsmInfo& Info)
{
	switch (Info.Condition)
	{
	case EPsmCondition::ENTER:
	{
		bIsSticked = true;
	}
	break;

	case EPsmCondition::STAY:
	{
	}
	break;

	case EPsmCondition::EXIT:
	{
	}
	break;
	}
}

void UBanditBand::PullDashStateFunc(const FPsmInfo& Info)
{
	if (!OwnerMoveable)
	{
		UE_LOG(LogTemp, Error, TEXT("Owner does not implement IMoveable!!"));
		return;
	}

	switch (Info.Condition)
	{
	case EPsmCondition::ENTER:
	{
		OwnerMoveable->AddSpeed(BoostOnPullDash);
	}
	break;

	case EPsmCondition::STAY:
	{
		AActor* Owner = GetOwner();

		// Banditがくっついている対象へ、Ownerを向かせる
		FRotator LookAtRotator = FRotatorUtility::GetLookAtRotator(Owner, StickedPos, Info.DeltaTime, TurnSpeed);
		Owner->SetActorRotation(LookAtRotator);

		// Banditがくっついている対象へ、Ownerを加速させながら移動
		OwnerMoveable->AddSpeed(AccelOnPullDash * Info.DeltaTime);
		OwnerMoveable->MoveToward(StickedPos, Info.DeltaTime);

		float CurrentLength = FVector::Distance(StickedPos, GetComponentLocation());
		UE_LOG(LogTemp, Log, TEXT("Band Length: %f"), CurrentLength);
		/*
		if (CurrentLength < NearDistanceOnPullDash)
		{
			Psm->TurnOffState(PullDashState);
		}
		else if (CurrentLength > MaxLength)
		{
			Psm->TurnOffState(PullDashState);
		}
		*/
	}
	break;

	case EPsmCondition::EXIT:
	{
		CutBand();
	}
	break;
	}
}
