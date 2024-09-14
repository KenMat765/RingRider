// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Level/HexBase.h"
#include "HexTile.generated.h"

/**
 * 
 */
UCLASS()
class RINGRIDER_API AHexTile : public AHexBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHexTile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
