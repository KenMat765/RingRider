// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/HexTile.h"

// Sets default values
AHexTile::AHexTile()
{
	if (Tile)
	{
		const TCHAR TileMeshPath[] = TEXT("/Game/Levels/Parts/HexTile");
		UStaticMesh* TileMesh = LoadObject<UStaticMesh>(nullptr, TileMeshPath);
		Tile->SetStaticMesh(TileMesh);
	}
}

// Called when the game starts or when spawned
void AHexTile::BeginPlay()
{
	Super::BeginPlay();
	
}
