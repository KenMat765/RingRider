// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/DashPole.h"
#include "Interface/Moveable.h"
#include "Utility/TransformUtility.h"
#include "Utility/ComponentUtility.h"


ADashPole::ADashPole()
{
	PrimaryActorTick.bCanEverTick = false;
}


void ADashPole::OnBanditSticked(UBanditBand* _OtherBanditBand, AActor* _OtherActor)
{
	OtherMoveable = Cast<IMoveable>(_OtherActor);
	if (!OtherMoveable)
		UE_LOG(LogTemp, Warning, TEXT("%s: Could not get IMoveable from %s"), *GetName(), *_OtherActor->GetName());
}

void ADashPole::OnBanditPulledEnter(UBanditBand* _OtherBanditBand, AActor* _OtherActor)
{
	if (OtherMoveable)
	{
		float DotProduct = FVector::DotProduct(_OtherBanditBand->GetBandDirection(), OtherMoveable->GetMoveDirection());
		float AccelRate = (DotProduct + 1) / 2.f; // 0.f [Pole���^���] ~ 1.f [Pole���^����]
		OtherMoveable->AddSpeed(AccelOnPullDashEnter * AccelRate);
	}
	else
		_OtherBanditBand->CutBand(); // �K�v�ȃC���^�[�t�F�[�X���������Ă��Ȃ��ꍇ�A�����ł��Ȃ��̂ł��̂܂ܐ؂�
}

void ADashPole::OnBanditPulledStay(UBanditBand* _OtherBanditBand, AActor* _OtherActor, float _DeltaTime)
{
	if (!OtherMoveable)
		return;

	static float PrevBandLength = INFINITY;
	FVector StickPos = _OtherBanditBand->GetStickInfo().StickPos;

	// �����̕���OtherActor����������
	FRotator LookAtRotator = FRotatorUtility::GetLookAtRotator(_OtherActor, StickPos, _DeltaTime, TurnSpeedOnPullDashStay);
	_OtherActor->SetActorRotation(LookAtRotator);

	// OtherActor�����������Ȃ���ړ�
	float DotProduct = FVector::DotProduct(_OtherBanditBand->GetBandDirection(), OtherMoveable->GetMoveDirection());
	float AccelRate = (DotProduct + 1) / 2.f; // 0.f [Pole���^���] ~ 1.f [Pole���^����]
	float Accel = AccelOnPullDashStay * AccelRate;
	OtherMoveable->AddSpeed(Accel * _DeltaTime);

	float BandLength = _OtherBanditBand->GetBandLength();
	if (BandLength <= ForceCutLength ||
		// GreatCut�����ł����Ɛ��񂳂��ƊȒP��GreatCut�ł��Ă��܂��̂ŁA������Band�����L�т��狭���J�b�g
		(BandLength <= GreatCutLength && BandLength > PrevBandLength))
	{
		bIsForceCut = true;
		PrevBandLength = INFINITY;
		_OtherBanditBand->CutBand();
	}
	else
		PrevBandLength = BandLength;
}

void ADashPole::OnBanditPulledExit(UBanditBand* _OtherBanditBand, AActor* _OtherActor)
{
	if (!OtherMoveable)
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
		UE_LOG(LogTemp, Log, TEXT("Force"));
	}
	else if (BandLength <= PerfectCutLength)
	{
		UE_LOG(LogTemp, Log, TEXT("Perfect"));
	}
	else if (BandLength <= GreatCutLength)
	{
		UE_LOG(LogTemp, Log, TEXT("Great"));
	}
}

