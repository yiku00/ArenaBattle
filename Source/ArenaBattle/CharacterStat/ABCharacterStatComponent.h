// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ABCharacterStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHpZeroDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHpChangedDeletage,float /*CurrentHp*/);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARENABATTLE_API UABCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UABCharacterStatComponent();
	FOnHpZeroDelegate OnHpZero;
	FOnHpChangedDeletage OnHpChanged;

	FORCEINLINE float GerMaxHp() {return MaxHp;}
	FORCEINLINE float GetCurrentHp() {return CurrentHp;}
	float ApplyDamage(float InDamage);
protected:
	void SetHp(float NewHp);
	virtual void BeginPlay() override;

	UPROPERTY(VisibleInstanceOnly, Category=Stat)
	float MaxHp;

	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat)
	float CurrentHp;
public:	

		
};
