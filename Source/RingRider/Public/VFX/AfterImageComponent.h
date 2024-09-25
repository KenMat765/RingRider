// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "AfterImageComponent.generated.h"


class AAfterImageActor;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RINGRIDER_API UAfterImageComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	UAfterImageComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	// Mesh //////////////////////////////////////////////////////////////////////////////////////////////
private:
	TArray<UStaticMesh*> Meshes;

public:
	void AddMesh(UStaticMesh*);
	void RemoveMesh(UStaticMesh*);


	// Material //////////////////////////////////////////////////////////////////////////////////////////
private:
	FLinearColor ImageBaseColor;
	float ImageMetallic;
	float ImageRoughness;
	float ImageOpacity;

public:
	void SetMaterialParams(FLinearColor BaseColor, float Metallic, float Roughness, float Opacity);


	// Effect ////////////////////////////////////////////////////////////////////////////////////////////
private:
	bool bIsPlaying;
	float Lifetime;
	float Interval;
	float SpawnTimer;
	TArray<AAfterImageActor*> ImagesInScene;

public:
	void SetLifetime(float NewLifetime);
	void SetInterval(float NewInterval);
	void PlayEffect();
	void StopEffect();


	// Utilities /////////////////////////////////////////////////////////////////////////////////////////
private:
	AAfterImageActor* SpawnAfterImageActor();
};
