// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/HexBend.h"

// Sets default values
AHexBend::AHexBend()
{
	if (Tile)
	{
		const TCHAR TileMeshPath[] = TEXT("/Game/Levels/Parts/HexBend");
		UStaticMesh* TileMesh = LoadObject<UStaticMesh>(nullptr, TileMeshPath);
		Tile->SetStaticMesh(TileMesh);
	}
}

// Called when the game starts or when spawned
void AHexBend::BeginPlay()
{
	Super::BeginPlay();
	
}
