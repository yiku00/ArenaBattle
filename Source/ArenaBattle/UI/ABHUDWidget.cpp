// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ABHUDWidget.h"
#include "UI/ABHPBarWidget.h"
#include "UI/ABCharacterStatWidget.h"
#include "Interface/ABCharacterHUDInterface.h"

void UABHUDWidget::UpdateStat(const FABCharacterStat& BaseStat, const FABCharacterStat& ModifierStat)
{
	FABCharacterStat TotalStat = BaseStat + ModifierStat;
	HpBar->SetMaxHp(TotalStat.MaxHp);

	CharacterStat->UpdateStat(BaseStat, ModifierStat);


}

void UABHUDWidget::UpdateHpBar(float NewCurrentHp)
{
	HpBar->UpdateBar(NewCurrentHp);
	UE_LOG(LogTemp, Warning, TEXT("%f"), NewCurrentHp);
}

void UABHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HpBar = Cast<UABHPBarWidget>(GetWidgetFromName(TEXT("WidgetHpBar")));
	ensure(HpBar);

	CharacterStat = Cast<UABCharacterStatWidget>(GetWidgetFromName(TEXT("WidgetCharacterStat")));
	ensure(CharacterStat);

	IABCharacterHUDInterface* HUDPawn = Cast<IABCharacterHUDInterface>(GetOwningPlayerPawn());
	if (HUDPawn)
	{
		HUDPawn->SetupHUDWidget(this);
	}
}
