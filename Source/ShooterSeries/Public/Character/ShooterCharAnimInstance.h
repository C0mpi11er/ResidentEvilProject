// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "UObject/ObjectPtr.h"
#include "CoreMinimal.h"
#include "ShooterCharacter.h"
#include "Animation/AnimInstance.h"
#include "ShooterSeries/WeaponType.h"
#include "ShooterCharAnimInstance.generated.h"

/**
 * 
 */

enum class EWeaponType:uint8;

UENUM(BlueprintType)
enum class EAimOffSet:uint8
{
	EAOS_Aiming UMETA(DsiplayName="Aiming"),
	EAOS_Hip UMETA(DsiplayName="Hip"),
	EAOS_Reloading UMETA(DsiplayName="Reloading"),
	EAOS_InAir UMETA(DsiplayName="InAir"),
	EAOS_Max UMETA(DsiplayName="Max")
};

UCLASS()
class SHOOTERSERIES_API UShooterCharAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	/*function performs similar task to begin play*/
	virtual void NativeInitializeAnimation() override;
	/*function keeps track of  shooter char equipped weapons type*/
	void InitEquppiedWeaponType();
	/*function updates animation variable in runtime*/
	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperty(float DeltaTime);

	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

	/*init aim offset*/
	void InitializeAimOffset();
	/*init character aim offsetyaw*/
	void InitializeYaw();
	/*init character aim offset pitch*/
	void InitializePitch();
	/*init the isReloading variable*/
	void InitializeIsReloading();

	void InitializeCrouching();

	


	/*handles the character turn in place*/
	void TurnInPlace();

	/*handles character lean when running*/
	void Lean(float DeltaTime);
	/*sets recoil weight base on turning,crouching and reloading*/
	void InitAndSetRecoilWeight();

protected:
	virtual void NativeBeginPlay() override;


private:
	/*object reference to the shooter character*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement, meta=(AllowPrivateAccess))
	TObjectPtr<class AShooterCharacter> MShooterCharacter;

	/*bool check if shooter char is in the air */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement, meta=(AllowPrivateAccess))
	bool MbIsInAir{false};

	/*bool check if shooter char is accelerating*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement, meta=(AllowPrivateAccess))
	bool MbIsAccelerating{false};

	/*shooter char GroundSpeed*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement, meta=(AllowPrivateAccess))
	float MGroundSpeed{};
	/*character Movement Yaw*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Movement, meta=(AllowPrivateAccess))
	float MMovementOffsetYaw;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Movement, meta=(AllowPrivateAccess))
	float MLastMovOffsetYaw;

	UPROPERTY(BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess))
	bool MisWeaponAimed{false};


	//turn in place variables

	/*yaw of character present frame*/
	float MTipCurrentCharacterYaw{0.f};
	/*yaw of character last frame*/
	float MTipCharacterYawLastFrame{0.f};
	/*root bone yaw*/
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	float MRootBoneYawOffset{0.f};

	/*rotation curve for current frame*/
	float MRotationCurve{0.f};
	/*rotation Curve for last frame*/
	float MRotationCurveLastFrame{0.f};
	/*<turn in place variables/>*/

	/*the pitch of shooter char*/
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	float MPitch{0.f};

	/*the yaw of shooter char(controller yaw)*/
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	float MYaw{0.f};
	/*a runtime check if shooter char is reloading*/
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	bool MisReloading{false};

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	EAimOffSet MAimOffSet{EAimOffSet::EAOS_Hip};
	/*the delta of character yaw used for leaning*/
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	float MLeanCharDeltaYaw{0.f};

	FRotator MLeanCharYaw{0.f};

	FRotator MLeanCharYawLastFrame{0.f};

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	bool MIsEquipping{false};

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	bool MCrouching{false};

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	bool MisTurning{false};

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	float MRecoilWeight{1.f};

	/*montage played at level start*/
	UPROPERTY(EditAnywhere, Category=Combat)
	TObjectPtr<UAnimMontage> MLevelStartMontage;

	/**
	 * @brief  keeps track of equipped weapon type
	 */
	UPROPERTY(BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess))
	EWeaponType MEquippedWeaponType{EWeaponType::EWT_SubMachineGun};

	UPROPERTY(BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess))
	bool MShouldUseFABRIk{false};

	

public:
	EAimOffSet GetAimOffSet() const { return MAimOffSet; }
	void SetAimOffSet(EAimOffSet AimOffSet) { MAimOffSet = AimOffSet; }
	FORCEINLINE void SetCrouching(bool IsCrouching) { MCrouching = IsCrouching; }
	
};
