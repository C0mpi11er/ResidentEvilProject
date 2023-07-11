// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/ObjectPtr.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Item.generated.h"

UENUM(BlueprintType)
enum class EItemRarity:uint8
{
	EIR_Damaged UMETA(DisplayName="Damaged"),
	EIR_Common UMETA(DisplayName="Common"),
	EIR_UnCommon UMETA(DisplayName="UnCommon"),
	EIR_Rare UMETA(DisplayName="Rare"),
	EIR_Legendary UMETA(DisplayName="Legendary"),

	EIR_Max UMETA(DisplayName="Max"),
};

UENUM(BlueprintType)
enum class EItemType:uint8
{
	EIT_Ammo UMETA(DisplayName="Ammo"),
	EIT_Weapon UMETA(DisplayName="Weapon"),
	EIT_MAX UMETA(DisplayName="Max")
};

UENUM(BlueprintType)
enum class EItemState:uint8
{
	EIS_PickUp UMETA(DisplayName="PickUp"),
	EIS_EquipInterping UMETA(DisplayName="EquipInterping"),
	EIS_PickedUp UMETA(DisplayName="PickedUp"),
	EIS_Equipped UMETA(DisplayName="Equipped"),
	EIS_Falling UMETA(DisplayName="Falling"),

	EIS_Max UMETA(DisplayName="Max")
};

USTRUCT(BlueprintType)
struct FItemRarityTable : public FTableRowBase
{
	GENERATED_BODY()

	/**
	 * @brief Glow Color of the item
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor MGlowColor;

	/**
	 * @brief the upper light region of the item pickup widget
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor MLightColor;

	/**
	 * @brief the lower light region of the item pickup widget
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor MDarkColor;

	/**
	 * @brief number of stars each item has to reflect rarity
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MNumberOfStars;

	/**
	 * @brief item background type to also reflect rarity
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture2D> MItemBackground;
};

UCLASS()
class SHOOTERSERIES_API AItem : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AItem();
	/*makes widget invisible*/
	void InitializeWidgetInvisibility() const;
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	/*plays the pick up sound during a stipulated period of time*/
	void PlayPickupSound(bool T_bForcedSoundPlay = false) const;

	void PlayEquipSound(bool T_bForceSoundPlay = false) const;

	/*custom depth functions*/

	/*enables custom depth effect*/
	virtual void EnableCustomDepth();
	/*disable custom depth effect*/
	virtual void DisableCustomDepth();
	/*initialize custom depth ine begin play*/
	virtual void InitCustomDepth();

	/*enable glow material*/
	void EnableGlowMaterial() const;
	/*disable glow material*/
	void DisableGlowMaterial() const;


	virtual void OnConstruction(const FTransform& Transform) override;

	/*pulse material glow functions*/
	/*rests the pulse glow*/
	UFUNCTION()
	void ResetPulseTimer();
	/*starts the pulse glow*/
	void StartPulseTimer();

	void UpdatePulse() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	/*return the float value of actor oscillation offset*/
	float SinTransform() const;


	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent*
	                        OverlappedComponent, AActor* OtherActor,
	                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION() //call back function for start item curve.
	void StopInterping();

	void ItemInterp(float DeltaTime);

	virtual void SetItemProperties(EItemState ItemState) const;
	/*returns location of uscene comp base on item type*/
	UFUNCTION()
	FVector GetSceneInterpLocation();


	/*name of item*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=ItemProperties, meta=(AllowPrivateAccess="true"))
	FString MItemName{""};
	/*number figure on each item*/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=ItemProperties, meta=(AllowPrivateAccess="true"))
	int32 MItemCount{7};


	/*the item mesh,it is also the root component*/
	UPROPERTY(VisibleAnywhere, Category=ItemProperties)
	TObjectPtr<class USkeletalMeshComponent> MItemMeshComponent;
	/*to alert collision when line trace is performed*/
	UPROPERTY(VisibleAnywhere, Category=ItemProperties)
	TObjectPtr<class UBoxComponent> MBoxComponent;
	/*pops up when box comp blocks line trace*/
	UPROPERTY(EditAnywhere, Category=ItemProperties)
	TObjectPtr<class UWidgetComponent> MPopUpWidget;
	/*check for overlap event*/
	UPROPERTY(EditAnywhere, Category=ItemProperties)
	TObjectPtr<class USphereComponent> MOverlappingSphere;


	/*the rarity of item*/
	UPROPERTY(EditAnywhere, Category=ItemProperties)
	EItemRarity MItemRarity{EItemRarity::EIR_Common};
	/*TODO:check use*/
	UPROPERTY()
	TArray<bool> MActiveStars;

	UPROPERTY(VisibleAnywhere, Category=ItemProperties)
	EItemState MItemState{EItemState::EIS_PickUp};

	UPROPERTY(EditDefaultsOnly, Category=ItemProperties)
	TObjectPtr<class UCurveFloat> MZCurve;

	UPROPERTY(EditDefaultsOnly, Category=ItemProperties)
	TObjectPtr<class UCurveFloat> MItemScaleCurve;

	/*interpolation variables*/
	UPROPERTY() //start location of interpolating item
	FVector MItemInterpStartLoc{};
	UPROPERTY() //camera location item is interping to
	FVector MItemInterpCamLoc{};
	//check if item interping or not
	bool MisInterping{false};
	// a timer handle for counting down to when interping is done
	FTimerHandle MInterpTimerHandle{};

	UPROPERTY() //pointer to the shooter char
	TObjectPtr<class AShooterCharacter> MShooterChar;
	// time limit for interping.
	const float MZCurveTime{0.7};

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> MPickUpSound;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> MEquipSound;

	/*used to identify item type*/
	UPROPERTY(VisibleAnywhere)
	;
	EItemType MItemType{EItemType::EIT_Ammo};
	/*variable holds index of f_interp location array index*/
	int32 MFinterpLocIndex{0};
	/*to init material index*/
	UPROPERTY(EditAnywhere, Category=WeaponMaterial, BlueprintReadOnly, meta=(AllowPrivateAccess))
	int32 MMaterialIndex{0};

private:
	UPROPERTY(EditAnywhere, Category=WeaponMaterial, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TObjectPtr<UMaterialInstance> MMaterialInstance;

	UPROPERTY(VisibleAnywhere, Category=WeaponMaterial, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TObjectPtr<UMaterialInstanceDynamic> MMaterialInstanceDynamic;

	/*a bool check to validate the custom depth toggle*/
	bool MCanEnableCustomDepth{true};
	/*item pulse variables*/
	/*vector curve for pulse glow*/
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UCurveVector> MVectorPulseCurve;
	/*scalable variables for item pulse*/
	float MFresnelExponent{3.f};
	float MGlowAmount{150.f};
	float MFresnelFraction{3.f};

	FTimerHandle MPulseTimerHandle;
	/*time till next Pulse glow Starter*/
	float MPulseTimer{5.f};
	/*glows material wen interping*/
	UPROPERTY(EditAnywhere)
	TObjectPtr<UCurveVector> MInterpingVectorCurve;

	UPROPERTY(VisibleAnywhere, Category=Rarity, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TObjectPtr<class UTexture2D> MItemBackground;
	/*weapon inventory icon*/
	UPROPERTY(EditAnywhere, Category=ItemProperties, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TObjectPtr<UTexture2D> MItemIcon;
	/*weapon ammo inventory icon*/
	UPROPERTY(EditAnywhere, Category=ItemProperties, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TObjectPtr<UTexture2D> MAmmoIcon;
	/**
	 * @brief :keeps track of character inventory
	 */
	UPROPERTY(EditAnywhere, Category=ItemProperties, BlueprintReadOnly, meta=(AllowPrivateAccess))
	bool MisCharInevtoryFull{false};
	UPROPERTY(EditAnywhere, Category=ItemProperties, BlueprintReadOnly, meta=(AllowPrivateAccess))
	int32 MSlotIndex{0};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	TObjectPtr<class UDataTable> MItemDataTable;

	/**
	 * @brief Glow color for item
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess), Category=Rarity)
	FLinearColor MGlowColor;

	/**
	 * @brief upper light color of pick up widget
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess), Category=Rarity)
	FLinearColor MLightColor;

	/**
	 * @brief lower dark color for pick up widget
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess), Category=Rarity)
	FLinearColor MDarkColor;

	/**
	 * @brief number of stars to reflect rarity on pick up widget
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess), Category=Rarity)
	int32 MNumberOfStars;

	/*oscillation and rotation properties*/
	/*will be equivalent of DeltaTime*/
	UPROPERTY(EditAnywhere, Category=SinParameters)
	float MRunningTime{0.f};
	/*degree of actor offset wen multiplied by SinTransform*/
	UPROPERTY(EditAnywhere, Category=SinParameters)
	float MAmplitude{0.25f};
	/*amount to multiply the running time by*/
	UPROPERTY(EditAnywhere, Category=SinParameters)
	float MtimeConstant{4.f};
	/*degree to which the item actor should rotate each frame*/
	UPROPERTY(EditAnywhere,Category=SinParameters)
	float MRotationRate{1.5f};

public:
	FORCEINLINE UWidgetComponent* GetPopUpWidgetComponent() const { return MPopUpWidget; }

	FORCEINLINE FString GetItemName() const { return MItemName; }

	void SetItemName(FString& itemName);

	void SetActiveStars();

	FORCEINLINE USkeletalMeshComponent* GetMItemMesh() const { return MItemMeshComponent; }

	FORCEINLINE TArray<bool> GetActiveStars() const { return MActiveStars; }

	FORCEINLINE EItemRarity GetItemRarity() const { return MItemRarity; }

	FORCEINLINE UBoxComponent* GetMBoxComponent() const { return MBoxComponent; }

	FORCEINLINE USphereComponent* GetOverLappingSphereComponent() const { return MOverlappingSphere; }

	FORCEINLINE EItemState GetItemState() const { return MItemState; }

	void SetItemState(EItemState ItemState);

	void StartItemCurve(AShooterCharacter* T_ShooterChar, bool T_bForcedSoundPlay = false);

	FORCEINLINE USoundBase* GetPickupSound() const { return MPickUpSound; }
	FORCEINLINE USoundBase* GetEquipSound() const { return MEquipSound; }
	FORCEINLINE void SetPickUpSound(USoundBase* T_MPickSound) { MPickUpSound = T_MPickSound; }
	FORCEINLINE void SetEquipSound(USoundBase* T_MEquipSound) { MEquipSound = T_MEquipSound; }

	FORCEINLINE int32 GetItemCount() const { return MItemCount; }

	FORCEINLINE int32 GetSlotIndex() const { return MSlotIndex; }

	void SetSlotIndex(int32 T_SlotIndex) { MSlotIndex = T_SlotIndex; }

	FORCEINLINE void SetisCharInevtoryFull(bool T_isFull) { MisCharInevtoryFull = T_isFull; }
	FORCEINLINE bool GetisCharInventoryFull() const { return MisCharInevtoryFull; }

	FORCEINLINE void SetItemIcon(UTexture2D* T_ItemIcon) { MItemIcon = T_ItemIcon; }
	FORCEINLINE void SetAmmoIcon(UTexture2D* T_AmmoIcon) { MAmmoIcon = T_AmmoIcon; }

	FORCEINLINE int32 GetMMaterialIndex() const { return MMaterialIndex; }
	FORCEINLINE void SetMMaterialIndex(int32 T_MMaterialIndex) { MMaterialIndex = T_MMaterialIndex; }
	FORCEINLINE UMaterialInstance* GetMMaterialInstance() const { return MMaterialInstance; }
	FORCEINLINE void SetMMaterialInstance(UMaterialInstance* T_MMaterialInstance)
	{
		MMaterialInstance = T_MMaterialInstance;
	}

	FORCEINLINE void SetMaterialInstanceDynamic(UMaterialInstanceDynamic* T_MaterialInstanceDynamic)
	{
		MMaterialInstanceDynamic = T_MaterialInstanceDynamic;
	}

	FORCEINLINE UMaterialInstanceDynamic* GetMaterialInstanceDynamic() const { return MMaterialInstanceDynamic; }

	FORCEINLINE FLinearColor GetGlowColor() const { return MGlowColor; }
};
