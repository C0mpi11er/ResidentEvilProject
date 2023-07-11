// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/AmmoCountWidget.h"
#include "Components/TextBlock.h"
#include "Character/ShooterCharacter.h"
#include "Weapon/Weapon.h"


void UAmmoCountWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetShooterCha();
	if (MWeapon)
	{
		SetWeaponBullet(30);
	}
	
}



void UAmmoCountWidget::SetWeaponBullet(int32 Bullet) const
{
	if (MAmmoInGun)
		SetMAmmoInGun(Bullet);
}

void UAmmoCountWidget::BindCarriedAmmo() const
{
	//check for shooter char
	if (MShooterChar)
	{
		//check for equipped weapon and shooter carries weapon ammo type
		if (MWeapon&&MShooterChar->GetAmmoMap().Contains(
			MWeapon->GetAmmoType()))
		{
			//init carried weapon with ammo amount shooter carries and bind carried weapon 
			const int32 CarriedAmmo = MShooterChar->GetAmmoMap()[MWeapon->GetAmmoType()];
			SetCarriedAmmo(CarriedAmmo);
			UE_LOG(LogTemp, Warning, TEXT("bind Carried Weapon"))
		}
	}
}


void UAmmoCountWidget::SetMAmmoInGun(int32 GunAmmoCount) const
{
	//convert to string format then to text
	const FString StringAmmo{FString::Printf(TEXT("%d"), GunAmmoCount)};
	const FText TextAmmo{FText::FromString(StringAmmo)};
	if (MAmmoInGun)
		MAmmoInGun->SetText(TextAmmo);
}

void UAmmoCountWidget::SetCarriedAmmo(int32 CarriedAmmo) const
{
	//convert to string format then to text
	const FString StringAmmo{FString::Printf(TEXT("%d"), CarriedAmmo)};
	const FText TextAmmo{FText::FromString(StringAmmo)};
	if (MCarriedAmmo)
		MCarriedAmmo->SetText(TextAmmo);
}

void UAmmoCountWidget::SetShooterCha()
{
	MShooterChar = Cast<AShooterCharacter>(GetOwningPlayerPawn());
}

void UAmmoCountWidget::SetWeapon(AWeapon* EquippedWeapon)
{
	if (EquippedWeapon)
	{
		MWeapon = EquippedWeapon;
		if (MWeapon)
		{
			UE_LOG(LogTemp, Warning, TEXT("init weapon:%s"),*EquippedWeapon->GetName())
			
		}
			
	}
}
