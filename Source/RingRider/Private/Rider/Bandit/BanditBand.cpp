// Fill out your copyright notice in the Description page of Project Settings.


#include "Rider/Bandit/BanditBand.h"
#include "NiagaraComponent.h"
#include "GameInfo.h"
#include "Interface/Moveable.h"
#include "Utility/TransformUtility.h"


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

	Fsm = CreateDefaultSubobject<UFsmComponent>(TEXT("Bandit Fsm"));
	ExpandState = [this](const FFsmInfo& Info) { this->ExpandStateFunc(Info); };
	Fsm->AddState(ExpandState);
	StickState = [this](const FFsmInfo& Info) { this->StickStateFunc(Info); };
	Fsm->AddState(StickState);
	PullDashState = [this](const FFsmInfo& Info) { this->PullDashStateFunc(Info); };
	Fsm->AddState(PullDashState);
}


void UBanditBand::BeginPlay()
{
	Super::BeginPlay();

	OwnerMoveable = Cast<IMoveable>(GetOwner());
	ensureMsgf(OwnerMoveable, TEXT("Could not get IMoveable from Owner!!"));

	Deactivate();
}



// Actions ///////////////////////////////////////////////////////////////////////////////////////
void UBanditBand::ShootBand(const FVector& _ShootPos)
{
	ShootPos = _ShootPos;
	Fsm->SwitchState(&ExpandState);
}

void UBanditBand::CutBand()
{
	Deactivate();
	SetTipPos(GetComponentLocation());
	Fsm->SwitchToNullState();
	bIsSticked = false;
	StickedPos = FVector::ZeroVector;
	StickedActor = nullptr;
	if (OnCutBand.IsBound())
		OnCutBand.Broadcast();
}

void UBanditBand::StickBand(const FVector& _StickPos, AActor* _StickActor)
{
	Activate();
	SetTipPos(_StickPos);
	Fsm->SwitchState(&StickState);
	bIsSticked = true;
	StickedPos = _StickPos;
	StickedActor = _StickActor;
}

void UBanditBand::StartPullDash()
{
	if (IsStickState())
		Fsm->SwitchState(&PullDashState);
	else
		UE_LOG(LogTemp, Warning, TEXT("Could not pull dash because BanditBand was not in Stick State"));
}

inline FVector UBanditBand::GetTipPos() const
{
	// 射出前の状態であれば、先端の位置はコンポーネントの位置と同じ
	if (Fsm->IsNullState())
		return GetComponentLocation();
	return CurrentTipPos;
}

inline float UBanditBand::GetBandLength() const
{
	// 射出前の状態であれば、長さは0
	if (Fsm->IsNullState())
		return 0.f;
	return CurrentBandLength;
}



// States /////////////////////////////////////////////////////////////////////////////////////////
void UBanditBand::ExpandStateFunc(const FFsmInfo& Info)
{
	static float CurrentLength = 0;
	static FVector StartWorldPos;
	static FVector ShootWorldDir;

	switch (Info.Condition)
	{
	case EFsmCondition::ENTER: {
		CurrentLength = 0;
		StartWorldPos = GetComponentLocation();
		ShootWorldDir = (ShootPos - StartWorldPos).GetSafeNormal();
		SetTipPos(StartWorldPos);
		Activate();
	} break;

	case EFsmCondition::STAY: {
		// Bandが伸びきっていたら終了
		if (CurrentLength >= MaxLength)
		{
			CutBand();
			break;
		}

		// 現在と次フレームの先端位置を更新
		FVector CurrentTipWorldPos = StartWorldPos + ShootWorldDir * CurrentLength;
		CurrentLength += ExpandSpeed * Info.DeltaTime;
		if (CurrentLength > MaxLength)
			CurrentLength = MaxLength;
		FVector NextTipWorldPos = StartWorldPos + ShootWorldDir * CurrentLength;

		// 紐の先端が次フレームに移動する位置までの間にくっつく対象があるかをチェック
		FHitResult HitResult;
		bool bFoundStickable = SearchStickableBySweep(HitResult, CurrentTipWorldPos, NextTipWorldPos);

		// くっつき対象に当たったとき
		if (bFoundStickable)
			StickBand(HitResult.Location, HitResult.GetActor());
		else
			SetTipPos(NextTipWorldPos);
	} break;

	case EFsmCondition::EXIT: {
	} break;
	}
}

void UBanditBand::StickStateFunc(const FFsmInfo& Info)
{
	switch (Info.Condition)
	{
	case EFsmCondition::ENTER: {
	} break;

	case EFsmCondition::STAY: {
	} break;

	case EFsmCondition::EXIT: {
	} break;
	}
}

void UBanditBand::PullDashStateFunc(const FFsmInfo& Info)
{
	if (!OwnerMoveable)
	{
		UE_LOG(LogTemp, Error, TEXT("Owner does not implement IMoveable!!"));
		return;
	}

	switch (Info.Condition)
	{
	case EFsmCondition::ENTER: {
		OwnerMoveable->AddSpeed(BoostOnPullDash);
	} break;

	case EFsmCondition::STAY: {
		AActor* Owner = GetOwner();

		// Banditがくっついている対象へ、Ownerを向かせる
		FRotator LookAtRotator = FRotatorUtility::GetLookAtRotator(Owner, StickedPos, Info.DeltaTime, TurnSpeed);
		Owner->SetActorRotation(LookAtRotator);

		// Banditがくっついている対象へ、Ownerを加速させながら移動
		OwnerMoveable->AddSpeed(AccelOnPullDash * Info.DeltaTime);
		OwnerMoveable->MoveToward(StickedPos, Info.DeltaTime);

		/*
		if (CurrentLength < NearDistanceOnPullDash)
		{
			Fsm->TurnOffState(PullDashState);
		}
		else if (CurrentLength > MaxLength)
		{
			Fsm->TurnOffState(PullDashState);
		}
		*/
	} break;

	case EFsmCondition::EXIT: {
	} break;
	}
}


void UBanditBand::SetTipPos(const FVector& _TipPos)
{
	CurrentTipPos = _TipPos;
	CurrentBandLength = (_TipPos - GetComponentLocation()).Size();
	SetNiagaraVariableVec3(BANDIT_BEAM_END, _TipPos);
}

bool UBanditBand::SearchStickableBySweep(FHitResult& _HitResult, const FVector& _StartPos, const FVector& _EndPos)
{
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this->GetOwner());
	FCollisionObjectQueryParams ObjQueryParam;
	ObjQueryParam.AddObjectTypesToQuery(ECollisionChannel::ECC_GameTraceChannel3);	// BanditStickableBlock
	ObjQueryParam.AddObjectTypesToQuery(ECollisionChannel::ECC_GameTraceChannel4);	// BanditStickableOverlap
	bool bHit = GetWorld()->SweepSingleByObjectType(
		_HitResult,
		_StartPos,
		_EndPos,
		FQuat::Identity,
		ObjQueryParam,
		FCollisionShape::MakeSphere(TipRadius),
		QueryParams
	);
	return bHit;
}
