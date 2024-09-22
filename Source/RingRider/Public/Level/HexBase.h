// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameInfo.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "HexBase.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FTeamActionDelegate, ETeam);

UCLASS(abstract)
class RINGRIDER_API AHexBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHexBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when property was modified in editor
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;




	// Team of Tile //////////////////////////////////////////////////////////////////////////////////
	/**
	* Static tile: Team does not change throughout the game.
	* Static tile is permenantly static, and dynamic tile is permanently dynamic.
	*/
protected:
	UPROPERTY(EditInstanceOnly, Category = "Tile Property")
	bool bIsStatic;

	UPROPERTY(EditInstanceOnly, Category = "Tile Property", meta = (EditCondition="!bIsStatic"))
	ETeam Team;

public:
	bool IsStatic() const;
	// Setter for bIsStatic is not necessary, because it should only be modified in editor.
	ETeam GetTeam() const;
	void SetTeam(ETeam);

protected:
	FTeamActionDelegate OnTeamChangedAction;
	void TriggerOnTeamChangedAction(ETeam);

public:
	void AddOnTeamChangedAction(TFunction<void(ETeam)>);

private:
	void OnTeamChangedActionBase(ETeam);




	// Neighbour Tiles ///////////////////////////////////////////////////////////////////////////////
protected:
	AHexBase* Neighbours[6];

public:
	AHexBase** GetNeighbours();



	// Components ////////////////////////////////////////////////////////////////////////////////////
protected:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Tile;



	// Materials /////////////////////////////////////////////////////////////////////////////////////
protected:
	UMaterialInstanceDynamic* BaseMaterialInstance;
	UMaterialInstanceDynamic* LightMaterialInstance;

	// Name of material parameters.
	static const FName MATERIAL_PARAM_COLOR;
	static const FName MATERIAL_PARAM_STRENGTH;
	static const FName MATERIAL_PARAM_OPACITY;

	// Parameter values.
	static const FLinearColor DEFAULT_BASE_COLOR;
	static const FLinearColor DEFAULT_LIGHT_COLOR;
	static const FLinearColor LIGHT_COLOR_STATIC;
	static const FLinearColor LIGHT_COLOR_1;	// Color of Team1
	static const FLinearColor LIGHT_COLOR_2;	// Color of Team2

	static const float LOW_EMISSION;
	static const float HIGH_EMISSION;

	static const float DEFAULT_OPACITY;
	static const float SLIP_THROUGH_OPACITY;

	void SetMaterialColor(
		FLinearColor BaseColor, float BaseEmission,
		FLinearColor LightColor, float LightEmission);

public:
	// This method is necessary, because the transparency of the tiles are different for each player.
	void SetMaterialOpacity(float Opacity);
};