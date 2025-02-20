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
		// ”–‚¢‚Ì‚Å‘¾‚­‚·‚é
		Tile->SetRelativeScale3D(FVector(1.f, 1.f, 3.f));
	}
}

// Called when the game starts or when spawned
void AHexTile::BeginPlay()
{
	Super::BeginPlay();
	
}
