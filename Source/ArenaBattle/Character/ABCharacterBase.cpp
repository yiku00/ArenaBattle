// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ABCharacterBase.h"
#include <Components/CapsuleComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include "ABCharacterControlData.h"
#include "Animation/AnimMontage.h"
#include "ABComboActionData.h"
#include "Physics/ABCollision.h"
#include "Engine/DamageEvents.h"
#include "CharacterStat/ABCharacterStatComponent.h"
#include "UI/ABWidgetComponent.h"
#include "UI/ABHPBarWidget.h"
#include "Item/ABItems.h"

DEFINE_LOG_CATEGORY(LogABCharacter);

// Sets default values
AABCharacterBase::AABCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;
	
	//pawn
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	//capsule component
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);
	GetCapsuleComponent()->SetCollisionProfileName(CPROFILE_ABCAPUSLE);

	//movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	//mesh
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -100.f), FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard"));
	if (CharacterMeshRef.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceRef(TEXT("/Game/ArenaBattle/Animation/ABP_ABCharacter1.ABP_ABCharacter1_C"));
	if (AnimInstanceRef.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceRef.Class);
	}

	static ConstructorHelpers::FObjectFinder<UABCharacterControlData> ShoulderDataRef(TEXT("/Game/ArenaBattle/CharacterControl/ABC_Shoulder"));
	if (ShoulderDataRef.Succeeded())
	{
		CharacterControlManager.Add(ECharacterControlType::Shoulder, ShoulderDataRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UABCharacterControlData> QuaterDataRef(TEXT("/Game/ArenaBattle/CharacterControl/ABC_Quater"));
	if (QuaterDataRef.Succeeded())
	{
		CharacterControlManager.Add(ECharacterControlType::Quater, QuaterDataRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> UAnimMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/ArenaBattle/Animation/AM_ComboAttack.AM_ComboAttack'"));
	if (UAnimMontageRef.Succeeded())
	{
		ComboActionMontage = UAnimMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UABComboActionData> UABComboActionDataRef(TEXT("/Script/ArenaBattle.ABComboActionData'/Game/ArenaBattle/CharacterAction/ABA_Combo_Attack.ABA_Combo_Attack'"));
	if (UABComboActionDataRef.Succeeded())
	{
		ComboActionData = UABComboActionDataRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> DeadMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/ArenaBattle/Animation/AM_Dead.AM_Dead'"));
	if (DeadMontageRef.Succeeded())
	{
		DeadMontage = DeadMontageRef.Object;
	}

	//StatComponent
	Stat = CreateDefaultSubobject<UABCharacterStatComponent>(TEXT("Stat"));

	//Widget Component
	HpBar = CreateDefaultSubobject<UABWidgetComponent>(TEXT("Widget"));
	HpBar->SetupAttachment(GetMesh());
	HpBar->SetRelativeLocation(FVector(0.f, 0.f, 180.f));

	static ConstructorHelpers::FClassFinder<UUserWidget> HpBarWidgetRef(TEXT("/Game/ArenaBattle/UI/WBP_HPBar.WBP_HPBar_C"));
	if (HpBarWidgetRef.Class)
	{
		HpBar->SetWidgetClass(HpBarWidgetRef.Class);
		HpBar->SetWidgetSpace(EWidgetSpace::Screen);
		HpBar->SetDrawSize(FVector2D(150.0f, 15.0f));
		HpBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &AABCharacterBase::EquipWeapon)));
	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &AABCharacterBase::DrinkPotion)));
	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &AABCharacterBase::ReadScroll)));

	// Weapon Component
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));
}

void AABCharacterBase::SetCharacterControlData(const UABCharacterControlData* CharacterControlData)
{
	//pawn
	bUseControllerRotationYaw = CharacterControlData->bUseControllerRotationYaw;
	
	//character movement
	GetCharacterMovement()->bOrientRotationToMovement = CharacterControlData->bOrientRotationToMovement;
	GetCharacterMovement()->bUseControllerDesiredRotation = CharacterControlData->bUseControllerDesiredRotation;
	GetCharacterMovement()->RotationRate = CharacterControlData->RotationRate;


}

void AABCharacterBase::ComboActionBegin()
{
	CurrentComboCnt = 1;

	//Movement Setting
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	const float AttackSpeedRate = 1.f;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(ComboActionMontage, AttackSpeedRate);
	
	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &AABCharacterBase::ComboActionEnd);
	AnimInstance->Montage_SetEndDelegate(EndDelegate,ComboActionMontage);

	ComboTimerHandle.Invalidate();
	SetComboCheckTimer();
}

void AABCharacterBase::ComboActionEnd(UAnimMontage* TargetMontage, bool IsProperlyEnded)
{
	ensure(CurrentComboCnt != 0);
	CurrentComboCnt = 0;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	NotifyComboActionEnd();
}

void AABCharacterBase::NotifyComboActionEnd()
{

}

void AABCharacterBase::SetComboCheckTimer()
{
	int32 ComboIdx = CurrentComboCnt - 1;

	ensure(ComboActionData->EffectiveFrameCount.IsValidIndex(ComboIdx));

	const float AttackSpeedRate = Stat->GetTotalStat().AttackSpeed;
	float ComboEffectiveTime = (ComboActionData->EffectiveFrameCount[ComboIdx] / ComboActionData->FrameRate) / AttackSpeedRate;
	if (ComboEffectiveTime > 0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &AABCharacterBase::ComboCheck,ComboEffectiveTime,false);
	}

}

void AABCharacterBase::ComboCheck()
{
	ComboTimerHandle.Invalidate();
	if (HasNextComboCommand)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		CurrentComboCnt = FMath::Clamp(CurrentComboCnt + 1, 1, ComboActionData->MaxComboCount);
		FName NextSection = *FString::Printf(TEXT("%s%d"), *ComboActionData->MontageSectionNamePrefix, CurrentComboCnt);
		AnimInstance->Montage_JumpToSection(NextSection, ComboActionMontage);
		SetComboCheckTimer();
		HasNextComboCommand = false;
	}
}

void AABCharacterBase::ProcessComboCommand()
{
	if (CurrentComboCnt == 0)
	{
		ComboActionBegin();
		return;
	}
	else if (!ComboTimerHandle.IsValid())
	{
		HasNextComboCommand = false;
	}
	else {
		HasNextComboCommand = true;
	}
}

void AABCharacterBase::AttackHitCheck()
{
	FHitResult OutHitResult;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), true, this);

	const float AttackRange = Stat->GetTotalStat().AttackRange;
	const float AttackRadius = Stat->GetAttackRadius();
	const float AttackDamage = Stat->GetTotalStat().Attack;
	const FVector Start = GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector End = Start + GetActorForwardVector() * AttackRange;

	bool HitDetected = GetWorld()->SweepSingleByChannel(OutHitResult, Start, End, FQuat::Identity, CCHANEL_ABACTION, FCollisionShape::MakeSphere(AttackRadius), Params);
	if (HitDetected)
	{
		FDamageEvent DamageEvent;
		OutHitResult.GetActor()->TakeDamage(AttackDamage, DamageEvent, GetController(), this);
	}

#if ENABLE_DRAW_DEBUG
	FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
	float CapsuleHalfHeight = AttackRange * 0.5f;
	FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;

	DrawDebugCapsule(GetWorld(), 
		CapsuleOrigin, 
		CapsuleHalfHeight,
		AttackRadius, 
		FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), 
		DrawColor, 
		false,
		5.f);

#endif
}

float AABCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstogator, AActor* OhterActor)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstogator, OhterActor);
	//SetDead();
	Stat->ApplyDamage(DamageAmount);
	return DamageAmount;
}

void AABCharacterBase::SetDead()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	PlayDeadAnimation();
	//PlayAnimMontage(DeadMontage);
	SetActorEnableCollision(false);
	HpBar->SetHiddenInGame(true);
}

void AABCharacterBase::PlayDeadAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->StopAllMontages(0.0f);
	AnimInstance->Montage_Play(DeadMontage, 1.0f);
}

void AABCharacterBase::SetupCharacterWidget(UABUserWidget* InUserWidget)
{
	UABHPBarWidget* HpBarWidtet = Cast<UABHPBarWidget>(InUserWidget);
	if (HpBarWidtet)
	{
		HpBarWidtet->UpdateStat(Stat->GetBaseStat(), Stat->GetModifierStat());
		HpBarWidtet->UpdateBar(Stat->GetCurrentHp());
		Stat->OnStatChanged.AddUObject(HpBarWidtet, &UABHPBarWidget::UpdateStat);
		Stat->OnHpChanged.AddUObject(HpBarWidtet, &UABHPBarWidget::UpdateBar);
	}
}

void AABCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	Stat->OnHpZero.AddUObject(this, &AABCharacterBase::SetDead);
	Stat->OnStatChanged.AddUObject(this, &AABCharacterBase::ApplyStat);
}

void AABCharacterBase::TakeItem(UABItemData* InItemData)
{
	if (InItemData)
	{
		TakeItemActions[(uint8)InItemData->Type].ItemDegate.ExecuteIfBound(InItemData);
	}
}

void AABCharacterBase::DrinkPotion(UABItemData* InItemData)
{
	UE_LOG(LogABCharacter, Log, TEXT("Drink Potion"));
	UABPotionItemData* PotionItemData = Cast<UABPotionItemData>(InItemData);
	if (PotionItemData)
	{
		Stat->HealHp(PotionItemData->HealAmount);
	}

}

void AABCharacterBase::EquipWeapon(UABItemData* InItemData)
{
	UE_LOG(LogABCharacter, Log, TEXT("Equip Weapon"));
	UABWeaponItemData* WeaponItemData = Cast<UABWeaponItemData>(InItemData);
	if (WeaponItemData)
	{
		if (WeaponItemData->WeaponMesh.IsPending())
		{
			WeaponItemData->WeaponMesh.LoadSynchronous();
		}
		Weapon->SetSkeletalMesh(WeaponItemData->WeaponMesh.Get());
		Stat->SetModifierStat(WeaponItemData->ModifierStat);
	}
}

void AABCharacterBase::ReadScroll(UABItemData* InItemData)
{
	UABScrolItemlData* ScrollItemData = Cast<UABScrolItemlData>(InItemData);
	if (ScrollItemData)
	{
		Stat->AddBaseStat(ScrollItemData->BaseStat);
	}
	UE_LOG(LogABCharacter, Log, TEXT("Read Scroll"));
}

int32 AABCharacterBase::GetLevel()
{
	return Stat->GetCurrentLevel();
}

void AABCharacterBase::SetLevel(int32 NewLevel)
{
	Stat->SetLevelStat(NewLevel);
}

void AABCharacterBase::ApplyStat(const FABCharacterStat& BaseStat, const FABCharacterStat& ModifierStat)
{
	const float MovementSpeed = (BaseStat + ModifierStat).MovementSpeed;
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
}
