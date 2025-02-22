// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/DashPole.h"
#include "Interface/Moveable.h"
#include "Interface/Rotatable.h"
#include "Utility/TransformUtility.h"
#include "Utility/ComponentUtility.h"


ADashPole::ADashPole()
{
	PrimaryActorTick.bCanEverTick = false;
}


void ADashPole::OnBanditSticked(UBanditBand* _OtherBanditBand)
{
	if(!GetStickedBands().Contains(_OtherBanditBand))
		AddStickedBand(_OtherBanditBand);

	AActor* OtherActor = _OtherBanditBand->GetOwner();

	OtherMoveable = Cast<IMoveable>(OtherActor);
	if (!OtherMoveable)
		UE_LOG(LogTemp, Warning, TEXT("%s: Could not get IMoveable from %s"), *GetName(), *OtherActor->GetName());

	OtherRotatable = Cast<IRotatable>(OtherActor);
	if (!OtherRotatable)
		UE_LOG(LogTemp, Warning, TEXT("%s: Could not get IRotatable from %s"), *GetName(), *OtherActor->GetName());
}

void ADashPole::OnBanditPulledEnter(UBanditBand* _OtherBanditBand)
{
	if (!OtherMoveable || !OtherRotatable)
	{
		_OtherBanditBand->CutBand(); // �K�v�ȃC���^�[�t�F�[�X���������Ă��Ȃ��ꍇ�A�����ł��Ȃ��̂ł��̂܂ܐ؂�
		return;
	}
	float DotProduct = FVector::DotProduct(_OtherBanditBand->GetBandDirection(), OtherMoveable->GetMoveDirection());
	float AccelRate = (DotProduct + 1) / 2.f; // 0.f [Pole���^���] ~ 1.f [Pole���^����]
	OtherMoveable->AddSpeed(AccelOnPullDashEnter * AccelRate);
}

void ADashPole::OnBanditPulledStay(UBanditBand* _OtherBanditBand, float _DeltaTime)
{
	static float PrevBandLength = INFINITY;

	float BandLength = _OtherBanditBand->GetBandLength();
	FVector StickPos = _OtherBanditBand->GetStickInfo().StickPos;

	// �����̕���OtherActor����������
	if (OtherRotatable)
	{
		float BandLengthRatio = BandLength / _OtherBanditBand->GetMaxLength();
		float TurnSpeed = MinTurnSpeed + (MaxTurnSpeed - MinTurnSpeed) * FMath::Pow((1.f - BandLengthRatio), TurnSpeedPower);
		FRotator LookAtRotator = FRotatorUtility::GetLookAtRotator(_OtherBanditBand->GetOwner(), StickPos, _DeltaTime, TurnSpeed);
		OtherRotatable->SetRotation(LookAtRotator);
	}

	// OtherActor�����������Ȃ���ړ�
	if (OtherMoveable)
	{
		float DotProduct = FVector::DotProduct(_OtherBanditBand->GetBandDirection(), OtherMoveable->GetMoveDirection());
		float AccelRate = (DotProduct + 1) / 2.f; // 0.f [Pole���^���] ~ 1.f [Pole���^����]
		float Accel = AccelOnPullDashStay * AccelRate;
		OtherMoveable->AddSpeed(Accel * _DeltaTime);
	}

	if (BandLength <= ForceCutLength ||
		// PerfectCut�����ł����Ɛ��񂳂��ƊȒP��PerfectCut�ł��Ă��܂��̂ŁA������Band�����L�т��狭���J�b�g
		(BandLength <= PerfectCutLength && BandLength > PrevBandLength)
		)
	{
		bIsForceCut = true;
		PrevBandLength = INFINITY;
		_OtherBanditBand->CutBand();
	}
	else
		PrevBandLength = BandLength;
}

void ADashPole::OnBanditPulledExit(UBanditBand* _OtherBanditBand)
{
	if (!OtherMoveable || !OtherRotatable)
		return;

	// OtherActor�̂��̃A�N�^�[�ɑ΂���R���W�������ꎞ�I�ɖ���������
	UPrimitiveComponent* StickComp = _OtherBanditBand->GetStickInfo().StickComp;
	if (IsValid(StickComp))
	{
		ECollisionChannel StickChannel = StickComp->GetCollisionObjectType();
		FComponentUtility::IgnoreCollisionTemporary(GetWorldTimerManager(), StickComp, StickChannel, CollisionIgnoreSeconds);
	}

	float BandLength = _OtherBanditBand->GetBandLength();
	if (bIsForceCut)
	{
		bIsForceCut = false;
		UE_LOG(LogTemp, Log, TEXT("Force: %f"), BandLength);
	}
	else if (BandLength <= PerfectCutLength)
	{
		UE_LOG(LogTemp, Log, TEXT("Perfect: %f"), BandLength);
		OtherMoveable->AddSpeed(AccelOnPerfectCut);
	}
	else if (BandLength <= GreatCutLength)
	{
		UE_LOG(LogTemp, Log, TEXT("Great: %f"), BandLength);
		OtherMoveable->AddSpeed(AccelOnGreatCut);
	}
}

