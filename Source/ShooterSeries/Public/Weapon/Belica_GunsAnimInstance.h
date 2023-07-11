// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "UObject/ObjectPtr.h"
#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Belica_GunsAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERSERIES_API UBelica_GunsAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

protected:
	void InitializeShooterCharacter();
	void InitializeWeapon();
	void InitializeHandCompTransform();
	bool IsWeaponClipMoving();

private:
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TObjectPtr<class AShooterCharacter> MShooterCharacter;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TObjectPtr<class AWeapon> MOwningWeapon;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	bool MisMovingClip{false};

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FTransform MHandClipTransform;
     /*shows the amount at which pistol clip is displaced*/
	UPROPERTY(BlueprintReadOnly,meta=(AllowPrivateAccess="true"))
	float MPistolDisplacement{0.f};

public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE AShooterCharacter* GetShooterCharacter() const { return MShooterCharacter; }

	FORCEINLINE AWeapon* GetOwningWeapon() const { return MOwningWeapon; }
	FORCEINLINE bool GetIsMovingClip() const { return MisMovingClip; }
	FORCEINLINE FTransform GetTransform() const { return MHandClipTransform; }
	FORCEINLINE void SetIsMovingClip(bool isMovingClip) { MisMovingClip = isMovingClip; }
};
