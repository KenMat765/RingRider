// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/GroundPole.h"
#include "Interface/Moveable.h"
#include "Utility/TransformUtility.h"
#include "Utility/ComponentUtility.h"


AGroundPole::AGroundPole()
{
	PrimaryActorTick.bCanEverTick = false;
}


void AGroundPole::OnBanditSticked(UBanditBand* _OtherBanditBand, AActor* _OtherActor)
{
	OtherMoveable = Cast<IMoveable>(_OtherActor);
	if (!OtherMoveable)
		UE_LOG(LogTemp, Warning, TEXT("%s: Could not get IMoveable from %s"), *GetName(), *_OtherActor->GetName());
}

void AGroundPole::OnBanditPulledEnter(UBanditBand* _OtherBanditBand, AActor* _OtherActor)
{
	if (OtherMoveable)
		OtherMoveable->AddSpeed(AccelOnPullDashEnter);
	else
		_OtherBanditBand->CutBand(); // �K�v�ȃC���^�[�t�F�[�X���������Ă��Ȃ��ꍇ�A�����ł��Ȃ��̂ł��̂܂ܐ؂�
}

void AGroundPole::OnBanditPulledStay(UBanditBand* _OtherBanditBand, AActor* _OtherActor, float _DeltaTime)
{
	if (!OtherMoveable)
		return;

	FVector StickPos = _OtherBanditBand->GetStickInfo().StickPos;

	// �����̕���OtherActor����������
	FRotator LookAtRotator = FRotatorUtility::GetLookAtRotator(_OtherActor, StickPos, _DeltaTime, TurnSpeedOnPullDashStay);
	_OtherActor->SetActorRotation(LookAtRotator);

	// �����̕���OtherActor�����������Ȃ���ړ�
	// OtherMoveable->AddSpeed(AccelOnPullDashStay * _DeltaTime);
	OtherMoveable->MoveToward(StickPos, _DeltaTime);

	if (_OtherBanditBand->GetBandLength() <= ForceCutLength)
		_OtherBanditBand->CutBand();
}

void AGroundPole::OnBanditPulledExit(UBanditBand* _OtherBanditBand, AActor* _OtherActor)
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
	if (BandLength <= ForceCutLength)
	{
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

