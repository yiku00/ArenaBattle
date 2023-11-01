// Fill out your copyright notice in the Description page of Project Settings.


#include "Prop/ABFounTain.h"
#include <Components/StaticMeshComponent.h>

// Sets default values
AABFounTain::AABFounTain()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	Water = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Water"));

	RootComponent = Body;
	Water->SetupAttachment(Body);
	Water->SetRelativeLocation(FVector(0.f,0.f,132.f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BodyMesh(TEXT("/Game/ArenaBattle/Environment/Props/SM_Plains_Castle_Fountain_01.SM_Plains_Castle_Fountain_01"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> WaterMesh(TEXT("/Game/ArenaBattle/Environment/Props/SM_Plains_Fountain_02.SM_Plains_Fountain_02"));

	if (BodyMesh.Succeeded())
	{
		Body->SetStaticMesh(BodyMesh.Object);
	}

	if (WaterMesh.Succeeded())
	{
		Water->SetStaticMesh(WaterMesh.Object);
	}
}

// Called when the game starts or when spawned
void AABFounTain::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AABFounTain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

