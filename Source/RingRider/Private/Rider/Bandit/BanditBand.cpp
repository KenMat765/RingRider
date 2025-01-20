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



// Actions ///////////////////////////////////////////////////////////////////////////////////////
void UBanditBand::ShootBand(const FVector& _AimTarget)
{
	UE_LOG(LogTemp, Log, TEXT("Shoot Band"));
	AimTarget = _AimTarget;
	Psm->TurnOnState(ExpandState);
}

void UBanditBand::CutBand()
{
	UE_LOG(LogTemp, Log, TEXT("Cut Band"));
	Deactivate();
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
		// Band���L�т����Ă�����I��
		if (CurrentLength >= MaxLength)
		{
			Psm->TurnOffState(ExpandState);
			CutBand();
			break;
		}

		// ���t���[���̐�[�ʒu���X�V
		FVector CurrentTipWorldPos = StartWorldPos + ShootWorldDir * CurrentLength;
		NextLength += ExpandSpeed * Info.DeltaTime;
		if (NextLength > MaxLength)
			NextLength = MaxLength;
		FVector NextTipWorldPos = StartWorldPos + ShootWorldDir * NextLength;

		// �R�̐�[�����t���[���Ɉړ�����ʒu�܂ł̊Ԃɂ������Ώۂ����邩���`�F�b�N
		// OnComponentBeginOverlap�ł͕R�̓������������Č��o�R�ꂪ�������邽�߁A�����Ń}�j���A���Ń`�F�b�N����
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

		// �������Ώۂɓ��������Ƃ�
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

		// Bandit���������Ă���ΏۂցAOwner����������
		FRotator LookAtRotator = FRotatorUtility::GetLookAtRotator(Owner, StickedPos, Info.DeltaTime, TurnSpeed);
		Owner->SetActorRotation(LookAtRotator);

		// Bandit���������Ă���ΏۂցAOwner�����������Ȃ���ړ�
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
