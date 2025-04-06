// Copyright Epic Games, Inc. All Rights Reserved.


#include "cppmpWeaponComponent.h"
#include "../Characters/cppmpCharacter.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Animation/AnimInstance.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UcppmpWeaponComponent::UcppmpWeaponComponent() {
	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);
}
bool UcppmpWeaponComponent::AttachWeapon(AcppmpCharacter* TargetCharacter) {
	Character = TargetCharacter;

	// Check that the character is valid, and has no weapon component yet
	if (Character == nullptr || Character->GetInstanceComponents().FindItemByClass<UcppmpWeaponComponent>()) {
		return false;
	}

	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));
	return true;
}

void UcppmpWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	// maintain the EndPlay call chain
	Super::EndPlay(EndPlayReason);
}
