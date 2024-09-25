// Fill out your copyright notice in the Description page of Project Settings.


#include "VFX/AfterImageActor.h"


const TCHAR AAfterImageActor::IMAGE_MATERIAL_PATH[] = TEXT("/Game/VFX/AfterImage/Material_AfterImage");

const FName AAfterImageActor::IMAGE_BASE_COLOR = FName("Base Color");
const FName AAfterImageActor::IMAGE_METALLIC = FName("Metallic");
const FName AAfterImageActor::IMAGE_ROUGHNESS = FName("Roughness");
const FName AAfterImageActor::IMAGE_OPACITY = FName("Opacity");


AAfterImageActor::AAfterImageActor():
	Timer(0.f)
{
 	// �c���̃R���g���[����AfterImageComponent����s�� (�����̗�����ꂩ���ɂ܂Ƃ߂邽��)
	PrimaryActorTick.bCanEverTick = false;

	// RootComponent�̍쐬
	USceneComponent* RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = RootComp;
}

void AAfterImageActor::BeginPlay()
{
	Super::BeginPlay();

	// �}�e���A�����擾 (Create��BeginPlay�ł��K�v������)
	UMaterial* Material = LoadObject<UMaterial>(nullptr, IMAGE_MATERIAL_PATH);
	MaterialInstance = UMaterialInstanceDynamic::Create(Material, this);
}


void AAfterImageActor::AddMesh(UStaticMesh* Mesh)
{
	UStaticMeshComponent* MeshComp = NewObject<UStaticMeshComponent>(this);
	MeshComp->SetStaticMesh(Mesh);
	for (int k = 0; k < MeshComp->GetNumMaterials(); k++)
	{
		MeshComp->SetMaterial(k, MaterialInstance);
	}
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetSimulatePhysics(false);
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->RegisterComponent();
}


void AAfterImageActor::SetMaterialColor(FLinearColor Color)
{
	MaterialInstance->SetVectorParameterValue(IMAGE_BASE_COLOR, Color);
}

void AAfterImageActor::SetMaterialMetallic(float Metallic)
{
	MaterialInstance->SetScalarParameterValue(IMAGE_METALLIC, Metallic);
}

void AAfterImageActor::SetMaterialRoughness(float Roughness)
{
	MaterialInstance->SetScalarParameterValue(IMAGE_ROUGHNESS, Roughness);
}

void AAfterImageActor::SetMaterialOpacity(float Opacity)
{
	MaterialInstance->SetScalarParameterValue(IMAGE_OPACITY, Opacity);
}

