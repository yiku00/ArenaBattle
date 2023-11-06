// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ABItemBox.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Physics/ABCollision.h"
#include "Interface/ABCharacterInterface.h"
#include "Engine/AssetManager.h"
#include "ABItemData.h"

// Sets default values
AABItemBox::AABItemBox()
{
	Triger = CreateDefaultSubobject<UBoxComponent>(TEXT("TrigerBox"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Effect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Effect"));

	RootComponent = Triger;
	Mesh->SetupAttachment(Triger);
	Effect->SetupAttachment(Triger);
	Effect->SetAutoActivate(false);

	Triger->SetCollisionProfileName(CPROFILE_ABTRIGER);
	Triger->SetBoxExtent(FVector(40.f,42.f,30.f));


	static ConstructorHelpers::FObjectFinder<UStaticMesh> BoxMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/ArenaBattle/Environment/Props/SM_Env_Breakables_Box1.SM_Env_Breakables_Box1'"));
	if (BoxMeshRef.Succeeded())
	{
		Mesh->SetStaticMesh(BoxMeshRef.Object);
	}
	Mesh->SetRelativeLocation(FVector(0.f,-3.5f,-30.0f));
	Mesh->SetCollisionProfileName(TEXT("NoCollision"));

	static ConstructorHelpers::FObjectFinder<UParticleSystem> EffectRef(TEXT("/Script/Engine.ParticleSystem'/Game/ArenaBattle/Effect/P_TreasureChest_Open_Mesh.P_TreasureChest_Open_Mesh'"));
	if (EffectRef.Succeeded())
	{
		Effect->SetTemplate(EffectRef.Object);
	}

}

// Called when the game starts or when spawned
void AABItemBox::BeginPlay()
{
	Super::BeginPlay();
	
}

void AABItemBox::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult)
{
	if (nullptr == Item)
	{
		Destroy();
		return;
	}

	IABCharacterInterface* OverlappingPawn = Cast<IABCharacterInterface>(OtherActor);
	if (OverlappingPawn)
	{
		OverlappingPawn->TakeItem(Item);
	}

	Effect->SetActive(true);
	Mesh->SetHiddenInGame(true);
	SetActorEnableCollision(false);
	Effect->OnSystemFinished.AddDynamic(this, &AABItemBox::OnEffectFinished);
}

void AABItemBox::OnEffectFinished(UParticleSystemComponent* ParticleSystem)
{

}

void AABItemBox::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	UAssetManager& Manager = UAssetManager::Get();

	TArray<FPrimaryAssetId> Asset;
	Manager.GetPrimaryAssetIdList(TEXT("ABItemData"),Asset);

	ensure(0 < Asset.Num());
	int32 RandomIndex = FMath::RandRange(0, Asset.Num() - 1);
	FSoftObjectPtr AssetPtr(Manager.GetPrimaryAssetPath(Asset[RandomIndex]));
	if (AssetPtr.IsPending())
	{
		AssetPtr.LoadSynchronous();
	}
	Item = Cast<UABItemData>(AssetPtr.Get());
	ensure(Item);

	Triger->OnComponentBeginOverlap.AddDynamic(this, &AABItemBox::OnOverlapBegin);
}

