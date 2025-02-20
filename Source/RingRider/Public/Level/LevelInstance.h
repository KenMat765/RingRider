// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameInfo.h"
#include "LevelInstance.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogLevelInstance, Log, All);

struct FTileProperty
{
	FTileProperty()
		: Team(ETeam::Team_None), bIsStatic(false) {}

	FTileProperty(ETeam _Team, bool _bIsStatic)
		: Team(_Team), bIsStatic(_bIsStatic) {}

	ETeam Team;
	bool bIsStatic;
};

UCLASS()
class RINGRIDER_API ALevelInstance : public AActor
{
	GENERATED_BODY()
	
public:	
	ALevelInstance();

	UPROPERTY(EditAnywhere, Category = LevelInstance)
	bool bIsSelectable = true;
	virtual bool IsSelectable() const override { return true; }

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif



// Components /////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(VisibleInstanceOnly)
	UInstancedStaticMeshComponent* InstStaticMeshComp;

	bool InstanceIdInRange(int32 Id)
	{
		bool bIdInRange = 0 <= Id && Id < InstStaticMeshComp->GetInstanceCount();
		return bIdInRange;
	}



// Level Edit Functions ///////////////////////////////////////////////////////////////////////////////////////////
public:
	void AddTile(FTransform Trans, ETeam Team = ETeam::Team_None, bool bIsStatic = false)
	{
		int32 TileId = InstStaticMeshComp->AddInstance(Trans);
		SetTileProperty(TileId, FTileProperty(Team, bIsStatic));
	}



// Tile Properties ////////////////////////////////////////////////////////////////////////////////////////////////
public:
	FTileProperty GetTileProperty(int32 TileId);
	void SetTileProperty(int32 TileId, FTileProperty TileProperty);
	void ChangeTileTeam(int32 TileId, ETeam Team);
	void ChangeTileStatic(int32 TileId, bool bSetStatic);

private:
	TMap<int32, FTileProperty> TileProperties;

	UPROPERTY(EditInstanceOnly, Category = LevelInstance)
	FLinearColor BaseColorNone1 = FLinearColor(0.010f, 0.010f, 0.010f);

	UPROPERTY(EditInstanceOnly, Category = LevelInstance)
	FLinearColor BaseColorNone2 = FLinearColor(0.0125f, 0.0125f, 0.0125f);

	UPROPERTY(EditInstanceOnly, Category = LevelInstance)
	FLinearColor BaseColorNone3 = FLinearColor(0.015f, 0.015f, 0.015f);

	UPROPERTY(EditInstanceOnly, Category = LevelInstance)
	FLinearColor BaseColorTeam1 = FLinearColor(1.f, 0.f, 0.85f);

	UPROPERTY(EditInstanceOnly, Category = LevelInstance)
	FLinearColor BaseColorTeam2 = FLinearColor(0.f, 1.f, 0.08f);

	UPROPERTY(EditInstanceOnly, Category = LevelInstance)
	FLinearColor BaseColorStatic = FLinearColor(0.1f, 0.1f, 0.1f);

	UPROPERTY(EditInstanceOnly, Category = LevelInstance)
	FLinearColor LightColorNone = FLinearColor(0.015f, 0.015f, 0.015f);

	UPROPERTY(EditInstanceOnly, Category = LevelInstance)
	FLinearColor LightColorTeam1 = FLinearColor(1.f, 0.f, 0.85f);

	UPROPERTY(EditInstanceOnly, Category = LevelInstance)
	FLinearColor LightColorTeam2 = FLinearColor(0.f, 1.f, 0.08f);

	UPROPERTY(EditInstanceOnly, Category = LevelInstance)
	FLinearColor LightColorStatic = FLinearColor(1.f, 1.f, 1.f);

	UPROPERTY(EditInstanceOnly, Category = LevelInstance)
	float LowEmission = 1.f;

	UPROPERTY(EditInstanceOnly, Category = LevelInstance)
	float HighEmission = 8.f;

	FLinearColor GetRandomBaseColor()
	{
		int RandomInt = FMath::RandRange(1, 3);
		switch (RandomInt)
		{
		case 1:  return BaseColorNone1;
		case 2:  return BaseColorNone2;
		case 3:  return BaseColorNone3;
		default: return BaseColorNone1;
		}
	}

	void UpdateTileAppearance(int32 TileId);

	UPROPERTY(EditInstanceOnly, Category = LevelInstance)
	bool bUpdateTileAppearanceImmediately = false;

	UFUNCTION(CallInEditor, Category = LevelInstance)
	void UpdateAllTileAppearance()
	{
		for (int i = 0; i < InstStaticMeshComp->GetInstanceCount(); i++)
		{
			UpdateTileAppearance(i);
		}
	}



// Tile Edit Fuctions /////////////////////////////////////////////////////////////////////////////////////////////
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
