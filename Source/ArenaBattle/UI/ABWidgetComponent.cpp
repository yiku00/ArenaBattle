// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ABWidgetComponent.h"
#include "ABUserWidget.h"

void UABWidgetComponent::InitWidget()
{
	Super::InitWidget();
	UABUserWidget* ABUSerWidget = Cast<UABUserWidget>(GetWidget());
	if (ABUSerWidget)
	{
		ABUSerWidget->SetOwningActor(GetOwner());
	}
}
