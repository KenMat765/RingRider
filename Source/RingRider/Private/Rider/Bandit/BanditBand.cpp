// Fill out your copyright notice in the Description page of Project Settings.


#include "Rider/Bandit/BanditBand.h"
#include "GameInfo.h"
#include "Interface/BanditStickable.h"


FBanditStickInfo::FBanditStickInfo():
	StickPos(FVector::ZeroVector), StickActor(nullptr), StickComp(nullptr) {}

FBanditStickInfo::FBanditStickInfo(const FVector& _StickPos, AActor* _StickActor, UPrimitiveComponent* _StickComp):
	StickPos(_StickPos), StickActor(_StickActor), StickComp(_StickComp) {}


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
		ShootPos = _ShootPos; // ShootPos��ExpandState�ŎQ�Ƃ��邽�߁A��ɍX�V����
		Fsm->SwitchState(&ExpandState);
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("Could not shoot because bCanShoot was false"));
}

void UBanditBand::CutBand()
{
	Fsm->SwitchToNullState(); // StickInfo��O�X�e�[�g��Exit�ŎQ�Ƃł���悤�ɂ��邽�߁AStickInfo�X�V�O�ɌĂ�
	Deactivate();
	SetTipPos(GetComponentLocation());
	bIsSticked = false;
	StickInfo = FBanditStickInfo();

	// ��莞�Ԍ��Band�����Ă�悤�ɂ���
	if (ShootEnableDuration > 0.f)
	{
		FTimerHandle TimerHandle;
		FTimerDelegate EnableShootDelegate = FTimerDelegate::CreateLambda([this]() { bCanShoot = true; });
		GetOwner()->GetWorldTimerManager().SetTimer(TimerHandle, EnableShootDelegate, ShootEnableDuration, false);
	}
	else
		bCanShoot = true;
}

void UBanditBand::StickBand(const FBanditStickInfo& _StickInfo)
{
	Activate();
	SetTipPos(_StickInfo.StickPos);
	bIsSticked = true;
	StickInfo = _StickInfo;
	Fsm->SwitchState(&StickState); // StickInfo��StickState��Enter�ŎQ�Ƃł���悤�ɂ��邽�߁AStickInfo�X�V��ɌĂ�
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
			StickBand(FBanditStickInfo(HitResult.Location, HitResult.GetActor(), HitResult.GetComponent())); // -> Stick State
		else
		{
			SetTipPos(NextTipWorldPos);
			if (GetBandLength() >= MaxLength)
				CutBand(); // -> Null State
		}
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
		AActor* StickActor = GetStickInfo().StickActor;
		if (!IsValid(StickActor))
		{
			UE_LOG(LogTemp, Error, TEXT("%s: StickActor was invalid!!"), *(GetOwner()->GetName()));
			return;
		}

		BanditStickable = Cast<IBanditStickable>(StickActor);
		if (!BanditStickable)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s: Could not get IBanditStickable from %s"), *GetOwner()->GetName(), *StickActor->GetName());
			return;
		}
		BanditStickable->OnBanditSticked(this, GetOwner());
	} break;

	case EFsmCondition::STAY: {
		if (GetBandLength() > MaxLength)
			CutBand(); // -> Null State
	} break;

	case EFsmCondition::EXIT: {
	} break;
	}
}

void UBanditBand::PullStateFunc(const FFsmInfo& Info)
{
	switch (Info.Condition)
	{
	case EFsmCondition::ENTER: {
		bCanShoot = false;
		if (BanditStickable)
			BanditStickable->OnBanditPulledEnter(this, GetOwner());
		else
			CutBand(); // IBanditStickable�����������ꍇ�A�����������璼���ɐ؂�
	} break;

	case EFsmCondition::STAY: {
		if (BanditStickable)
			BanditStickable->OnBanditPulledStay(this, GetOwner(), Info.DeltaTime);

		if (GetBandLength() > MaxLength)
			CutBand(); // -> Null State
	} break;

	case EFsmCondition::EXIT: {
		if (BanditStickable)
			BanditStickable->OnBanditPulledExit(this, GetOwner());
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
