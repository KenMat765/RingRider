// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelMaker.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogLevelMaker, Log, All);

/**
* �X�e�[�W�����⏕����N���X�B
* �X�e�[�W���쒆�ɂ̂ݎg�����̂ł���A�����^�C�����ɓ��삷����̂ł͂Ȃ��B
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

	float TileWidth() { return TileEdgeLength * FMath::Sqrt(3); }



	// Align Tiles ///////////////////////////////////////////////////////////////////////////////////
private:
	UFUNCTION(CallInEditor, Category = "Support Functions|Align Tiles")
	void AlignTiles();

	UPROPERTY(EditInstanceOnly, Category = "Support Functions|Align Tiles", meta = (ClampMin = "1"))
	int Radius;
};
