// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/Stone.h"
#include "Level/LevelInstance.h"
#include "DestructibleComponent.h"
#include "Interface/StoneCarryable.h"
#include "Interface/Energy.h"


AStone::AStone()
{
	PrimaryActorTick.bCanEverTick = true;

	// ===== Stone Mesh ===== //
	StoneDestructComp = CreateDefaultSubobject<UDestructibleComponent>(TEXT("Stone Destructible Mesh"));
	RootComponent = StoneDestructComp;
}

void AStone::Tick(float DeltaTime)
{
	if (IsStoneCarried())
	{
		// Carrier��ǂ�������
		FVector StoneLoc = GetActorLocation();
		FVector ActorLoc = CarrierActor->GetActorLocation();
		FVector TargetLoc = ActorLoc + FVector(0, 0, ZOffset);
		FVector DiffLoc = TargetLoc - StoneLoc;
		FVector MoveLoc = DiffLoc * ChaseRatio;
		AddActorWorldOffset(MoveLoc);

		if (IEnergy* CarrierEnergy = Cast<IEnergy>(CarrierActor))
		{
			// �G�l���M�[����
			float DeltaEnergy = DecreaseEnergyPerSec * DeltaTime;
			CarrierEnergy->AddEnergy(-DeltaEnergy);

			// �G�l���M�[���s����������
			if (CarrierEnergy->GetEnergy() <= 0.f)
			{
				bAnimating = false;
				SetStoneCarrier(nullptr);
				DestructStone();
			}
		}
		else
			UE_LOG(LogTemp, Error, TEXT("Stone: Could not get IEnergy from %s"), *GetStoneCarrier()->GetName());
	}

	// Z�����ɏ��������I�t�Z�b�g����A�j���[�V�������Đ�
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
		// �n�ʂɌ�����RayCast���A�^�C�������m
		FVector RayStart = GetActorLocation();
		FVector RayDir = FVector::DownVector;
		FVector RayEnd = RayStart + RayDir * RayDistance;
		FCollisionObjectQueryParams ObjQueryParam;
		ObjQueryParam.AddObjectTypesToQuery(ECollisionChannel::ECC_GameTraceChannel1);
		FCollisionQueryParams QueryParam;
		QueryParam.AddIgnoredActor(this);
		FHitResult Hit;
		bool bHit = GetWorld()->LineTraceSingleByObjectType(Hit, RayStart, RayEnd, ObjQueryParam, QueryParam);

		// ���m�����^�C���̃`�[����ύX
		AActor* HitActor = Hit.GetActor();
		if (HitActor && HitActor->ActorHasTag(FTagList::TAG_HEXTILE))
		{
			int TileId = Hit.Item;
			ALevelInstance* LevelInstance = Cast<ALevelInstance>(HitActor);
			if (LevelInstance)
				LevelInstance->SetTileTeam(TileId, Team);
			else
				UE_LOG(LogTemp, Error, TEXT("Stone: Could not get LevelInstance!!"));
		}
	}
}


inline void AStone::SetStoneCarrier(AActor* _NewCarrierActor)
{
	CarrierActor = _NewCarrierActor;

	// �V�����A�N�^�[�ɏ��L���ꂽ�Ƃ�
	if (_NewCarrierActor)
	{
		SetStickable(false);
		AnimTimer = 0.f;
		bAnimating = true;
	}

	// ������ꂽ�Ƃ�
	else
	{
		SetStickable(true);
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
	// �N�ɂ����L����Ă��Ȃ��X�g�[���̂�BanditBand�ň����񂹂邱�Ƃ��ł���
	if (!IsStoneCarried())
	{
		AActor* OtherActor = _OtherBanditBand->GetOwner();
		if (IStoneCarryable* StoneCarryable = Cast<IStoneCarryable>(OtherActor))
			StoneCarryable->CarryStone(this);
		else
			UE_LOG(LogTemp, Warning, TEXT("Stone: Could not get IStoneCarryable from %s"), *OtherActor->GetName());
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("Stone: Already carried by %s"), *GetStoneCarrier()->GetName());
	_OtherBanditBand->CutBand();
}


