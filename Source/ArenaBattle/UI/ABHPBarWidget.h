// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ABUserWidget.h"
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

	FORCEINLINE void SetMaxHp(float NewMaxHp) { MaxHp = NewMaxHp; }
	void UpdateBar(float NewCurrentHp);
protected:
	virtual void NativeConstruct() override;

	UPROPERTY()
	TObjectPtr<class UProgressBar> HPProgressBar;

	UPROPERTY()
	float MaxHp;

};
