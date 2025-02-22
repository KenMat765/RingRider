// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EnvQueryContext_FinalDestination.generated.h"

struct FEnvQueryContextData;
struct FEnvQueryInstance;

UCLASS()
class RINGRIDER_API UEnvQueryContext_FinalDestination : public UEnvQueryContext
{
	GENERATED_BODY()
	
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
};
