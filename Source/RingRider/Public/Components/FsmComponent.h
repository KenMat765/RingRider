// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FsmComponent.generated.h"


enum class EFsmCondition : uint8 {ENTER, STAY, EXIT};


struct FFsmInfo
{
	FFsmInfo(float, EFsmCondition);

	float DeltaTime;
	EFsmCondition Condition;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RINGRIDER_API UFsmComponent : public UActorComponent
{
	GENERATED_BODY()


public:	
	UFsmComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


public:
	using TFsmStateFunc = TFunction<void (const FFsmInfo&)>;
	void AddState(TFsmStateFunc& _FsmStateFunc);
	void RemoveState(TFsmStateFunc& _FsmStateFunc);
	bool SwitchState(TFsmStateFunc& _FsmStateFunc);
	TFsmStateFunc* GetCurrentState() const { return CurrentState; };


private:
	TArray<TFsmStateFunc*> RegisteredStates;
	TFsmStateFunc* CurrentState;
};
