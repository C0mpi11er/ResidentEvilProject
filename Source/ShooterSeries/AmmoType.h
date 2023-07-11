#pragma once

UENUM(BlueprintType)
enum class EAmmoType :uint8
{
	EAT_9mmAmmo UMETA(DisplayName="9mmAmmo"),
	EAT_AssaultRifleAmmo UMETA(DisplayName="AssaultRifleAmmo"),
	EAT_MAX UMETA(DisplayName="MAX")
};