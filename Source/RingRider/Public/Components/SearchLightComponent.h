// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SearchLightComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RINGRIDER_API USearchLightComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	USearchLightComponent();

protected:
	virtual void BeginPlay() override;

public:
	/**
	*   / +Angle
	* C ---------
	*	\ -Angle
	*/
	TArray<AActor*> SearchActors(
		float Radius,
		float Angle,
		TArray<FName> TargetTags = TArray<FName>(),
		TArray<ECollisionChannel> CollisionChannels = TArray<ECollisionChannel>(),
		UClass* ClassFilter = nullptr,
		TArray<AActor*> ActorsToIgnore = TArray<AActor*>()
	);
};
