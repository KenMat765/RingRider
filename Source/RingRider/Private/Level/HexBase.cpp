// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/HexBase.h"
#include "TagList.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"


AHexBase::AHexBase()
{
	PrimaryActorTick.bCanEverTick = false;

	Tags.Add(FTagList::TAG_HEXTILE);
	Tags.Add(FTagList::TAG_GROUND);

	Team = ETeam::Team_None;



	// Material /////////////////////////////////////////////////////////////////////////////////////////////
	Tile = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tile Mesh"));
	RootComponent = Tile;

	const TCHAR BaseMaterialPath[] = TEXT("/Game/Levels/Parts/HexMaterial_Base");
	UMaterial* BaseMaterial = LoadObject<UMaterial>(nullptr, BaseMaterialPath);
	Tile->SetMaterial(0, BaseMaterial);

	const TCHAR LightMaterialPath[] = TEXT("/Game/Levels/Parts/HexMaterial_Light");
	UMaterial* LightMaterial = LoadObject<UMaterial>(nullptr, LightMaterialPath);
	Tile->SetMaterial(1, LightMaterial);



	// VFX ///////////////////////////////////////////////////////////////////////////////////////////////////
	ParticleRiseComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Particle Rise Effect"));
	ParticleRiseComp->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> ParticleRiseSystem(TEXT("/Game/Levels/Parts/NS_ParticleRise"));
	if (ParticleRiseSystem.Succeeded())
	{
		ParticleRiseComp->SetAsset(ParticleRiseSystem.Object);
	}
}



void AHexBase::BeginPlay()
{
	Super::BeginPlay();

	ParticleRiseComp->Deactivate();


	// ===== Material Settings ===== //
	UMaterialInterface* BaseMaterial = Tile->GetMaterial(0);
	if (BaseMaterial)
	{
		BaseMaterialInstance = UMaterialInstanceDynamic::Create(BaseMaterial, this);
		if (BaseMaterialInstance)
		{
			Tile->SetMaterial(0, BaseMaterialInstance);
		}
	}

	UMaterialInterface* LightMaterial = Tile->GetMaterial(1);
	if (LightMaterial)
	{
		LightMaterialInstance = UMaterialInstanceDynamic::Create(LightMaterial, this);
		if (LightMaterialInstance)
		{
			Tile->SetMaterial(1, LightMaterialInstance);
		}
	}
}



void AHexBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	/**
	* Material parameter can not be changed in editor, because UMaterialInstanceDynamic is created in BeginPlay.
    FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
    if (PropertyName == GET_MEMBER_NAME_CHECKED(AHexBase, Team))
    {
		if (!bIsStatic)
		{
			TriggerOnTeamChangedAction(Team);
		}
    }
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(AHexBase, bIsStatic))
	{
		if (bIsStatic)
		{
			SetMaterialColor(DEFAULT_BASE_COLOR, LOW_EMISSION, DEFAULT_LIGHT_COLOR, HIGH_EMISSION);
		}
		else
		{
			TriggerOnTeamChangedAction(Team);
		}
	}
	*/
}



// Team of Tile //////////////////////////////////////////////////////////////////////////////////
bool AHexBase::IsStatic() const { return bIsStatic; }
ETeam AHexBase::GetTeam() const { return Team; }

void AHexBase::SetTeam(ETeam NewTeam)
{
	if (bIsStatic)
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to change team of static tiles!!"));
		return;
	}
	Team = NewTeam;
	OnTeamChanged(NewTeam);
	TriggerOnTeamChangedAction(NewTeam);
}

void AHexBase::TriggerOnTeamChangedAction(ETeam NewTeam)
{
	if (OnTeamChangedAction.IsBound())
	{
		OnTeamChangedAction.Broadcast(NewTeam);
	}
}

void AHexBase::AddOnTeamChangedAction(TFunction<void(ETeam)>NewFunc)
{
	auto NewAction = FTeamActionDelegate::FDelegate::CreateLambda(NewFunc);
	OnTeamChangedAction.Add(NewAction);
}

/**
* On team changed:
*	1. マテリアルカラーを変更
*	2. Niagaraエフェクト再生
*/
void AHexBase::OnTeamChanged(ETeam NewTeam)
{
	// 1. マテリアルカラーを変更
	switch (NewTeam)
	{
	case ETeam::Team_None:
		SetMaterialParams(DEFAULT_BASE_COLOR, LOW_EMISSION, DEFAULT_LIGHT_COLOR, LOW_EMISSION);
		break;

	case ETeam::Team_1:
		SetMaterialParams(LIGHT_COLOR_1, HIGH_EMISSION, LIGHT_COLOR_1, HIGH_EMISSION);
		break;

	case ETeam::Team_2:
		SetMaterialParams(LIGHT_COLOR_2, HIGH_EMISSION, LIGHT_COLOR_2, HIGH_EMISSION);
		break;
	}

	// 2. Niagaraエフェクト再生
	ParticleRiseComp->Activate();
}



// Neighbour Tiles ///////////////////////////////////////////////////////////////////////////////
AHexBase** AHexBase::GetNeighbours() { return Neighbours; }



// Materials /////////////////////////////////////////////////////////////////////////////////////
const FName AHexBase::MATERIAL_PARAM_COLOR = FName("Emissive Color");
const FName AHexBase::MATERIAL_PARAM_STRENGTH = FName("Emissive Strength");

// Parameter values.
const FLinearColor AHexBase::DEFAULT_BASE_COLOR = FLinearColor(0.1f, 0.1f, 0.1f);
const FLinearColor AHexBase::DEFAULT_LIGHT_COLOR = FLinearColor(0.05f, 0.05f, 0.05f);
const FLinearColor AHexBase::LIGHT_COLOR_STATIC = FLinearColor(1.f, 1.f, 1.f);
const FLinearColor AHexBase::LIGHT_COLOR_1 = FLinearColor(1.f, 0.f, 0.85f);
const FLinearColor AHexBase::LIGHT_COLOR_2 = FLinearColor(0.f, 1.f, 0.08f);

const float AHexBase::LOW_EMISSION = 1.f;
const float AHexBase::HIGH_EMISSION = 20.f;
const float AHexBase::SUPER_HIGH_EMISSION = 1000.f;

void AHexBase::SetMaterialParams(
	FLinearColor BaseColor, float BaseEmission,
	FLinearColor LightColor, float LightEmission)
{
	BaseMaterialInstance->SetVectorParameterValue(MATERIAL_PARAM_COLOR, BaseColor);
	BaseMaterialInstance->SetScalarParameterValue(MATERIAL_PARAM_STRENGTH, BaseEmission);
	LightMaterialInstance->SetVectorParameterValue(MATERIAL_PARAM_COLOR, LightColor);
	LightMaterialInstance->SetScalarParameterValue(MATERIAL_PARAM_STRENGTH, LightEmission);
}

void AHexBase::SetMaterialColor(FLinearColor BaseColor, FLinearColor LightColor)
{
	BaseMaterialInstance->SetVectorParameterValue(MATERIAL_PARAM_COLOR, BaseColor);
	LightMaterialInstance->SetVectorParameterValue(MATERIAL_PARAM_COLOR, LightColor);
}

void AHexBase::SetMaterialEmission(float BaseEmission, float LightEmission)
{
	BaseMaterialInstance->SetScalarParameterValue(MATERIAL_PARAM_STRENGTH, BaseEmission);
	LightMaterialInstance->SetScalarParameterValue(MATERIAL_PARAM_STRENGTH, LightEmission);
}

