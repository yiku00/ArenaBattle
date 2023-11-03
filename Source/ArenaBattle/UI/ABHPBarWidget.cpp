// Fill out your copyright notice in the Description page of Project Settings.


#include "ABHPBarWidget.h"
#include "Components/ProgressBar.h"
#include "Interface/ABCharacterWidgetInterface.h"

UABHPBarWidget::UABHPBarWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MaxHp = -1.f;
}

void UABHPBarWidget::UpdateBar(float NewCurrentHp)
{
	ensure(MaxHp > 0);
	if (HPProgressBar)
	{
		HPProgressBar->SetPercent(NewCurrentHp /MaxHp);
	}
}

void UABHPBarWidget::NativeConstruct()
{
	Super::NativeConstruct();
	HPProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PB_HPBar")));
	ensure(HPProgressBar != nullptr);

	IABCharacterWidgetInterface* CharacterWidget = Cast<IABCharacterWidgetInterface>(OwningActor);
	if (CharacterWidget)
	{
		CharacterWidget->SetupCharacterWidget(this);
	}
	
}
