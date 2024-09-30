// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/Ring.h"
#include "Components/BoxComponent.h"
#include "Rider/Rider.h"


ARing::ARing():
	bIsPassed(false),
	AnimTimer(0.f),
	AnimDuration(1.f),
	RotateSpeed(1080.f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;



	// ===== Actor Settings ===== //
	Tags.Add(FName("Bounce"));

	const TCHAR AnimCurvePath[] = TEXT("/Game/Gimmick/RingAnimCurve");
	AnimCurve = LoadObject<UCurveFloat>(nullptr, AnimCurvePath);



	// ===== Ring Static Mesh ===== //
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ring Mesh"));
	RootComponent = MeshComp;

	// Mesh
	const TCHAR RingMeshPath[] = TEXT("/Game/Gimmick/Ring");
	UStaticMesh* RingMesh = LoadObject<UStaticMesh>(nullptr, RingMeshPath);
	MeshComp->SetStaticMesh(RingMesh);

	// Material
	const TCHAR RingMat0Path[] = TEXT("/Game/Gimmick/Material_Base");
	UMaterialInterface* RingMat0 = LoadObject<UMaterialInterface>(nullptr, RingMat0Path);
	MeshComp->SetMaterial(0, RingMat0);

	const TCHAR RingMat1Path[] = TEXT("/Game/Gimmick/Material_Neon");
	UMaterialInterface* RingMat1 = LoadObject<UMaterialInterface>(nullptr, RingMat1Path);
	MeshComp->SetMaterial(1, RingMat1);

	// Collision
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetGenerateOverlapEvents(false);

	// StaticMesh の設定後でないとスケールが変わらない
	const float DefaultScale = 5;
	SetActorScale3D(FVector(DefaultScale, DefaultScale, DefaultScale));



	// ===== Collision ===== //
	const float RingRadius = 90.f;
	const FVector BoxScale(0.55f, 0.45f, 1.8f);
	for (int k = 0; k < VertNum; k++)
	{
		FString CompStr = FString::Printf(TEXT("Collider%d"), k);
		FName CompName(*CompStr);
		UBoxComponent* BoxComp = CreateDefaultSubobject<UBoxComponent>(CompName);
		BoxComp->SetupAttachment(RootComponent);
		BoxComp->SetVisibility(false);

		// Collision
		BoxComp->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		BoxComp->SetCollisionProfileName(TEXT("BlockAllDynamic"));

		// Physics
		BoxComp->SetSimulatePhysics(false);

		// Transform
		float Angle = (360.f / VertNum) * k;
		float Sin;
		float Cos;
		FMath::SinCos(&Sin, &Cos, FMath::DegreesToRadians(Angle));
		float LocX = RingRadius * Cos;
		float LocY = RingRadius * Sin;
		BoxComp->SetRelativeLocation(FVector(0.f, LocX, LocY));
		BoxComp->SetRelativeRotation(FRotator(0.f, 0.f, -Angle));
		BoxComp->SetRelativeScale3D(BoxScale);

		ColComps[k] = BoxComp;
	}



	// ===== Pass Check ===== //
	PassCheckComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pass Check"));
	PassCheckComp->SetupAttachment(RootComponent);
	PassCheckComp->SetVisibility(false);
	PassCheckComp->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
	PassCheckComp->SetRelativeScale3D(FVector(1.f, 1.f, 0.1f));

	// Mesh
	const TCHAR CylinderMeshPath[] = TEXT("/Engine/BasicShapes/Cylinder.Cylinder");
	UStaticMesh* CylinderMesh = LoadObject<UStaticMesh>(nullptr, CylinderMeshPath);
	PassCheckComp->SetStaticMesh(CylinderMesh);

	// Collision
	PassCheckComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PassCheckComp->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	PassCheckComp->SetGenerateOverlapEvents(true);
	PassCheckComp->OnComponentBeginOverlap.AddDynamic(this, &ARing::OnOverlapBegin);

	// Physics
	PassCheckComp->SetSimulatePhysics(false);
}



void ARing::BeginPlay()
{
	Super::BeginPlay();
}



void ARing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsPassed)
	{
		// 時間計測 & カーブのサンプル
		AnimTimer += DeltaTime;
		float TimeRatio = AnimTimer / AnimDuration;
		TimeRatio = FMath::Clamp(TimeRatio, 0.f, 1.f);
		float CurveVal = AnimCurve->GetFloatValue(TimeRatio);	// 0 -> 1

		// リングをスピン
		float DeltaAngle = RotateSpeed * DeltaTime;
		AddActorLocalRotation(FRotator(0.f, 0.f, DeltaAngle));

		// リングを縮小
		float S = StartScale * (1 - CurveVal);
		SetActorScale3D(FVector(S,S,S));

		// ライダーを追尾
		FVector DiffPos = PassedRider->GetActorLocation() - GetActorLocation();
		FVector DeltaPos = DiffPos * CurveVal;
		AddActorWorldOffset(DeltaPos);
	}
}



// Overlap Events //////////////////////////////////////////////////////////////////////////////
void ARing::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherbodyIndex,
	bool bFromSweep,
	const FHitResult& Hit
)
{
	if (bIsPassed)
		return;
	if (!OtherActor->ActorHasTag(ARider::RIDER_TAG))
		return;

	PassedRider = dynamic_cast<ARider*>(OtherActor);
	if (PassedRider == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Passed actor was not Rider"));
		return;
	}

	bIsPassed = true;

	// リング生成時にスケールが徐々に大きくなる演出があるので、BeginPlayでなく、ここでリングのスケールを取得
	StartScale = GetActorScale().X;

	// ライダーが吹っ飛んでしまうので、コライダーはOFF
	for (int k = 0; k < VertNum; k++)
	{
		ColComps[k]->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}


