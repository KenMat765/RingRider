// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/LevelInstance.h"
#include "Components/InstancedStaticMeshComponent.h"

DEFINE_LOG_CATEGORY(LogLevelInstance);

ALevelInstance::ALevelInstance()
{
	PrimaryActorTick.bCanEverTick = false;

	Tags.Add(TAG_GROUND);
	Tags.Add(TAG_HEXTILE);

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

	InstStaticMeshComp->SetCollisionProfileName(TEXT("Level"));
}

void ALevelInstance::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = PropertyChangedEvent.GetPropertyName();

	if (bUpdateTileAppearanceImmediately)
	{
		if (PropertyName == GET_MEMBER_NAME_CHECKED(ALevelInstance, BaseColorNone1)		||
			PropertyName == GET_MEMBER_NAME_CHECKED(ALevelInstance, BaseColorNone2)		||
			PropertyName == GET_MEMBER_NAME_CHECKED(ALevelInstance, BaseColorNone3)		||
			PropertyName == GET_MEMBER_NAME_CHECKED(ALevelInstance, LightColorNone)		||
			PropertyName == GET_MEMBER_NAME_CHECKED(ALevelInstance, BaseColorTeam1)		||
			PropertyName == GET_MEMBER_NAME_CHECKED(ALevelInstance, LightColorTeam1)	||
			PropertyName == GET_MEMBER_NAME_CHECKED(ALevelInstance, BaseColorTeam2)		||
			PropertyName == GET_MEMBER_NAME_CHECKED(ALevelInstance, LightColorTeam2)	||
			PropertyName == GET_MEMBER_NAME_CHECKED(ALevelInstance, BaseColorStatic)	||
			PropertyName == GET_MEMBER_NAME_CHECKED(ALevelInstance, LightColorStatic)	||
			PropertyName == GET_MEMBER_NAME_CHECKED(ALevelInstance, LowEmission)		||
			PropertyName == GET_MEMBER_NAME_CHECKED(ALevelInstance, HighEmission))
		{
			for (int i = 0; i < InstStaticMeshComp->GetInstanceCount(); i++)
			{
				UpdateTileAppearance(i);
			}
		}
	}
}



// Tile Properties ////////////////////////////////////////////////////////////////////////////////////////////////
FTileProperty ALevelInstance::GetTileProperty(int32 TileId)
{
	if (!InstanceIdInRange(TileId))
	{
		UE_LOG(LogLevelInstance, Error, TEXT("TileID %d was out of range."), TileId);
		return FTileProperty();
	}

	FTileProperty* TileProperty = TileProperties.Find(TileId);
	if (!TileProperty)
	{
		UE_LOG(LogLevelInstance, Error, TEXT("TileID %d was not found in TileProperties."), TileId);
		return FTileProperty();
	}

	return *TileProperty;
}

void ALevelInstance::SetTileProperty(int32 TileId, FTileProperty TileProperty)
{
	if (!InstanceIdInRange(TileId))
	{
		UE_LOG(LogLevelInstance, Error, TEXT("TileID %d was out of range."), TileId);
		return;
	}
	TileProperties.Emplace(TileId, TileProperty);
	UpdateTileAppearance(TileId);
}

void ALevelInstance::ChangeTileTeam(int32 TileId, ETeam NewTeam)
{
	if (!InstanceIdInRange(TileId))
	{
		UE_LOG(LogLevelInstance, Error, TEXT("TileID %d was out of range."), TileId);
		return;
	}

	FTileProperty* TileProperty = TileProperties.Find(TileId);
	if (!TileProperty)
	{
		UE_LOG(LogLevelInstance, Error, TEXT("TileID %d was not found in TileProperties."), TileId);
		return;
	}

	TileProperty->Team = NewTeam;
	SetTileProperty(TileId, *TileProperty);
}

void ALevelInstance::ChangeTileStatic(int32 TileId, bool bNewStatic)
{
	if (!InstanceIdInRange(TileId))
	{
		UE_LOG(LogLevelInstance, Error, TEXT("TileID %d was out of range."), TileId);
		return;
	}

	FTileProperty* TileProperty = TileProperties.Find(TileId);
	if (!TileProperty)
	{
		UE_LOG(LogLevelInstance, Error, TEXT("TileID %d was not found in TileProperties."), TileId);
		return;
	}

	TileProperty->bIsStatic = bNewStatic;
	SetTileProperty(TileId, *TileProperty);
}

void ALevelInstance::UpdateTileAppearance(int32 TileId)
{
	if (!InstanceIdInRange(TileId))
	{
		UE_LOG(LogLevelInstance, Error, TEXT("TileID %d was out of range."), TileId);
		return;
	}

	if (!TileProperties.Find(TileId))
	{
		UE_LOG(LogLevelInstance, Error, TEXT("TileID %d was not found in TileProperties."), TileId);
		return;
	}

	if (TileProperties[TileId].bIsStatic)
	{
		SetTileColor(TileId, BaseColorStatic, LightColorStatic);
		SetTileEmission(TileId, LowEmission, HighEmission);
	}
	else
	{
		switch (TileProperties[TileId].Team)
		{
		case ETeam::Team_None :
		{
			SetTileColor(TileId, GetRandomBaseColor(), LightColorNone);
			SetTileEmission(TileId, LowEmission, LowEmission);
		} break;

		case ETeam::Team_1 :
		{
			SetTileColor(TileId, BaseColorTeam1, LightColorTeam1);
			SetTileEmission(TileId, HighEmission, HighEmission);
		} break;

		case ETeam::Team_2 :
		{
			SetTileColor(TileId, BaseColorTeam2, LightColorTeam2);
			SetTileEmission(TileId, HighEmission, HighEmission);
		} break;
		}
	}
}



// Tile Edit Fuctions /////////////////////////////////////////////////////////////////////////////////////////////
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

