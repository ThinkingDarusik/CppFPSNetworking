// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "cppmpWeaponComponent.generated.h"

class AcppmpCharacter;
class AcppmpProjectile;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CPPMP_API UcppmpWeaponComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:
	/** Sets default values for this component's properties */
	UcppmpWeaponComponent();

	/** Attaches the actor to a FirstPersonCharacter */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	bool AttachWeapon(AcppmpCharacter* TargetCharacter);

protected:
	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile, meta=(AllowPrivateAccess=true))
	TSubclassOf<AcppmpProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Projectile, meta=(AllowPrivateAccess=true))
	USoundBase* FireSound;
	
	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile, meta=(AllowPrivateAccess=true))
	UAnimMontage* FireAnimation;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Projectile, meta=(AllowPrivateAccess=true))
	FVector MuzzleOffset;

public: // Getters
	// Projectile class
	TSubclassOf<AcppmpProjectile> GetProjectileClass() { return ProjectileClass; };

	// Muzzle Offset
	FVector GetMuzzleOffset() const { return MuzzleOffset; };

	// FireSound
	USoundBase* GetFireSound() const { return FireSound; };

	// FireAnimation
	UAnimMontage* GetFireAnimation() const { return FireAnimation; };
	
protected:
	/** Ends gameplay for this component. */
	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	/** The Character holding this weapon*/
	UPROPERTY()
	AcppmpCharacter* Character;
};
