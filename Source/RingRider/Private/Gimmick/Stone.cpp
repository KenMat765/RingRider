// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/Stone.h"
#include "Rider/Rider.h"
#include "Level/LevelInstance.h"
#include "DestructibleComponent.h"


AStone::AStone()
{
	PrimaryActorTick.bCanEverTick = true;


	// ===== Stone Mesh ===== //
	StoneDestructComp = CreateDefaultSubobject<UDestructibleComponent>(TEXT("Stone Destructible Mesh"));
	RootComponent = StoneDestructComp;
}



void AStone::BeginPlay()
{
	Super::BeginPlay();

	StoneDestructComp->OnComponentBeginOverlap.AddDynamic(this, &AStone::OnOverlapBegin);
}



void AStone::Tick(float DeltaTime)
{
	if (OwnerRider)
	{
		// Riderを追いかける
		FVector StoneLoc = GetActorLocation();
		FVector RiderLoc = OwnerRider->GetActorLocation();
		FVector TargetLoc = RiderLoc + FVector(0, 0, ZOffset);
		FVector DiffLoc = TargetLoc - StoneLoc;
		FVector MoveLoc = DiffLoc * ChaseRatio;
		AddActorWorldOffset(MoveLoc);

		// Riderのエネルギー消費
		float DeltaEnergy = DecreaseEnergyPerSec * DeltaTime;
		OwnerRider->AddEnergy(-DeltaEnergy);

		// エネルギーが尽きたら崩れる
		float OwnerRiderEnergy = OwnerRider->GetEnergy();
		if (OwnerRiderEnergy <= 0.f)
		{
			OwnerRider = nullptr;
			if (Animating)
				StopZOffsetAnimation();
			DestructStone();
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AStone::DestroyStone, DestroyDelay, false);
		}
	}

	// Z方向に少しだけオフセットするアニメーションを再生
	if (Animating)
	{
		AnimTimer += DeltaTime;
		float AnimTimeRatio = AnimTimer / AnimDuration;	// 0 -> 1
		float AnimCurveValue = AnimCurve->GetFloatValue(AnimTimeRatio);	// 0 -> 1 -> 0

		float ZOffset_ = AnimMaxZOffset * AnimCurveValue;
		AddActorWorldOffset(FVector(0, 0, ZOffset_));

		if (AnimTimer >= AnimDuration)
		{
			StopZOffsetAnimation();
		}
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
		bool bHit = GetWorld()->LineTraceSingleByObjectType(
			Hit,
			RayStart,
			RayEnd,
			ObjQueryParam,
			QueryParam
		);


		// Hitした対象がタイルか判定
		AActor* HitActor = Hit.GetActor();
		if (HitActor == nullptr)
			return;
		if (!HitActor->ActorHasTag(FTagList::TAG_HEXTILE))
			return;


		// タイルのチームを変更
		int TileId = Hit.Item;
		ALevelInstance* LevelInstance = Cast<ALevelInstance>(HitActor);
		if (!LevelInstance)
		{
			UE_LOG(LogTemp, Error, TEXT("Could not get LevelInstance!!"));
			return;
		}
		LevelInstance->SetTileTeam(TileId, Team);
	}
}



void AStone::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* Other,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& Hit
)
{
	if (Other->ActorHasTag(FTagList::TAG_RIDER))
	{
		ARider* OverlappedRider = Cast<ARider>(Other);
		if (OverlappedRider == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Could not cast to ARider!!"));
			return;
		}
		OnOwnedByRider(OverlappedRider);
	}
}



// Owner Rider ///////////////////////////////////////////////////////////////////////////////////////////
void AStone::OnOwnedByRider(ARider* NewOwnerRider)
{
	// Stoneを地面から拾った場合 (!= 他のRiderから奪った場合)
	if (OwnerRider == nullptr)
	{
		StartZOffsetAnimation();
	}

	OwnerRider = NewOwnerRider;

	// タイルのチーム変更を開始
	ETeam OwnerRiderTeam = NewOwnerRider->GetTeam();
	SetTeam(OwnerRiderTeam);
	SetCanChangeTile(true);
}



// Destructible Mesh /////////////////////////////////////////////////////////////////////////////////////
inline void AStone::DestructStone()
{
	float DamageAmount = 10;
	FVector HitLoc = GetActorLocation();
	FVector ImpulseDir = FVector::UpVector;
	StoneDestructComp->ApplyDamage(DamageAmount, HitLoc, ImpulseDir, DestructImpulse);
}

void AStone::DestroyStone()
{
	Destroy();
}

