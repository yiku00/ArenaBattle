// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ABPlayerController.h"
#include "UI/ABHUDWidget.h"

AABPlayerController::AABPlayerController()
{
	static ConstructorHelpers::FClassFinder<UABHUDWidget>WidgetClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/ArenaBattle/UI/WBP_ABHUD.WBP_ABHUD_C'"));
	if (WidgetClass.Succeeded())
	{
		ABHUDWidgetClass = WidgetClass.Class;
	}

}

void AABPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);

	ABHUDWidget = CreateWidget<UABHUDWidget>(this, ABHUDWidgetClass);
	if (ABHUDWidget)
	{
		ABHUDWidget->AddToViewport();
	}
	
}
