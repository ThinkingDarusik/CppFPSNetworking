// Copyright Epic Games, Inc. All Rights Reserved.

#include "cppmpCharacter.h"
#include "../Weapons/cppmpProjectile.h"
#include "../Weapons/cppmpWeaponComponent.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

#define DEBUG false

//////////////////////////////////////////////////////////////////////////
// AcppmpCharacter

AcppmpCharacter::AcppmpCharacter() {
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));
}

//////////////////////////////////////////////////////////////////////////// Input

void AcppmpCharacter::NotifyControllerChanged() {
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller)) {
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) {
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AcppmpCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AcppmpCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AcppmpCharacter::Look);

		// Fire
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &AcppmpCharacter::Fire);
	} else {
		UE_LOG(LogTemplateCharacter, Error,
		       TEXT(
			       "'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."
		       ), *GetNameSafe(this));
	}
}


void AcppmpCharacter::Move(const FInputActionValue& Value) {
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr) {
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AcppmpCharacter::Look(const FInputActionValue& Value) {
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr) {
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AcppmpCharacter::Fire_Implementation() {

	if (DEBUG) {
		UE_LOG(LogTemp, Display, TEXT("%s"), TEXT("Fire"));
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("Fire"));
	}

	if (GetController() == nullptr) {
		return;
	}

	UcppmpWeaponComponent* WeaponComponent = FindComponentByClass<UcppmpWeaponComponent>();

	if (WeaponComponent == nullptr) {
		return;
	}

	TSubclassOf<class AcppmpProjectile> ProjectileClass = WeaponComponent->GetProjectileClass();

	// Try and fire a projectile
	if (ProjectileClass != nullptr) {
		UWorld* const World = GetWorld();
		if (World != nullptr) {
			APlayerController* PlayerController = Cast<APlayerController>(GetController());
			const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = GetActorLocation() + SpawnRotation.
				RotateVector(WeaponComponent->GetMuzzleOffset());

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride =
				ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			// Spawn the projectile at the muzzle
			World->SpawnActor<AcppmpProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		}
	}

	PlayEffects();
}

void AcppmpCharacter::PlayEffects_Implementation() {
	if (GetController() == nullptr) {
		return;
	}

	UcppmpWeaponComponent* WeaponComponent = FindComponentByClass<UcppmpWeaponComponent>();

	if (WeaponComponent == nullptr) {
		return;
	}

	USoundBase* FireSound = WeaponComponent->GetFireSound();
	UAnimMontage* FireAnimation = WeaponComponent->GetFireAnimation();

	// Try and play the sound if specified
	if (FireSound != nullptr) {
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// Try and play a firing animation if specified
	if (FireAnimation != nullptr) {
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr) {
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}
