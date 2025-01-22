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
		_OtherBanditBand->CutBand(); // 必要なインターフェースを実装していない場合、何もできないのでそのまま切る
}

void AGroundPole::OnBanditPulledStay(UBanditBand* _OtherBanditBand, AActor* _OtherActor, float _DeltaTime)
{
	if (!OtherMoveable)
		return;

	FVector StickPos = _OtherBanditBand->GetStickInfo().StickPos;

	// 自分の方へOtherActorを向かせる
	FRotator LookAtRotator = FRotatorUtility::GetLookAtRotator(_OtherActor, StickPos, _DeltaTime, TurnSpeedOnPullDashStay);
	_OtherActor->SetActorRotation(LookAtRotator);

	// 自分の方へOtherActorを加速させながら移動
	// OtherMoveable->AddSpeed(AccelOnPullDashStay * _DeltaTime);
	OtherMoveable->MoveToward(StickPos, _DeltaTime);

	if (_OtherBanditBand->GetBandLength() <= ForceCutLength)
		_OtherBanditBand->CutBand();
}

void AGroundPole::OnBanditPulledExit(UBanditBand* _OtherBanditBand, AActor* _OtherActor)
{
	if (!OtherMoveable)
		return;

	// OtherActorのこのアクターに対するコリジョンを一時的に無効化する
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

