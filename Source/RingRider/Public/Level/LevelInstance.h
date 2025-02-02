// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameInfo.h"
#include "LevelInstance.generated.h"

UCLASS()
class RINGRIDER_API ALevelInstance : public AActor
{
	GENERATED_BODY()
	
public:	
	ALevelInstance();

	UPROPERTY(EditAnywhere)
	bool bIsSelectable = true;
	virtual bool IsSelectable() const override { return true; }



// Components /////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(VisibleInstanceOnly)
	UInstancedStaticMeshComponent* InstStaticMeshComp;



// Level Edit Functions ///////////////////////////////////////////////////////////////////////////////////////////
public:
	void AddTile(FTransform Trans, ETeam Team = ETeam::Team_None)
	{
		int32 TileId = InstStaticMeshComp->AddInstance(Trans);
		SetTileTeam(TileId, Team);
	}



// Tile Properties ////////////////////////////////////////////////////////////////////////////////////////////////
private:
	static const FLinearColor BASE_COLOR_NONE_1;
	static const FLinearColor BASE_COLOR_NONE_2;
	static const FLinearColor BASE_COLOR_NONE_3;
	static const FLinearColor BASE_COLOR_TEAM_1;
	static const FLinearColor BASE_COLOR_TEAM_2;
	static const FLinearColor BASE_COLOR_STATIC;

	static const FLinearColor LIGHT_COLOR_NONE;
	static const FLinearColor LIGHT_COLOR_1;
	static const FLinearColor LIGHT_COLOR_2;
	static const FLinearColor LIGHT_COLOR_STATIC;

	static const float LOW_EMISSION;
	static const float HIGH_EMISSION;

public:
	static const FLinearColor GetRandomBaseColor()
	{
		int RandomInt = FMath::RandRange(1, 3);
		switch (RandomInt)
		{
		case 1:  return BASE_COLOR_NONE_1;
		case 2:  return BASE_COLOR_NONE_2;
		case 3:  return BASE_COLOR_NONE_3;
		default: return BASE_COLOR_NONE_1;
		}
	}



// Tile Edit Fuctions /////////////////////////////////////////////////////////////////////////////////////////////
public:
	void SetTileTeam(int32 TileId, ETeam NewTeam);

private:
	void SetTileColor(int32 TileId, FLinearColor BaseColor, FLinearColor LightColor);
	void SetTileBaseColor(int32 TileId, FLinearColor Color);
	void SetTileLightColor(int32 TileId, FLinearColor Color);

	void SetTileEmission(int32 TileId, float BaseEmission, float LightEmission);
	void SetTileBaseEmission(int32 TileId, float Emission);
	void SetTileLightEmission(int32 TileId, float Emission);

private:
	// Custom data of each tile instance.
	static const int32 CUSTOM_DATA_ID_BASEMAT_R = 0;
	static const int32 CUSTOM_DATA_ID_BASEMAT_G = 1;
	static const int32 CUSTOM_DATA_ID_BASEMAT_B = 2;

	static const int32 CUSTOM_DATA_ID_LIGHTMAT_R = 3;
	static const int32 CUSTOM_DATA_ID_LIGHTMAT_G = 4;
	static const int32 CUSTOM_DATA_ID_LIGHTMAT_B = 5;

	static const int32 CUSTOM_DATA_ID_BASEMAT_EMIT = 6;
	static const int32 CUSTOM_DATA_ID_LIGHTMAT_EMIT = 7;
};
