#pragma once
#include "UObject/ObjectMacros.h"


UENUM(BlueprintType)
enum class EWeaponType:uint8
{
	EWT_SubMachineGun UMETA(DisplayName="SMG"),
	EWT_AssaultRifle UMETA(DisplayName="AssaultRifle"),
	EWT_Pistol UMETA(DisplayName="Pistol"),
	EWT_MAX UMETA(DisplayName="DefaultMax")
};