// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/ABItemData.h"
#include "GameData/ABCharacterStat.h"
#include "ABScrolItemlData.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABScrolItemlData : public UABItemData
{
	GENERATED_BODY()
public:
	UABScrolItemlData();

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("ABItemData", GetFName());
	};

	UPROPERTY(EditAnyWhere, Category = Stat)
	FABCharacterStat BaseStat;
	
};
