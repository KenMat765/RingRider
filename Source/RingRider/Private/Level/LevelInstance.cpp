// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/LevelInstance.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "TagList.h"



ALevelInstance::ALevelInstance()
{
	PrimaryActorTick.bCanEverTick = false;

	Tags.Add(FTagList::TAG_GROUND);
	Tags.Add(FTagList::TAG_HEXTILE);

	// ===== Instance Static Mesh Component ===== //
	InstStaticMeshComp = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Instanced Static Mesh"));
	RootComponent = InstStaticMeshComp;
	InstStaticMeshComp->SetMobility(EComponentMobility::Static);

	// Custom Data
	InstStaticMeshComp->NumCustomDataFloats = 8;

	// Mesh
	const TCHAR TileMeshPath[] = TEXT("/Game/Levels/Parts/HexTile");
	UStaticMesh* TileMesh = LoadObject<UStaticMesh>(nullptr, TileMeshPath);
	InstStaticMeshComp->SetStaticMesh(TileMesh);

	InstStaticMeshComp->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
}



// Tile Edit Fuctions /////////////////////////////////////////////////////////////////////////////////////////////
const FLinearColor ALevelInstance::BASE_COLOR_NONE		= FLinearColor(0.1f,  0.1f,  0.1f);
const FLinearColor ALevelInstance::BASE_COLOR_1			= FLinearColor(1.f,   0.f,   0.85f);
const FLinearColor ALevelInstance::BASE_COLOR_2			= FLinearColor(0.f,   1.f,   0.08f);
const FLinearColor ALevelInstance::BASE_COLOR_STATIC	= FLinearColor(0.1f,  0.1f,  0.1f);

const FLinearColor ALevelInstance::LIGHT_COLOR_NONE		= FLinearColor(0.05f, 0.05f, 0.05f);
const FLinearColor ALevelInstance::LIGHT_COLOR_1		= FLinearColor(1.f,   0.f,   0.85f);
const FLinearColor ALevelInstance::LIGHT_COLOR_2		= FLinearColor(0.f,   1.f,   0.08f);
const FLinearColor ALevelInstance::LIGHT_COLOR_STATIC	= FLinearColor(1.f,   1.f,   1.f);

const float ALevelInstance::LOW_EMISSION				= 1.f;
const float ALevelInstance::HIGH_EMISSION				= 10.f;



// Tile Edit Fuctions /////////////////////////////////////////////////////////////////////////////////////////////
void ALevelInstance::SetTileTeam(int32 TileId, ETeam NewTeam)
{
	switch (NewTeam)
	{
	case ETeam::Team_None :
	{
		SetTileColor(TileId, BASE_COLOR_NONE, LIGHT_COLOR_NONE);
		SetTileEmission(TileId, LOW_EMISSION, LOW_EMISSION);
	} break;

	case ETeam::Team_1 :
	{
		SetTileColor(TileId, BASE_COLOR_1, LIGHT_COLOR_1);
		SetTileEmission(TileId, HIGH_EMISSION, HIGH_EMISSION);
	} break;

	case ETeam::Team_2 :
	{
		SetTileColor(TileId, BASE_COLOR_2, LIGHT_COLOR_2);
		SetTileEmission(TileId, HIGH_EMISSION, HIGH_EMISSION);
	} break;
	}
}

inline void ALevelInstance::SetTileColor(int32 TileId, FLinearColor BaseColor, FLinearColor LightColor)
{
	SetTileBaseColor(TileId, BaseColor);
	SetTileLightColor(TileId, LightColor);
}

inline void ALevelInstance::SetTileBaseColor(int32 TileId, FLinearColor Color)
{
	InstStaticMeshComp->SetCustomDataValue(TileId, CUSTOM_DATA_ID_BASEMAT_R, Color.R, true);
	InstStaticMeshComp->SetCustomDataValue(TileId, CUSTOM_DATA_ID_BASEMAT_G, Color.G, true);
	InstStaticMeshComp->SetCustomDataValue(TileId, CUSTOM_DATA_ID_BASEMAT_B, Color.B, true);
}

inline void ALevelInstance::SetTileLightColor(int32 TileId, FLinearColor Color)
{
	InstStaticMeshComp->SetCustomDataValue(TileId, CUSTOM_DATA_ID_LIGHTMAT_R, Color.R, true);
	InstStaticMeshComp->SetCustomDataValue(TileId, CUSTOM_DATA_ID_LIGHTMAT_G, Color.G, true);
	InstStaticMeshComp->SetCustomDataValue(TileId, CUSTOM_DATA_ID_LIGHTMAT_B, Color.B, true);
}

inline void ALevelInstance::SetTileEmission(int32 TileId, float BaseEmission, float LightEmission)
{
	SetTileBaseEmission(TileId, BaseEmission);
	SetTileLightEmission(TileId, LightEmission);
}

inline void ALevelInstance::SetTileBaseEmission(int32 TileId, float Emission)
{
	InstStaticMeshComp->SetCustomDataValue(TileId, CUSTOM_DATA_ID_BASEMAT_EMIT, Emission, true);
}

inline void ALevelInstance::SetTileLightEmission(int32 TileId, float Emission)
{
	InstStaticMeshComp->SetCustomDataValue(TileId, CUSTOM_DATA_ID_LIGHTMAT_EMIT, Emission, true);
}

