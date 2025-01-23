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
		float AccelRate = (DotProduct + 1) / 2.f; // 0.f [Poleが真後ろ] ~ 1.f [Poleが真正面]
		OtherMoveable->AddSpeed(AccelOnPullDashEnter * AccelRate);
	}
	else
		_OtherBanditBand->CutBand(); // 必要なインターフェースを実装していない場合、何もできないのでそのまま切る
}

void ADashPole::OnBanditPulledStay(UBanditBand* _OtherBanditBand, AActor* _OtherActor, float _DeltaTime)
{
	if (!OtherMoveable)
		return;

	static float PrevBandLength = INFINITY;
	FVector StickPos = _OtherBanditBand->GetStickInfo().StickPos;

	// 自分の方へOtherActorを向かせる
	FRotator LookAtRotator = FRotatorUtility::GetLookAtRotator(_OtherActor, StickPos, _DeltaTime, TurnSpeedOnPullDashStay);
	_OtherActor->SetActorRotation(LookAtRotator);

	// OtherActorを加速させながら移動
	float DotProduct = FVector::DotProduct(_OtherBanditBand->GetBandDirection(), OtherMoveable->GetMoveDirection());
	float AccelRate = (DotProduct + 1) / 2.f; // 0.f [Poleが真後ろ] ~ 1.f [Poleが真正面]
	float Accel = AccelOnPullDashStay * AccelRate;
	OtherMoveable->AddSpeed(Accel * _DeltaTime);

	float BandLength = _OtherBanditBand->GetBandLength();
	if (BandLength <= ForceCutLength ||
		// GreatCut圏内でずっと旋回されると簡単にGreatCutできてしまうので、圏内でBand長が伸びたら強制カット
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

