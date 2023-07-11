// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UObject/ObjectPtr.h"
#include "ShooterSeries/AmmoType.h"
#include "ShooterCharacter.generated.h"


UENUM(BlueprintType)
enum class ECombatState:uint8
{
	ECS_UnOccupied UMETA(DisplayName="UnOccupied"),
	ECS_Reloading UMETA(DisplayName="Reloading"),
	ECS_Firing UMETA(DisplayName="Firing"),
	ECS_Equipping UMETA(DisplayName="Equipping"),
	ECS_Stunned UMETA(DisplayName="Stunned"),
	ECS_MAX UMETA(DisplayName="MAX")
};

USTRUCT()
struct FInterpolation
{
	GENERATED_BODY()
	/*interpolation component location*/
	UPROPERTY()
	TObjectPtr<USceneComponent> MInterpSceneComponent;
	/*number of items interpolating to a location*/
	int32 ItemCount;
};


enum class EWeaponType:uint8;

class AWeapon;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquipItemDelegate, int32, CurrentSlotIndex, int32, NewSlotIndex);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHighlightIconDelegate, int32, SlotIndex, bool, StartAnimation);

UCLASS()
class SHOOTERSERIES_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();
	void DefaultCamFov();

	UFUNCTION(BlueprintCallable)
	void EndStun();
	//called when shooter char dies
	void Die();
	//anime notify of deathMontage
	UFUNCTION(BlueprintCallable)
	void FinishedDead();

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                         AActor* DamageCauser) override;
	/**
	 * @brief returns next empty weapon slot index
	 */
	int32 GetEmptyWeaponSlot();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/*called to make character jump*/
	virtual void Jump() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	//check if shooter char is overlapping an item
	void CheckOverlappingItem(bool IsOverlappingItem);
	void CheckcharInventory();
	//check for item by triggering temp cross hair trace when overlapping.
	void CheckForItem();

	//auto triggers what to do after weapon or item has interpolation to camera
	void GetPickUpItem(class AItem* ItemToPickUp);

	/*set look up and look right rate*/
	void SetMLookUpRateDegree(float LookupRate) { MLookUpRateDegree = LookupRate; }

	void SetMLookRightRateDegree(float LookRightRate) { MLookRightRateDegree = LookRightRate; }
	/*anim notify*/
	UFUNCTION(BlueprintCallable)
	void FinishReloading();
	/*return the scene comp index with the lowest item count at location*/
	UFUNCTION()
	int32 FinterLowestIndex();

	/*increases f_interpolation index item count amount*/
	void IncreaseFInterpItemCount(int32 index, int32 Amount);

	/**
	 * @brief triggers weapon inventory slot highlight
	 */
	void HighlightIconsSlot();

	/**
	 * @brief deactivates  weapon inventory slot highlight 
	 */
	void UnHighlightIconSlot();

	/*pick and equip sound functions*/
	void ResetPickupSoundTimer();
	void ResetEquipSoundTimer();

	void StartPickupSoundTimer();
	void StartEquipSoundTimer();
	/*</>*/

protected:
	void PlayStunnedMontage() const;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	/*called to pick up ammo*/
	void PickUpAmmo(class AAmmo* T_Ammo);
	/*init all structure interpolation objects*/
	void InitFInterpolationLoc();

	/*performs line trace and reflect what shooter char is stepping on*/
	UFUNCTION(BlueprintCallable)
	EPhysicalSurface GetPhysicalSurfaceType();

	/*MovementInputBindings*/
	void MoveForward(float Value);
	void MoveRight(float Value);
	/*Scaling Function To Look Up other Scaling Variables May Affect
	 * @Param Value is normalized i.e 1.0f is 100%
	 */
	void LookUpRate(float Value);
	void LookRightRate(float Value);
	/*turn @Param:ViewportSize to to engine editor view port*/
	void InitializeViewPortSize(FVector2d& ViewPortSize);
	/*plays the hip fire montage*/
	void PlayHipFireMontage() const;
	/*plays EquipRifle Montage*/
	void PlayEquipRifleMontage();
	/*called from blue print sets char state to unoccupied*/
	UFUNCTION(BlueprintCallable)
	void FinishEquipRifleMontage();
	/*performs Cross Hair Trace*/
	bool TempCrossHairTrace(FHitResult& OutHit, FVector& T_SmokeBeamEndPoint,
	                        const USkeletalMeshSocket* T_MuzzleSocket = nullptr);
	/*init default shooter char weapon*/
	UFUNCTION()
	AWeapon* SetDefaultWeapon() const;

	/*equip weapon*/
	void EquipWeapon(AWeapon* Weapon, bool T_bisSwapping = false);
	/*Drop weapon*/
	void DropWeapon();
	/*Swap Weapon*/
	void SwapWeapon(AWeapon* WeaponToSwap);

	/*ReloadWeapon*/
	void ReloadWeapon();

	/*input functions for select button*/
	UFUNCTION()
	void SelectButtonPressed();
	UFUNCTION()
	void SelectButtonReleased();
	void InitiShooterWalkCrouchSpeed();
	void InterpolateCapsuleHeight(float DeltaTime);
	UFUNCTION()
	void CrouchButtonPressed();

	UFUNCTION()
	void ReloadButtonPressed();

	void PlayGunSound() const;
	void EmitMuzzleFlash(const USkeletalMeshSocket*& ShooterMuzzleSocket) const;
	void InitGunBarrelTrace(const UWorld* World, FVector& BulletBeamEnd,
	                        const USkeletalMeshSocket* ShooterMuzzleSocket) const;
	UParticleSystemComponent* SpawnSmokeBeam(const USkeletalMeshSocket* ShooterMuzzleSocket) const;
	void EmitBulletImpactParticle(const FVector& BulletBeamEnd) const;
	void SmokeBeamTrace(const FVector& BulletBeamEnd, const USkeletalMeshSocket* ShooterMuzzleSocket) const;
	void EmitBeam_BulletFX(const FVector& BulletBeamEnd, const USkeletalMeshSocket* ShooterMuzzleSocket) const;
	bool ProjectCrossHair(const FVector2d& CrossHairScreenLocation, const APlayerController* ShooterController,
	                      FVector& CrossHairStartLocation, FVector& CrossHairEndLocation);
	void CrossHairTrace(const FVector& CrossHairStartLocation, const FVector& CrossHairEndLocation,
	                    FHitResult& CrossHairTraceHitResult) const;
	//returns bullet decal
	UDecalComponent* GetBulletDecal(FHitResult CrossHairTraceHitResult, FRotator DecalRo) const;
	void ReleaseBullet(const USkeletalMeshSocket*& ShooterMuzzleSocket);
	void InitializeMuzzleSocket(const USkeletalMeshSocket*& ShooterMuzzleSocket);

	/*called to make shooter character fire weapon*/
	void FireWeapon();
	void Aim();
	/*called when shooter char zoomed aim or not*/
	void AimButtonPressed();
	void StopAiming();
	void AimButtonReleased();
	/*makes cross hair move slower wen aiming*/
	void InterpolationWeaponAim(float DeltaTime);
	/*sets the crossHair sensitivity*/
	void SetLookSensitivity();

	void FKeyPressed();
	void OneKeyPressed();
	void TwoKeyPressed();
	/*switches the equipped weapon tio the next-slot-index  inventory*/
	void SwitchInventoryWeapon(int32 T_CurrentSlotIndex, int32 T_NextSlotIndex);

	void CalculateCrossHairSpread(float DeltaTime);
	/*called after cross hair firing
	 *to call the end cross hair firing timer
	 *to shrink back after every shot to default*/
	UFUNCTION()
	void StartCrossHairFiringTimer();
	/*called by start cross hair firing to to set cross hair is firing bool to false */
	UFUNCTION()
	void EndCrossHairFiringTimer();

	/*init @MisfireWeaponButton button to true to true*/
	void PressFireButton();
	/*init @ MisfireWeaponButton to false*/
	void ReleaseFireButton();
	/*starts the next trigger pull time rate count down
	 * after firing weapon
	 */
	void StartAutomaticFireTimer();

	/*resets the can shoot bool and check if shoot button is still pressed*/
	UFUNCTION()
	void AutoResetFiringTimer();
	//init ammo  amount for each weapon
	void InitializeAmmoMap();
	//check if weapon has ammo
	bool IsWeaponHavingAmmo() const;
	/*checks if shooter char has ammo of weapon type*/
	bool IsCarryingAmmo() const;


	UFUNCTION(BlueprintCallable)
	void GrabClip();
	UFUNCTION(BlueprintCallable)
	void ReturnClip();

private:
	/*Shooter Character Property SpringArm*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess), Category=Camera)
	TObjectPtr<class USpringArmComponent> MSpringArmComponent;
	/*Follow Camera*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess), Category=Camera)
	TObjectPtr<class UCameraComponent> MCameraComponent;

	/*Degree Character Should look Right */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess), Category=Controller)
	float MLookRightRateDegree{45.f};
	/*Degree Character Should Look Up*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess), Category=Controller)
	float MLookUpRateDegree{45.f};
	/*Look up rate sensitivity when aiming*/
	UPROPERTY(VisibleAnywhere, meta=(AllowPrivateAccess), Category=Controller,
		meta=(ClampMin="0.0", ClampMax="15.0", UIMin="0.0", UIMax="15.0"))
	float MAimingLookUpRateDegree{15.f};
	/*Look right rate sensitivity while aiming*/
	UPROPERTY(VisibleAnywhere, meta=(AllowPrivateAccess), Category=Controller,
		meta=(ClampMin="0.0", ClampMax="15.0", UIMin="0.0", UIMax="15.0"))
	float MAimingLookRightRateDegree{15.f};

	/*look up and right rate when weapon position on hip*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess), Category=Controller)
	float MHipLookRightRateDegree{80.f};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess), Category=Controller)
	float MHipLookUpRateDegree{80.f};


	/*GunFire Sound ref*/ //not needed anymore
	/*UPROPERTY(EditDefaultsOnly, Category=Combat)
	TObjectPtr<class USoundBase> MGunFireSound;*/
	/*Muzzle effect*/ //not needed anymore
	/*UPROPERTY(EditDefaultsOnly, Category=Combat)
	TObjectPtr<class UParticleSystem> MMuzzleFlash;*/
	/*For Recoil and fire animation*/
	UPROPERTY(EditDefaultsOnly, Category=Combat)
	TObjectPtr<class UAnimMontage> MHipFireMontage;
	/*Reload weapon montage*/
	UPROPERTY(EditAnywhere, Category=Combat)
	TObjectPtr<UAnimMontage> MReloadMontage;
	/*montage for Equipping rifles*/
	UPROPERTY(EditAnywhere, Category=Combat)
	TObjectPtr<UAnimMontage> MEquipRifleMontage;

	/*fx for bullet impact*/
	UPROPERTY(EditDefaultsOnly, Category=Combat)
	TObjectPtr<class UParticleSystem> MBulletImpactFx;
	/*Smoke Trail That Follows Bullet*/
	UPROPERTY(EditDefaultsOnly, Category=Combat)
	TObjectPtr<class UParticleSystem> MBulletBeamFx;
	/*if true weapon shooter char zoomed aim*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess))
	bool MisZoomedAim{false};
	/*Camera default field of view is set in begin play*/
	UPROPERTY(EditDefaultsOnly)
	float MDefaultCameraFov{0.f};
	/*current camera field of view*/
	UPROPERTY()
	float MCurrentCamFov{0.f};
	/*Camera Zoomed Fov*/
	UPROPERTY(EditDefaultsOnly)
	float MCameraZoomedFov{25.f};

	UPROPERTY(EditDefaultsOnly, Category=Combat)
	float MAimZoomInterpolationSpeed{10.f};

	/*implements the spread of the cross hair*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category=Combat)
	float MCrossHairMultiplier{0.f};
	/*calculate the velocity of CrossHair*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category=Combat)
	float MCrossHairVelocity{0.f};
	/*the air component of the CrossHair*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category=Combat)
	float MCrossAirComponent{0.f};
	/*the shooting component of the CrossHair*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category=Combat)
	float MCrossHairShootingComponent{0.f};
	/*the air component of the CrossHair*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category=Combat)
	float MCrossHairAimComponent{0.f};
	/*use to keep track if shooter character is firing*/
	UPROPERTY()
	bool MisCrossHairFiring{false};
	/*for setting the firing check back to false after recoil time*/
	UPROPERTY()
	FTimerHandle MCrossHairTimerHandle;
	/*time it takes for cross hair to return to default after each firing*/
	UPROPERTY()
	float MCrossHairRecoilTime{0.05f};

	/*Automatic fire Variable*/
	/*Checks if shooter char can fire or not*/
	bool MbCanFire{true};
	/*time to next trigger pull*/
	/*float MTriggerPullRate{0.1f};*/ //not needed anymore
	/*check if fire button is pressed*/
	bool MisfireButtonPressed{false};
	/*time manager for automatic weapon call back*/
	UPROPERTY()
	FTimerHandle MAutoFireTimeHandle;

	bool MisOverLappingItem{false};
	/*points to the last item actor that was traced by cross-hair*/
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class AItem> MLastTraceItem;

	/*current equipped shooter char weapon*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess="true"))
	TObjectPtr<AWeapon> MEquippedWeapon;

	/*blue print class of default shooter char weapon*/
	UPROPERTY(EditDefaultsOnly, Category=Combat)
	TSubclassOf<AWeapon> MDefaultWeaponClass;
	//distance from follow camera forward vector
	UPROPERTY(EditAnywhere)
	float MCamWeapInterpolationDistance{250.f};
	//distance @Prop:MCamWeaponInterpolationDistance vertically
	UPROPERTY(EditAnywhere)
	float MCamWeapInterpolationHeight{65.f};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category=Combat)
	TMap<EAmmoType, int32> MAmmoMap;

	/*starting 9mm ammo*/
	int32 MStarting9MMAmmo{7};
	/*starting assault rifle ammo*/
	int32 MStartingAssaultAmmo{11};
	UPROPERTY(VisibleAnywhere)
	ECombatState MCombatState{ECombatState::ECS_UnOccupied};


	/*UPROPERTY()
	EWeaponType MWeaponType;*/
	//use to attach and return weapon clip
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TObjectPtr<USceneComponent> MHandClipComponent;

	bool MisCrouching{false};
	/*normal movement speed of shooter char*/
	float MBaseWalkSpeed{500.f};

	/*crouch movement speed of shooter char*/
	float MCrouchWalkSpeed{100.f};

	/*Current Capsule  height*/
	float MCurrentCapsuleHalfHeight{0.f};
	/*Standing Capsule Height*/
	float MStandingCapsuleHalfHeight{88.f};

	/*Capsule Height When Crouching*/
	float MCrouchCapsuleHeight{44.f};

	/*Standing brake friction*/
	float MStandFrictionAmt{2.0};
	/*Crouching Brake Friction*/
	float MCrouchFrictionAmt{100.0};

	bool MisAimButtonPressed{false};
	/*interpolation location container*/
	TArray<FInterpolation> MInterpLocationArray;

	/*interpolation scene component variables*/
	UPROPERTY(EditAnywhere)
	TObjectPtr<USceneComponent> MWeaponInterpComp;
	UPROPERTY(EditAnywhere)
	TObjectPtr<USceneComponent> MInterpComp1;
	UPROPERTY(EditAnywhere)
	TObjectPtr<USceneComponent> MInterpComp2;
	UPROPERTY(EditAnywhere)
	TObjectPtr<USceneComponent> MInterpComp3;
	UPROPERTY(EditAnywhere)
	TObjectPtr<USceneComponent> MInterpComp4;
	UPROPERTY(EditAnywhere)
	TObjectPtr<USceneComponent> MInterpComp5;
	UPROPERTY(EditAnywhere)
	TObjectPtr<USceneComponent> MInterpComp6;

	/*pick up sound variables*/
	FTimerHandle MPickUpSoundTimerHandle;
	FTimerHandle MEquipSoundTimerHandle;
	/*time to reset the pickup sound bool*/
	float MPickupSoundResetTime{0.5f};
	float MEquipSoundResetTime{0.5f};
	/*checks if pickup sound should be played*/
	bool MShouldPlayPickupSound{true};
	bool MShouldPlayEquipSound{true};

	/*Array inventory to store weapons*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess), Category=Inventory)
	TArray<class AItem*> MItemInventory;
	/*capacity of item inventory*/
	int32 MInventoryCapacity{3};

	/*delegate that broadcast information to the inventory class*/
	UPROPERTY(EditAnywhere, BlueprintAssignable, meta=(AllowPrivateAccess="true"), Category=Delegate)
	FEquipItemDelegate MEquipItemDelegate;

	UPROPERTY(EditAnywhere, BlueprintAssignable, meta=(AllowPrivateAccess="true"), Category=Delegate)
	FHighlightIconDelegate MHighlightIconDelegate;

	/**
	 * @brief Keeps track of current highLightedSlot default initialization means no slot is highlighted
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category=Delegate)
	int32 MHighlightedSlot{-1};

	/*keeps check of bullet hitting enemy or mare actor*/
	bool MIsBulletValidHit{false};

	//current health
	UPROPERTY(VisibleAnywhere, Category=Combat)
	float MHealth{100.f};
	//maximum health
	UPROPERTY(VisibleAnywhere, Category=Combat)
	float MMaxHealth{100.f};

	UPROPERTY()
	TObjectPtr<class AShooterController> MShooterController;
	//sound when shooter is hit
	UPROPERTY(EditAnywhere)
	TObjectPtr<class USoundBase> MImpactMelee;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UNiagaraSystem> MImpactBlood;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UAnimMontage> MStunnedMontage;
	//chance of the shooter char being stunned
	//below 0.5 not stunned above 0.5 stunned
	float MShooterStunnedChance{0.5};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category=Combat)
	bool bIsDead{false};

	UPROPERTY(EditAnywhere, Category=Combat)
	TObjectPtr<UAnimMontage> MDeathMontage;

public:
	/*Returns Camera Boom*/
	FORCEINLINE USpringArmComponent* GetMSpringArmComponent() const { return MSpringArmComponent; }

	/*Returns Follow Camera*/
	FORCEINLINE UCameraComponent* GetMCameraComponent() const { return MCameraComponent; }

	/*Returns Zoomed Aim*/
	FORCEINLINE bool GetMisZoomedAim() const { return MisZoomedAim; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE AWeapon* GetEquippedWeapon() const { return MEquippedWeapon; }

	//no longer used
	//returns camera interpolation location;
	/*FVector GetCamInterpolationLocation();*/
	//returns cross hair multiplier 
	UFUNCTION(BlueprintCallable)
	float GetMCrossHairMultiplier() const;

	void SetCombatState(ECombatState TCharacterState) { MCombatState = TCharacterState; }
	ECombatState GetCombatState() const { return MCombatState; }

	UFUNCTION(BlueprintCallable)
	TMap<EAmmoType, int32> GetAmmoMap() const { return MAmmoMap; }


	FORCEINLINE USceneComponent* GetHandClipComponent() const { return MHandClipComponent; }

	/*bool checks if char is crouching or not*/
	FORCEINLINE bool GetIsCrouching() const { return MisCrouching; }

	FInterpolation GetFInterLocation(int32 index);

	bool GetShouldPlayPickupSound() const { return MShouldPlayPickupSound; }
	bool GetShouldPlayEquipSound() const { return MShouldPlayEquipSound; }

	FORCEINLINE float GetHealth() const { return MHealth; }
	FORCEINLINE void SetHealth(float T_Health){MHealth=T_Health;}

	FORCEINLINE float GetMaxHealth() const { return MMaxHealth; }
	

	FORCEINLINE USoundBase* GetImpactMeleeSound() const { return MImpactMelee; }

	FORCEINLINE UNiagaraSystem* GetImpactBlood() const { return MImpactBlood; }
	/*
	void SetWeaponType(EWeaponType WeaponType){MWeaponType=WeaponType;}
	EWeaponType GetWeaponType()const{return MWeaponType;}*/
	FORCEINLINE bool GetIsDead() const { return bIsDead; }
};
