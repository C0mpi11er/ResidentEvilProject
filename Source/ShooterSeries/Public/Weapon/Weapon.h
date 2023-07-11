// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Item.h"
#include "ShooterSeries/AmmoType.h"
#include "Engine/DataTable.h"
#include "ShooterSeries/WeaponType.h"
#include "Weapon.generated.h"

/**
 * 
 */
enum class EAmmoType :uint8;


USTRUCT(BlueprintType)
struct FWeaponDataTable : public FTableRowBase
{
	GENERATED_BODY()

	/**
	 * @brief type of ammo the weapon uses
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EAmmoType MAmmoType;

	/**
	 * @brief amount of bullet the weapon has
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MAmmoAmt;


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MMagazineCapacity;

	/**
	 * @brief skeletal mesh of the weapon
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMesh> MWeaponMesh;

	/**
	 * @brief pickup sound for the weapon
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USoundBase> MPickUpSound;

	/**
	 * @brief equip sound for the weapon
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USoundBase> MEquipSound;


	/**
	 * @brief weapon icon reflecting on inventory
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UTexture2D> MWeaponIcon;

	/**
	 * @brief Ammo type icon reflecting on inventory
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UTexture2D> MAmmoIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString MWeaponName;

	/**
	 * @brief weapon type that reflects on data table
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EWeaponType MWeaponType;

	/**
	 * @brief material instance of weapon data table to enable flexible change
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> MWeaponMaterialInstance;

	/**
	 * @brief material element index of weapon
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MMaterialIndex;

	/**
	 * @brief  magazine bone name on weapon mesh
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName MMagazineClipName;

	/**
	 * @brief reload section name of each  weapon
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName MReloadSectionName;

	/**
	 * @brief animation blue print for each individual weapon
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UAnimInstance> MWeaponBluePrint;

	/*Cross hair combination for each weapon*/
	UPROPERTY(EditAnywhere, Category=Combat)
	TObjectPtr<UTexture2D> CrossHairMiddle;
	UPROPERTY(EditAnywhere, Category=Combat)
	TObjectPtr<UTexture2D> CrossHairLeft;
	UPROPERTY(EditAnywhere, Category=Combat)
	TObjectPtr<UTexture2D> CrossHairRight;
	UPROPERTY(EditAnywhere, Category=Combat)
	TObjectPtr<UTexture2D> CrossHairUp;
	UPROPERTY(EditAnywhere, Category=Combat)
	TObjectPtr<UTexture2D> CrossHairDown;

	/*weapon sound fo each individual weapon*/
	UPROPERTY(EditAnywhere, Category=Combat)
	TObjectPtr<USoundBase> WeaponSound;
	UPROPERTY(EditAnywhere,Category=Combat)
	TObjectPtr<USoundBase>WeaponBulletFlyby;
	/*the muzzle flash for each individual weapon*/
	UPROPERTY(EditAnywhere, Category=Combat)
	TObjectPtr<UParticleSystem> MuzzleFlash;
	/*the fire rate of each individual weapon*/
	UPROPERTY(EditAnywhere, Category=Combat)
	float WeaponFireRate;

	UPROPERTY(EditAnywhere, Category=Combat)
	FName BoneToHide;

	UPROPERTY(EditAnywhere,Category=Combat)
	bool IsAutomaticFire;
	
	UPROPERTY(EditAnywhere,Category=Combat)
	float DamageAmount;

	UPROPERTY(EditAnywhere,Category=Combat)
	float HeadShotDamageAmount;

	
};

UCLASS()
class SHOOTERSERIES_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	virtual void OnConstruction(const FTransform& Transform) override;


	AWeapon();
	//function called by shooter char to throw weapon away when dropping
	void ThrowWeapon();
	//called to decrease Weapon ammunition
	void DecreaseAmmo();
	/**
	 * 
	 * @param Ammo is added to weapon current ammo
	 */
	void ReloadWeapon(int32 Ammo);

	virtual void Tick(float DeltaSeconds) override;

	/*return true if ammo in magazine is full*/
	bool MagazineClipFull() const;

	/*called when pistol slide is starting*/
	void StartPistolSlideTimer();

protected:
	//call back function when weapon fall timer elapsed
	//sets weapon falling back to false and itemState to pickup
	UFUNCTION()
	void StopFalling();
	/*function updates the pistol slide in tick*/
	void UpdatePistolSlideDisplacement();

	/*call back for StartPistolSlideTimer*/
	UFUNCTION()
	void EndPistolSlideTimer();

	virtual void BeginPlay() override;

private:
	//checks if weapon is in air
	bool MisWeaponFalling{false};
	//a count down timer for to when weapon should reach the ground
	float MWeaponFallTime{0.7f};
	// weapon throw timer handle
	FTimerHandle MWeaponFallTimerHandle;
	//ammo count for weapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"), Category=WeaponProperties)
	int32 MWeaponAmmoAmt{30};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category=WeaponProperties)
	EWeaponType MWeaponType{EWeaponType::EWT_SubMachineGun};

	/**
	 * @brief MAmmoType the current ammo type weapon is using
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category=WeaponProperties)
	EAmmoType MAmmoType{EAmmoType::EAT_9mmAmmo};

	UPROPERTY(EditAnywhere)
	FName MReloadMontageSection{"ReloadSMG"};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category=WeaponProperties)
	int32 MMagazineCapacity{30};
	//true when clip is moving
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category=WeaponProperties)
	bool MisMovingClip{false};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess), Category=WeaponRarity)
	TObjectPtr<UDataTable> MWeaponDataTable;

	UPROPERTY(VisibleAnywhere, Category=WeaponMaterial, BlueprintReadOnly, meta=(AllowPrivateAccess))
	int32 MPreviousMaterialIndex;

	//clip bone name on equipped  weapon mesh
	UPROPERTY(VisibleAnywhere, Category=Combat)
	FName MWeaponClipBoneName{"smg_clip"};

	/*Weapon Cross Hair Textures*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess))
	TObjectPtr<UTexture2D> MCrossHairMiddle;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess))
	TObjectPtr<UTexture2D> MCrossHairLeft;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess))
	TObjectPtr<UTexture2D> MCrossHairRight;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess))
	TObjectPtr<UTexture2D> MCrossHairUp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess))
	TObjectPtr<UTexture2D> MCrossHairDown;
	/*</>*/

	/**
	 * @brief the fire rate of  weapon
	 */
	UPROPERTY(VisibleAnywhere, Category=Combat)
	float MAutoFireRate;

	/**
	 * @brief the muzzle flash particle system for weapon
	 */
	UPROPERTY(VisibleAnywhere, Category=Combat)
	TObjectPtr<UParticleSystem> MWeaponMuzzleFlash;

	/**
	 * @brief the sound weapon makes when fired
	 */
	UPROPERTY(VisibleAnywhere, Category=Combat)
	TObjectPtr<USoundBase> MGunFireSound;

	UPROPERTY(VisibleAnywhere, Category=Combat)
	FName MBoneToHide;
	/*the float value that handles pistol cocking displacement*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess))
	float MPistolDisplacementValue{0.f};
	/*the curve value that handles pistol cocking displacement*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess))
	TObjectPtr<UCurveFloat> MPistolCurve;
	/*keeps check of the pistol slide displacement*/
	UPROPERTY()
	FTimerHandle MPistolSlideTimerHandle;
	/*keeps check if pistol slide is displaced or not when firing*/
	UPROPERTY(BlueprintReadOnly,meta=(AllowPrivateAccess))
	bool MisPistolSlideDisplaced{false};
	/*the time window for pistol slide displacement*/
	UPROPERTY()
	float MPistolSlideTime{0.1};

	UPROPERTY(EditAnywhere,BlueprintReadOnly,meta=(AllowPrivateAccess))
	float MPistolMaxDisplacement{1.f};

	UPROPERTY(VisibleAnywhere)
	bool MisAutomaticFire{false};
	
	UPROPERTY(EditAnywhere,Category=Combat)
	TObjectPtr<USoundBase>MWeaponBulletFlyby;
    /*amount of damage weapon causes*/
	UPROPERTY(VisibleAnywhere,Category=Combat)
	float MWeaponDamageAmount{0.f};
     /*amount of damage weapon causes when bullet hits head*/
	UPROPERTY(VisibleAnywhere,Category=Combat)
	float MWeaponHeadShotDamageAmount{0.f};

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UMaterialInterface>MBulletDecal;

public:
	UFUNCTION()
	int32 GetMWeaponAmmo() const { return MWeaponAmmoAmt; }

	void SetWeaponType(EWeaponType WeaponType) { MWeaponType = WeaponType; }
	EWeaponType GetWeaponType() const { return MWeaponType; }

	void SetAmmoType(EAmmoType AmmoType) { MAmmoType = AmmoType; }
	EAmmoType GetAmmoType() const { return MAmmoType; }

	FORCEINLINE FName GetReloadSection() const { return MReloadMontageSection; }
	FORCEINLINE void SetReloadSection(const FName& T_MontageSection) { MReloadMontageSection = T_MontageSection; }

	FORCEINLINE int32 GetMagazineCapacity() const { return MMagazineCapacity; }
	FORCEINLINE void SetMagazineCapacity(int32 T_MagazineCapacity) { MMagazineCapacity = T_MagazineCapacity; }

	FORCEINLINE void SetIsMovingClip(bool MoveClip) { MisMovingClip = MoveClip; }
	FORCEINLINE bool GetIsMovingClip() const { return MisMovingClip; }
	FORCEINLINE bool GetWeaponIsFalling() const { return MisWeaponFalling; }
	FORCEINLINE FName GetWeaponClipBoneName() const { return MWeaponClipBoneName; }

	FORCEINLINE void SetWeaponClipBoneName(const FName& T_WeaponClipName) { MWeaponClipBoneName = T_WeaponClipName; }
	FORCEINLINE float GetWeaponFireRate() const { return MAutoFireRate; }
	FORCEINLINE UParticleSystem* GetMuzzleFlash() const { return MWeaponMuzzleFlash; }
	FORCEINLINE USoundBase* GetWeaponFireSound() const { return MGunFireSound; }
	FORCEINLINE void SetAmmoAmt(int32 T_AmmoAmt) { MWeaponAmmoAmt = T_AmmoAmt; }
	FORCEINLINE float GetPistolDisplacementValue()const{return MPistolDisplacementValue;}
	FORCEINLINE bool GetIsAutomaticFire()const{return MisAutomaticFire;}
	FORCEINLINE USoundBase*GetWeaponBulletFlyBy()const{return MWeaponBulletFlyby;}
	FORCEINLINE float GetWeaponDamageAmount()const{return MWeaponDamageAmount;}
	FORCEINLINE float GetWeaponHeadShotDmgAmount()const{return MWeaponHeadShotDamageAmount;}
	FORCEINLINE UMaterialInterface*GetBulletDecal()const{return MBulletDecal;}
};
