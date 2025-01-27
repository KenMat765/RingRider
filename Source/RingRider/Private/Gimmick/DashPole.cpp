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
		_OtherBanditBand->CutBand(); // 必要なインターフェースを実装していない場合、何もできないのでそのまま切る
		return;
	}
	float DotProduct = FVector::DotProduct(_OtherBanditBand->GetBandDirection(), OtherMoveable->GetMoveDirection());
	float AccelRate = (DotProduct + 1) / 2.f; // 0.f [Poleが真後ろ] ~ 1.f [Poleが真正面]
	OtherMoveable->AddSpeed(AccelOnPullDashEnter * AccelRate);
}

void ADashPole::OnBanditPulledStay(UBanditBand* _OtherBanditBand, float _DeltaTime)
{
	static float PrevBandLength = INFINITY;

	float BandLength = _OtherBanditBand->GetBandLength();
	FVector StickPos = _OtherBanditBand->GetStickInfo().StickPos;

	// 自分の方へOtherActorを向かせる
	if (OtherRotatable)
	{
		float BandLengthRatio = BandLength / _OtherBanditBand->GetMaxLength();
		float TurnSpeed = MinTurnSpeed + (MaxTurnSpeed - MinTurnSpeed) * FMath::Pow((1.f - BandLengthRatio), TurnSpeedPower);
		FRotator LookAtRotator = FRotatorUtility::GetLookAtRotator(_OtherBanditBand->GetOwner(), StickPos, _DeltaTime, TurnSpeed);
		OtherRotatable->SetRotation(LookAtRotator);
	}

	// OtherActorを加速させながら移動
	if (OtherMoveable)
	{
		float DotProduct = FVector::DotProduct(_OtherBanditBand->GetBandDirection(), OtherMoveable->GetMoveDirection());
		float AccelRate = (DotProduct + 1) / 2.f; // 0.f [Poleが真後ろ] ~ 1.f [Poleが真正面]
		float Accel = AccelOnPullDashStay * AccelRate;
		OtherMoveable->AddSpeed(Accel * _DeltaTime);
	}

	if (BandLength <= ForceCutLength ||
		// PerfectCut圏内でずっと旋回されると簡単にPerfectCutできてしまうので、圏内でBand長が伸びたら強制カット
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

	// OtherActorのこのアクターに対するコリジョンを一時的に無効化する
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

