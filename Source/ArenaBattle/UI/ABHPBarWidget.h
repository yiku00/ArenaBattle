// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ABUserWidget.h"
#include "GameData/ABCharacterStat.h"
#include "ABHPBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABHPBarWidget : public UABUserWidget
{
	GENERATED_BODY()
public:
	UABHPBarWidget(const FObjectInitializer& ObjectInitializer);
	void UpdateBar(float NewCurrentHp);
	void UpdateStat(const FABCharacterStat& BaseStat, const FABCharacterStat& ModifierStat);
	FString GetHpStateText();
protected:
	virtual void NativeConstruct() override;

	UPROPERTY()
	TObjectPtr<class UProgressBar> HPProgressBar;

	UPROPERTY()
	TObjectPtr<class UTextBlock> HpStat;

	UPROPERTY()
	float MaxHp;

	UPROPERTY()
	float CurrentHp;
};
