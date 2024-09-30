// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/Ring.h"
#include "Components/BoxComponent.h"


ARing::ARing()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;



	// ===== Actor Settings ===== //
	Tags.Add(FName("Bounce"));



	// ===== Ring Static Mesh ===== //
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ring Mesh"));
	RootComponent = MeshComp;
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

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



	// ===== Collision ===== //
	const float Radius = 90.f;
	const FVector Scale(0.55f, 0.45f, 1.8f);
	for (int k = 0; k < VertNum; k++)
	{
		FString CompStr = FString::Printf(TEXT("Collider%d"), k);
		FName CompName(*CompStr);
		UBoxComponent* BoxComp = CreateDefaultSubobject<UBoxComponent>(CompName);
		BoxComp->SetupAttachment(RootComponent);
		BoxComp->SetVisibility(false);
		BoxComp->SetSimulatePhysics(false);
		BoxComp->SetCollisionProfileName(TEXT("BlockAllDynamic"));

		float Angle = (360.f / VertNum) * k;
		float Sin;
		float Cos;
		FMath::SinCos(&Sin, &Cos, FMath::DegreesToRadians(Angle));
		float LocX = Radius * Cos;
		float LocY = Radius * Sin;
		BoxComp->SetRelativeLocation(FVector(0.f, LocX, LocY));
		BoxComp->SetRelativeRotation(FRotator(0.f, 0.f, -Angle));
		BoxComp->SetRelativeScale3D(Scale);

		ColComps[k] = BoxComp;
	}
}


void ARing::BeginPlay()
{
	Super::BeginPlay();
	
}


void ARing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

