// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PsmComponent.generated.h"


// Indicates current condition of each state .
enum class EPsmCondition : uint8
{
	ENTER,
	STAY,
	EXIT
};


// Informations passed to TFunction of each state.
struct FPsmInfo
{
	FPsmInfo(float, EPsmCondition);

	float DeltaTime;
	EPsmCondition Condition;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RINGRIDER_API UPsmComponent : public UActorComponent
{
	GENERATED_BODY()


public:
	using TStateFunc = TFunction<void (const FPsmInfo&)>;


public:	
	// Sets default values for this component's properties
	UPsmComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	// <State Function, Is On>
	TMap<TStateFunc*, bool> StateMap;

public:
	void AddState(TStateFunc&);
	void RemoveState(TStateFunc&);
	void ClearState();

	void TurnOnState(TStateFunc&);
	void TurnOffState(TStateFunc&);
	void TurnOnAll();
	void TurnOffAll();

	bool IsStateOn(TStateFunc&) const;
};
