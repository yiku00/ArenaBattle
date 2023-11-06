// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ABCharacterBase.h"
#include "Engine/StreamableManager.h"
#include "ABCharacterNonPlayer.generated.h"

/**
 * 
 */
UCLASS(config=ArenaBattle)
class ARENABATTLE_API AABCharacterNonPlayer : public AABCharacterBase
{
	GENERATED_BODY()

public:
	AABCharacterNonPlayer();

protected:
	virtual void SetDead() override;
	virtual void PostInitializeComponents() override;
	void NPCMeshLoadComplete();

	UPROPERTY(config)
	TArray<FSoftObjectPath> NPCMeshes;

	TSharedPtr <FStreamableHandle> NPCMeshHandle;
private:
	const float DeadEventDelayTime = 5.f;
};
