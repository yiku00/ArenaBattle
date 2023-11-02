// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ABCharacterPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ABCharacterControlData.h"

AABCharacterPlayer::AABCharacterPlayer()
{
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	static ConstructorHelpers::FObjectFinder<UInputAction> JumpActionRef(TEXT("/Game/ArenaBattle/Input/Actions/IA_Jump.IA_Jump"));
	if (JumpActionRef.Succeeded())
	{
		JumpAction = JumpActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> ChangeControlRef(TEXT("/Game/ArenaBattle/Input/Actions/IA_ChangeControl.IA_ChangeControl"));
	if (ChangeControlRef.Succeeded())
	{
		ChangeControlAction = ChangeControlRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> ShoulderMoveRef(TEXT("/Game/ArenaBattle/Input/Actions/IA_ShoulderMove.IA_ShoulderMove"));
	if (ShoulderMoveRef.Succeeded())
	{
		ShoulderMoveAction = ShoulderMoveRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> ShoulderLook(TEXT("/Game/ArenaBattle/Input/Actions/IA_ShoulderLook.IA_ShoulderLook"));
	if (ShoulderLook.Succeeded())
	{
		ShoulderLookAction = ShoulderLook.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> QuaterMoveRef(TEXT("/Game/ArenaBattle/Input/Actions/IA_QuaterMove.IA_QuaterMove"));
	if (QuaterMoveRef.Succeeded())
	{
		QuaterMoveAction = QuaterMoveRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> AttackActionRef(TEXT("/Game/ArenaBattle/Input/Actions/IA_Attack.IA_Attack"));
	if (QuaterMoveRef.Succeeded())
	{
		AttackAction = AttackActionRef.Object;
	}

	CurrentControlType = ECharacterControlType::Quater;

}

void AABCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();
	SetCharacterControl(CurrentControlType);
}

void AABCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		EnhancedInputComponent->BindAction(ChangeControlAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::ChangeCharaterControl);
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(ShoulderMoveAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::ShoulderMove);

		//Looking
		EnhancedInputComponent->BindAction(ShoulderLookAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::ShoulderLook);

		EnhancedInputComponent->BindAction(QuaterMoveAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::QuaterMove);

		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::Attack);
	}
}

void AABCharacterPlayer::SetCharacterControlData(const UABCharacterControlData* CharacterControlData)
{
	Super::SetCharacterControlData(CharacterControlData);

	CameraBoom->TargetArmLength = CharacterControlData->TargetArmLength;
	CameraBoom->SetRelativeRotation(CharacterControlData->RelativeRotation);
	CameraBoom->bUsePawnControlRotation = CharacterControlData->bUsePawnControllerRotation;
	CameraBoom->bInheritPitch = CharacterControlData->bInheritPitch;
	CameraBoom->bInheritYaw = CharacterControlData->bInheritYaw;
	CameraBoom->bInheritRoll = CharacterControlData->bInheritRoll;
	CameraBoom->bDoCollisionTest = CharacterControlData->bInheritTest;
}

void AABCharacterPlayer::ChangeCharaterControl()
{
	if (CurrentControlType == ECharacterControlType::Shoulder)
	{
		SetCharacterControl(ECharacterControlType::Quater);
	}
	else if (CurrentControlType == ECharacterControlType::Quater)
	{
		SetCharacterControl(ECharacterControlType::Shoulder);
	}
}

void AABCharacterPlayer::SetCharacterControl(ECharacterControlType NewCharacterControlType)
{
	UABCharacterControlData* NewCharacterControl = CharacterControlManager[NewCharacterControlType];
	check(NewCharacterControl);
	SetCharacterControlData(NewCharacterControl);

	APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		Subsystem->ClearAllMappings();
		UInputMappingContext* NewMappingContext = NewCharacterControl->InputMappingContext;
		if (NewMappingContext)
		{
			Subsystem->AddMappingContext(NewMappingContext, 0);
		}
	}

	CurrentControlType = NewCharacterControlType;
}

void AABCharacterPlayer::ShoulderMove(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.X);
		AddMovementInput(RightDirection, MovementVector.Y);
	}
}

void AABCharacterPlayer::QuaterMove(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	
	float MovementVectorSize = 1.f;
	float MovementVectorSizeSquared = MovementVector.SquaredLength();

	if (MovementVectorSizeSquared > 1.f)
	{
		MovementVector.Normalize();
		MovementVectorSizeSquared = 1.f;
	}
	else {
		MovementVectorSize = FMath::Sqrt(MovementVectorSizeSquared);
	}


	FVector MoveDirection = FVector(MovementVector.X, MovementVector.Y, 0.f);
	//GetController()->SetControlRotation(FRotationMatrix::MakeFromX(MoveDirection).Rotator());
	GetController()->SetControlRotation(MoveDirection.Rotation());
	AddMovementInput(MoveDirection, MovementVectorSize);
	AddMovementInput(MoveDirection, MovementVectorSize);
}

void AABCharacterPlayer::ShoulderLook(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AABCharacterPlayer::Attack(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("AttackAttackAttackAttackAttackAttack"));
	ProcessComboCommand();
}

void AABCharacterPlayer::ProcessComboCommand()
{
	Super::ProcessComboCommand();
}
