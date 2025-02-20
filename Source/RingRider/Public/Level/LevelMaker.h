// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelMaker.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogLevelMaker, Log, All);

/**
* ステージ制作を補助するクラス。
* ステージ製作中にのみ使うものであり、ランタイム中に動作するものではない。
*/
UCLASS()
class RINGRIDER_API ALevelMaker : public AActor
{
	GENERATED_BODY()
	
public:	
	ALevelMaker();



	// Tiles Properties ///////////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY(EditInstanceOnly, meta = (ClampMin = "0"))
	float TileEdgeLength = 200;

	UPROPERTY(EditInstanceOnly, meta = (ToolTip = "Name of the folder to put spawned tiles in to."))
	FName FolderName;

	float TileWidth() { return TileEdgeLength * FMath::Sqrt(3); }



	// Align Tiles ///////////////////////////////////////////////////////////////////////////////////
private:
	UFUNCTION(CallInEditor, Category = "Support Functions", meta = (ToolTip = "Aligns normal hex tiles."))
	void AlignTiles();

	UPROPERTY(EditInstanceOnly, Category = "Support Functions", meta = (ClampMin = "1"))
	int AlignRadius;



	// Create LevelInstance //////////////////////////////////////////////////////////////////////////////
private:
	UFUNCTION(CallInEditor, Category = "Support Functions")
	void CreateLevelInstance();
};
