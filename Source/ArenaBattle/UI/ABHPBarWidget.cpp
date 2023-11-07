// Fill out your copyright notice in the Description page of Project Settings.


#include "ABHPBarWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Interface/ABCharacterWidgetInterface.h"

UABHPBarWidget::UABHPBarWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MaxHp = -1.f;
}

void UABHPBarWidget::UpdateBar(float NewCurrentHp)
{
	CurrentHp = NewCurrentHp;
	ensure(MaxHp > 0);
	if (HPProgressBar)
	{
		HPProgressBar->SetPercent(NewCurrentHp /MaxHp);
	}

	if (HpStat)
	{
		HpStat->SetText(FText::FromString(GetHpStateText()));
	}
}

void UABHPBarWidget::UpdateStat(const FABCharacterStat& BaseStat, const FABCharacterStat& ModifierStat)
{
	MaxHp = (BaseStat + ModifierStat).MaxHp;
	if (HPProgressBar)
	{
		HPProgressBar->SetPercent(CurrentHp / MaxHp);
	}

	if (HpStat)
	{
		HpStat->SetText(FText::FromString(GetHpStateText()));
	}
}

FString UABHPBarWidget::GetHpStateText()
{
	return FString::Printf(TEXT("%.0f/%.0f"),CurrentHp,MaxHp);
}

void UABHPBarWidget::NativeConstruct()
{
	Super::NativeConstruct();
	HPProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PB_HPBar")));
	ensure(HPProgressBar != nullptr);

	HpStat = Cast<UTextBlock>(GetWidgetFromName(TEXT("TxtHpStat")));
	ensure(HpStat != nullptr);

	IABCharacterWidgetInterface* CharacterWidget = Cast<IABCharacterWidgetInterface>(OwningActor);
	if (CharacterWidget)
	{
		CharacterWidget->SetupCharacterWidget(this);
	}
	
}
