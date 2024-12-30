// Fill out your copyright notice in the Description page of Project Settings.


#include "Rider/Bandit/BanditBand.h"
#include "NiagaraComponent.h"
#include "GameInfo.h"

// Debug
#include "Kismet/KismetSystemLibrary.h"


const FString UBanditBand::BANDIT_BEAM_END	 = TEXT("BeamEnd");
const FString UBanditBand::BANDIT_BEAM_WIDTH = TEXT("BeamWidth");
const FString UBanditBand::BANDIT_COLOR		 = TEXT("Color");
const FString UBanditBand::BANDIT_INTENSITY	 = TEXT("Intensity");


UBanditBand::UBanditBand()
{
	PrimaryComponentTick.bCanEverTick = true;

	BanditVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Bandit Band"));
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> BanditNS(TEXT("/Game/Rider/Bandit/NS_BanditBand"));
	if (BanditNS.Succeeded())
		BanditVFX->SetAsset(BanditNS.Object);

	Psm = CreateDefaultSubobject<UPsmComponent>(TEXT("Bandit PSM"));
	ExpandState = [this](const FPsmInfo& Info) { this->ExpandStateFunc(Info); };
	Psm->AddState(ExpandState);
	StickState = [this](const FPsmInfo& Info) { this->StickStateFunc(Info); };
	Psm->AddState(StickState);
}


void UBanditBand::BeginPlay()
{
	Super::BeginPlay();

	if(BanditVFX)
		BanditVFX->Deactivate();
}


void UBanditBand::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsAiming)
	{
		FVector SnapPos;
		if (CheckSnap(AimTarget, SnapPos))
		{
			AimTarget = SnapPos;
		}

		if (OnAimingActions.IsBound())
			OnAimingActions.Broadcast(AimTarget);
	}
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

	bIsAiming = true;
	if (OnStartAimActions.IsBound())
		OnStartAimActions.Broadcast(AimTarget);
}

void UBanditBand::EndAim()
{
	UE_LOG(LogTemp, Log, TEXT("End Aim"));

	bIsAiming = false;
	if (OnEndAimActions.IsBound())
		OnEndAimActions.Broadcast();
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
		FVector HitActorWorldLoc = Hit.Actor->GetActorLocation();
		FVector HitCompLocalLoc = Hit.Component->GetComponentLocation();
		SnapPos = HitActorWorldLoc + HitCompLocalLoc;
	}

	return bHit;
}



// Shoot Out ////////////////////////////////////////////////////////////////////////////////////
void UBanditBand::ShootBand(const FVector* _AimTarget)
{
	UE_LOG(LogTemp, Log, TEXT("Shoot Band"));
	if (_AimTarget)
		AimTarget = *_AimTarget;
	Psm->TurnOnState(ExpandState);
}

void UBanditBand::CutBand()
{
	// TODO
	UE_LOG(LogTemp, Log, TEXT("Cut Band"));
	BanditVFX->Deactivate();
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
		UE_LOG(LogTemp, Log, TEXT("Enter bandit expand state"));
		CurrentLength = 0;
		NextLength = 0;
		StartWorldPos = GetComponentLocation();
		ShootWorldDir = (AimTarget - StartWorldPos).GetSafeNormal();
		BanditVFX->SetNiagaraVariableVec3(BANDIT_BEAM_END, StartWorldPos);
		BanditVFX->Activate();
	}
	break;

	case EPsmCondition::STAY:
	{
		// Bandが伸びきっていたら終了
		if (CurrentLength >= MaxLength)
		{
			UE_LOG(LogTemp, Log, TEXT("Bandit reached max length"));
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
		bool bHit = GetWorld()->SweepSingleByChannel(
			HitResult,
			CurrentTipWorldPos,
			NextTipWorldPos,
			FQuat::Identity,
			ECollisionChannel::ECC_GameTraceChannel3,
			FCollisionShape::MakeSphere(TipRadius),
			QueryParams
		);

		// くっつき対象に当たったとき
		if (bHit)
		{
			// Debug
			FString ActorName = HitResult.GetActor()->GetFName().ToString();
			FString ComponentName = HitResult.GetComponent()->GetFName().ToString();
			UE_LOG(LogTemp, Log, TEXT("Hit Actor: %s, Comp: %s"), *ActorName, *ComponentName);

			NextTipWorldPos = HitResult.Location;
			Psm->TurnOffState(ExpandState);
			Psm->TurnOnState(StickState);
		}

		// Debug
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), NextTipWorldPos, TipRadius, 12, FLinearColor::Green);

		BanditVFX->SetNiagaraVariableVec3(BANDIT_BEAM_END, NextTipWorldPos);

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
