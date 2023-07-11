// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/ObjectPtr.h"
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AmmoCountWidget.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERSERIES_API UAmmoCountWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	void SetWeaponBullet(int32 Bullet) const;
	void BindCarriedAmmo() const;


protected:

private:
	/*bound properties*/
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> MAmmoInGun;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock>MCarriedAmmo;
	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TObjectPtr<class AShooterCharacter> MShooterChar;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,meta=(AllowPrivateAccess="true"))
	TObjectPtr<class AWeapon>MWeapon;

public:
	UTextBlock* GetMAmmoInGun() const { return MAmmoInGun; }

	UFUNCTION()
	void SetMAmmoInGun(int32 GunAmmoCount) const;
	
	void SetCarriedAmmo(int32 CarriedAmmo) const;
	//init shooter char
	void SetShooterCha();
	UFUNCTION()
	void SetWeapon(AWeapon* EquippedWeapon);
	UFUNCTION(BlueprintCallable)
	AShooterCharacter* GetShooterCharacter() const { return MShooterChar; }
};
