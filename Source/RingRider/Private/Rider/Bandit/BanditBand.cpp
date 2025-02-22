// Fill out your copyright notice in the Description page of Project Settings.


#include "Rider/Bandit/BanditBand.h"
#include "GameInfo.h"
#include "Interface/BanditStickable.h"


FBanditStickInfo::FBanditStickInfo()
	: StickPos(FVector::ZeroVector), StickActor(nullptr), StickComp(nullptr), BanditStickable(nullptr) {}

FBanditStickInfo::FBanditStickInfo(const FVector& _StickPos, AActor* _StickActor, UPrimitiveComponent* _StickComp, IBanditStickable* _BanditStickable)
	: StickPos(_StickPos), StickActor(_StickActor), StickComp(_StickComp), BanditStickable(_BanditStickable) {}


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
	PullState = [this](const FFsmInfo& Info) { this->PullStateFunc(Info); };
	Fsm->AddState(PullState);
}


void UBanditBand::BeginPlay()
{
	Super::BeginPlay();

	Deactivate();
}



// Actions ///////////////////////////////////////////////////////////////////////////////////////
void UBanditBand::ShootBand(const FVector& _ShootPos)
{
	if (bCanShoot)
	{
		ShootPos = _ShootPos; // ShootPosをExpandStateで参照するため、先に更新する
		Fsm->SwitchState(&ExpandState);
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("Could not shoot because bCanShoot was false"));
}

void UBanditBand::CutBand()
{
	Fsm->SwitchToNullState(); // StickInfoを前ステートのExitで参照できるようにするため、StickInfo更新前に呼ぶ
	if (StickInfo.BanditStickable)
		StickInfo.BanditStickable->OnBanditReleased(this);	// StickInfo更新前に呼ぶ

	Deactivate();
	SetTipPos(GetComponentLocation());
	bIsSticked = false;
	StickInfo = FBanditStickInfo();

	// 一定時間後にBandを撃てるようにする
	if (ShootEnableDuration > 0.f)
	{
		FTimerHandle TimerHandle;
		FTimerDelegate EnableShootDelegate = FTimerDelegate::CreateLambda([this]() { bCanShoot = true; });
		GetOwner()->GetWorldTimerManager().SetTimer(TimerHandle, EnableShootDelegate, ShootEnableDuration, false);
	}
	else
		bCanShoot = true;
}

void UBanditBand::StickBand(FVector& _StickPos, AActor& _StickActor, UPrimitiveComponent& _StickComp, IBanditStickable& _BanditStickable)
{
	if (!_BanditStickable.IsStickable())
	{
		UE_LOG(LogTemp, Log, TEXT("%s: %s was not IsStickable()"), *GetOwner()->GetName(), *_StickActor.GetName());
		return;
	}
	Activate();
	SetTipPos(_StickPos);
	bIsSticked = true;
	StickInfo = FBanditStickInfo(_StickPos, &_StickActor, &_StickComp, &_BanditStickable);
	Fsm->SwitchState(&StickState); // StickInfoをStickStateのEnterで参照できるようにするため、StickInfo更新後に呼ぶ
}

void UBanditBand::PullBand()
{
	if (IsStickState())
		Fsm->SwitchState(&PullState);
	else
		UE_LOG(LogTemp, Warning, TEXT("Could not pull because BanditBand was not in Stick State"));
}



// States /////////////////////////////////////////////////////////////////////////////////////////
void UBanditBand::ExpandStateFunc(const FFsmInfo& Info)
{
	static FVector ShootWorldDir; 

	switch (Info.Condition)
	{
	case EFsmCondition::ENTER: {
		bCanShoot = false;
		ShootWorldDir = (ShootPos - GetComponentLocation()).GetSafeNormal();
		SetTipPos(GetComponentLocation());
		Activate();
	} break;

	case EFsmCondition::STAY: {
		FVector NextTipWorldPos = GetTipPos() + ShootWorldDir * TipSpeed * Info.DeltaTime;
		FHitResult HitResult;
		bool bFoundStickable = SearchStickableBySweep(HitResult, GetTipPos(), NextTipWorldPos);
		if (bFoundStickable)
		{
			// IBanditStickableを実装していないものにはStickしない
			IBanditStickable* BanditStickable = Cast<IBanditStickable>(HitResult.GetActor());
			if (BanditStickable && BanditStickable->IsStickable())
			{
				StickBand(HitResult.Location, *HitResult.GetActor(), *HitResult.GetComponent(), *BanditStickable); // -> Stick State
				break;
			}
		}
		SetTipPos(NextTipWorldPos);
		if (GetBandLength() >= MaxLength)
			CutBand(); // -> Null State
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
		bCanShoot = false;
		StickPos_Local = StickInfo.StickComp->GetComponentTransform().InverseTransformPosition(StickInfo.StickPos);
		StickInfo.BanditStickable->OnBanditSticked(this);
	} break;

	case EFsmCondition::STAY: {
		FVector NewTipPos = StickInfo.StickComp->GetComponentTransform().TransformPosition(StickPos_Local);
		SetTipPos(NewTipPos);
		StickInfo.StickPos = NewTipPos;

		if (GetBandLength() > MaxLength)
			CutBand(); // -> Null State
	} break;

	case EFsmCondition::EXIT: {
	} break;
	}
}

void UBanditBand::PullStateFunc(const FFsmInfo& Info)
{
	static float lifetime = 0.f;

	switch (Info.Condition)
	{
	case EFsmCondition::ENTER: {
		bCanShoot = false;
		lifetime = 0.f;
		StickInfo.BanditStickable->OnBanditPulledEnter(this);
	} break;

	case EFsmCondition::STAY: {
		FVector NewTipPos = StickInfo.StickComp->GetComponentTransform().TransformPosition(StickPos_Local);
		SetTipPos(NewTipPos);
		StickInfo.StickPos = NewTipPos;

		StickInfo.BanditStickable->OnBanditPulledStay(this, Info.DeltaTime);
		lifetime += Info.DeltaTime;
		if (GetBandLength() > MaxLength || lifetime > MaxLifetimeAfterPull)
			CutBand(); // -> Null State
	} break;

	case EFsmCondition::EXIT: {
		StickInfo.BanditStickable->OnBanditPulledExit(this);
	} break;
	}
}


void UBanditBand::SetTipPos(const FVector& _TipPos)
{
	CurrentTipPos = _TipPos;
	SetNiagaraVariableVec3(BANDIT_BEAM_END, _TipPos);
}

bool UBanditBand::SearchStickableBySweep(FHitResult& _HitResult, const FVector& _StartPos, const FVector& _EndPos)
{
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this->GetOwner());
	FCollisionObjectQueryParams ObjQueryParam;
	ObjQueryParam.AddObjectTypesToQuery(ECollisionChannel::ECC_GameTraceChannel3);	// BanditStickable
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
