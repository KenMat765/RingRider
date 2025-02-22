// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Rotatable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class URotatable : public UInterface
{
	GENERATED_BODY()
};

/**
 * オブジェクトの回転を扱うインターフェース
 */
class RINGRIDER_API IRotatable
{
	GENERATED_BODY()

public:
	virtual bool CanRotate() const = 0;
	virtual void SetCanRotate(bool _CanRotate) = 0;

	virtual FRotator GetRotation() const = 0;
	virtual void SetRotation(const FRotator& _NewRotator) = 0;
};
