// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ABCharacterInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UABCharacterInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ARENABATTLE_API IABCharacterInterface
{
	GENERATED_BODY()
public:
	virtual void TakeItem(class UABItemData* InItemData) = 0;
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
};
