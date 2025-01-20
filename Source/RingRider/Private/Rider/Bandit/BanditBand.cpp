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
void UBanditBand::ShootBand(const FVector& _AimTarget)
{
	AimTarget = _AimTarget;
	Fsm->SwitchState(&ExpandState);
}

void UBanditBand::CutBand()
{
	Deactivate();
	Fsm->SwitchToNullState();
	StickedPos = FVector::ZeroVector;
	StickedActor = nullptr;
	if (OnCutBand.IsBound())
		OnCutBand.Broadcast();
}

void UBanditBand::StartPullDash()
{
	if (Fsm->GetCurrentState() == &StickState)
		Fsm->SwitchState(&PullDashState);
}



// States /////////////////////////////////////////////////////////////////////////////////////////
void UBanditBand::ExpandStateFunc(const FFsmInfo& Info)
{
	static float CurrentLength = 0;
	static float NextLength = 0;
	static FVector StartWorldPos;
	static FVector ShootWorldDir;

	switch (Info.Condition)
	{
	case EFsmCondition::ENTER: {
		CurrentLength = 0;
		NextLength = 0;
		StartWorldPos = GetComponentLocation();
		ShootWorldDir = (AimTarget - StartWorldPos).GetSafeNormal();
		SetNiagaraVariableVec3(BANDIT_BEAM_END, StartWorldPos);
		Activate();
	} break;

	case EFsmCondition::STAY: {
		// Bandが伸びきっていたら終了
		if (CurrentLength >= MaxLength)
		{
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
			Fsm->SwitchState(&StickState);
		}

		// Debug
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), NextTipWorldPos, TipRadius, 12, FLinearColor::Green);

		SetNiagaraVariableVec3(BANDIT_BEAM_END, NextTipWorldPos);

		CurrentLength = NextLength;
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

		float CurrentLength = FVector::Distance(StickedPos, GetComponentLocation());
		UE_LOG(LogTemp, Log, TEXT("Band Length: %f"), CurrentLength);
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
		CutBand();
	} break;
	}
}
