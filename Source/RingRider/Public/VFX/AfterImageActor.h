// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AfterImageActor.generated.h"

UCLASS()
class RINGRIDER_API AAfterImageActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AAfterImageActor();

protected:
	virtual void BeginPlay() override;

public:
	float Timer;

public:
	void AddMesh(UStaticMesh*);

private:
	static const TCHAR IMAGE_MATERIAL_PATH[];
	UMaterialInstanceDynamic* MaterialInstance;

private:
	static const FName IMAGE_BASE_COLOR;
	static const FName IMAGE_METALLIC;
	static const FName IMAGE_ROUGHNESS;
	static const FName IMAGE_OPACITY;

public:
	void SetMaterialColor(FLinearColor Color);
	void SetMaterialMetallic(float Metallic);
	void SetMaterialRoughness(float Roughness);
	void SetMaterialOpacity(float Opacity);
};
