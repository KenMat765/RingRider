// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/Stone.h"
#include "Level/LevelInstance.h"
#include "DestructibleComponent.h"
#include "Rider/Bandit/BanditSnapArea.h"
#include "Interface/StoneCarryable.h"


AStone::AStone()
{
	PrimaryActorTick.bCanEverTick = true;

	// ===== Stone Mesh ===== //
	StoneDestructComp = CreateDefaultSubobject<UDestructibleComponent>(TEXT("Stone Destructible Mesh"));
	RootComponent = StoneDestructComp;
	StoneDestructComp->SetCollisionProfileName(TEXT("BanditStickableBlock"));
	StoneDestructComp->ComponentTags.Add(TAG_BOUNCE);

	// ===== Bandit Snap Area ===== //
	BanditSnapArea = CreateDefaultSubobject<UBanditSnapArea>(TEXT("Bandit Snap Area"));
	BanditSnapArea->SetupAttachment(RootComponent);
}

void AStone::Tick(float DeltaTime)
{
	if (IsStoneCarried())
	{
		AActor* CarrierActor = StoneCarrier->GetActor();

		// Carrierを追いかける
		FVector TargetLoc = CarrierActor->GetActorLocation() + FVector(0, 0, ZOffset);
		FVector DiffLoc = TargetLoc - GetActorLocation();
		FVector MoveLoc = DiffLoc * ChaseRatio;
		AddActorWorldOffset(MoveLoc);
	}

	// Z方向に少しだけオフセットするアニメーションを再生
	if (bAnimating)
	{
		AnimTimer += DeltaTime;
		float AnimTimeRatio = AnimTimer / AnimDuration;	// 0 -> 1
		float AnimCurveValue = AnimCurve->GetFloatValue(AnimTimeRatio);	// 0 -> 1 -> 0
		float _ZOffset = AnimMaxZOffset * AnimCurveValue;
		AddActorWorldOffset(FVector(0, 0, _ZOffset));
		if (AnimTimer >= AnimDuration)
			bAnimating = false;
	}

	if (bCanChangeTile)
	{
		// 地面に向けてRayCastし、タイルを検知
		FVector RayStart = GetActorLocation();
		FVector RayDir = FVector::DownVector;
		FVector RayEnd = RayStart + RayDir * RayDistance;
		FCollisionObjectQueryParams ObjQueryParam;
		ObjQueryParam.AddObjectTypesToQuery(ECollisionChannel::ECC_GameTraceChannel1);
		FCollisionQueryParams QueryParam;
		QueryParam.AddIgnoredActor(this);
		FHitResult Hit;
		bool bHit = GetWorld()->LineTraceSingleByObjectType(Hit, RayStart, RayEnd, ObjQueryParam, QueryParam);

		// 検知したタイルのチームを変更
		AActor* HitActor = Hit.GetActor();
		if (HitActor && HitActor->ActorHasTag(TAG_HEXTILE))
		{
			int TileId = Hit.Item;
			ALevelInstance* LevelInstance = Cast<ALevelInstance>(HitActor);
			if (LevelInstance)
				LevelInstance->ChangeTileTeam(TileId, Team);
			else
				UE_LOG(LogTemp, Error, TEXT("Stone: Could not get LevelInstance!!"));
		}
	}
}


inline void AStone::SetStoneCarrier(IStoneCarryable* _NewStoneCarrier)
{
	StoneCarrier = _NewStoneCarrier;

	// 新しいアクターに所有されたとき
	if (_NewStoneCarrier)
	{
		SetStickable(false);
		BanditSnapArea->EnableSnap(false);
		AnimTimer = 0.f;
		bAnimating = true;
	}

	// 手放されたとき
	else
	{
		SetStickable(true);
		BanditSnapArea->EnableSnap(true);
	}
}


inline void AStone::DestructStone()
{
	float DamageAmount = 10;
	FVector HitLoc = GetActorLocation();
	FVector ImpulseDir = FVector::UpVector;
	StoneDestructComp->ApplyDamage(DamageAmount, HitLoc, ImpulseDir, DestructImpulse);
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this](){this->Destroy();}, DestroyDelay, false);
}


void AStone::OnBanditPulledEnter(UBanditBand* _OtherBanditBand)
{
	// 誰にも所有されていないストーンのみBanditBandで引き寄せることができる
	if (!IsStoneCarried())
	{
		AActor* OtherActor = _OtherBanditBand->GetOwner();
		if (IStoneCarryable* StoneCarryable = Cast<IStoneCarryable>(OtherActor))
			StoneCarryable->CarryStone(this);
		else
			UE_LOG(LogTemp, Warning, TEXT("Stone: Could not get IStoneCarryable from %s"), *OtherActor->GetName());
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("Stone: Already carried by %s"), *GetStoneCarrier()->GetActor()->GetName());
	_OtherBanditBand->CutBand();
}


