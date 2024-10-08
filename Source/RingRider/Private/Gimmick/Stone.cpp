// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/Stone.h"
#include "TagList.h"
#include "Rider/Rider.h"
#include "Level/LevelInstance.h"


AStone::AStone()
{
	PrimaryActorTick.bCanEverTick = true;



	// ===== Stone Mesh ===== //
	StoneMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Stone Mesh"));
	RootComponent = StoneMeshComp;

	// Mesh
	const TCHAR StoneMeshPath[] = TEXT("/Game/Gimmick/Stone/Stone");
	UStaticMesh* StoneMesh = LoadObject<UStaticMesh>(nullptr, StoneMeshPath);
	StoneMeshComp->SetStaticMesh(StoneMesh);

	// Physics
	StoneMeshComp->SetSimulatePhysics(false);

	// Collision
	StoneMeshComp->SetCollisionProfileName(TEXT("OverlapAllDynamic"));	// OverlapOnlyPawn���ƌ��m����Ȃ�����
	StoneMeshComp->SetNotifyRigidBodyCollision(false);
	StoneMeshComp->SetGenerateOverlapEvents(true);
}



void AStone::BeginPlay()
{
	Super::BeginPlay();

	StoneMeshComp->OnComponentBeginOverlap.AddDynamic(this, &AStone::OnOverlapBegin);
}



void AStone::Tick(float DeltaTime)
{
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
		bool bHit = GetWorld()->LineTraceSingleByObjectType(
			Hit,
			RayStart,
			RayEnd,
			ObjQueryParam,
			QueryParam
		);


		// Hit�����Ώۂ��^�C��������
		AActor* HitActor = Hit.GetActor();
		if (HitActor == nullptr)
			return;
		if (!HitActor->ActorHasTag(FTagList::TAG_HEXTILE))
			return;


		// �^�C���̃`�[����ύX
		int TileId = Hit.Item;
		ALevelInstance* LevelInstance = Cast<ALevelInstance>(HitActor);
		if (!LevelInstance)
		{
			UE_LOG(LogTemp, Error, TEXT("Could not get LevelInstance!!"));
			return;
		}
		LevelInstance->SetTileTeam(TileId, Team);
	}


	if (OwnerRider)
	{
		// Rider�̃G�l���M�[����
		float DeltaEnergy = DecreaseEnergyPerSec * DeltaTime;
		OwnerRider->AddEnergy(-DeltaEnergy);

		// �G�l���M�[���s�����痣���
		float OwnerRiderEnergy = OwnerRider->GetEnergy();
		if (OwnerRiderEnergy <= 0.f)
		{
		
		}
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
		OwnerRider = OverlappedRider;

		// Rider�̓���ֈړ�
		AttachToActor(OverlappedRider, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		SetActorRelativeLocation(FVector(0, 0, ZOffset));

		// �^�C���̃`�[���ύX���J�n
		ETeam OverlappedRiderTeam = OverlappedRider->GetTeam();
		SetTeam(OverlappedRiderTeam);
		SetCanChangeTile(true);
	}
}

