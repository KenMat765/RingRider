// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Level/HexBase.h"
#include "HexBend.generated.h"

/**
 * 
 */
UCLASS()
class RINGRIDER_API AHexBend : public AHexBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHexBend();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
