// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Weapon.h"



void AWeapon::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	const FString WeaponTablePath{
		"/Script/Engine.DataTable'/Game/Game/Character/Assets/Items/Weapons/DataTable/WeaponDataTable.WeaponDataTable'"
	};

	const UDataTable* WeaponTableObject{
		(Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *WeaponTablePath)))
	};
	if (WeaponTableObject)
	{
		FWeaponDataTable* WeaponTableRow{nullptr};
		switch (MWeaponType)
		{
		case EWeaponType::EWT_SubMachineGun:
			WeaponTableRow = WeaponTableObject->FindRow<FWeaponDataTable>(FName{"SubMachineGun"},TEXT(""));
			break;

		case EWeaponType::EWT_AssaultRifle:
			WeaponTableRow = WeaponTableObject->FindRow<FWeaponDataTable>(FName{"AssaultRifle"},TEXT(""));
			break;

		case EWeaponType::EWT_Pistol:
			WeaponTableRow = WeaponTableObject->FindRow<FWeaponDataTable>(FName{"Pistol"},TEXT(""));
		default: break;;
		}

		if (WeaponTableRow)
		{
			SetItemName(WeaponTableRow->MWeaponName);
			SetMagazineCapacity(WeaponTableRow->MMagazineCapacity);
			SetAmmoAmt(WeaponTableRow->MAmmoAmt);
			GetMItemMesh()->SetSkeletalMesh(WeaponTableRow->MWeaponMesh);
			SetItemIcon(WeaponTableRow->MWeaponIcon);
			SetAmmoIcon(WeaponTableRow->MAmmoIcon);
			SetEquipSound(WeaponTableRow->MEquipSound);
			SetPickUpSound(WeaponTableRow->MPickUpSound);
			SetWeaponType(WeaponTableRow->MWeaponType);
			SetMMaterialInstance(WeaponTableRow->MWeaponMaterialInstance);
			//clearing previous material index and setting the instance
			MPreviousMaterialIndex = GetMMaterialIndex();
			GetMItemMesh()->SetMaterial(MPreviousMaterialIndex, nullptr);
			SetMMaterialIndex(WeaponTableRow->MMaterialIndex);
			SetWeaponClipBoneName(WeaponTableRow->MMagazineClipName);
			SetReloadSection(WeaponTableRow->MReloadSectionName);
			GetMItemMesh()->SetAnimInstanceClass(WeaponTableRow->MWeaponBluePrint);
			MCrossHairDown = WeaponTableRow->CrossHairDown;
			MCrossHairUp = WeaponTableRow->CrossHairUp;
			MCrossHairLeft = WeaponTableRow->CrossHairLeft;
			MCrossHairRight = WeaponTableRow->CrossHairRight;
			MCrossHairMiddle = WeaponTableRow->CrossHairMiddle;
			MAutoFireRate = WeaponTableRow->WeaponFireRate;
			MGunFireSound = WeaponTableRow->WeaponSound;
			MWeaponMuzzleFlash = WeaponTableRow->MuzzleFlash;
			MBoneToHide = WeaponTableRow->BoneToHide;
			MisAutomaticFire=WeaponTableRow->IsAutomaticFire;
			MWeaponBulletFlyby=WeaponTableRow->WeaponBulletFlyby;
			MWeaponDamageAmount=WeaponTableRow->DamageAmount;
			MWeaponHeadShotDamageAmount=WeaponTableRow->HeadShotDamageAmount;
		}


		if (GetMMaterialInstance())
		{
			//set item material
			SetMaterialInstanceDynamic(UMaterialInstanceDynamic::Create(GetMMaterialInstance(), this));
			GetMItemMesh()->SetMaterial(GetMMaterialIndex(), GetMaterialInstanceDynamic());
			GetMaterialInstanceDynamic()->SetVectorParameterValue(FName{"FresnelColour"}, GetGlowColor());
			EnableGlowMaterial();
		}
	}
}


AWeapon::AWeapon()

{
	PrimaryActorTick.bCanEverTick = true;
}

void AWeapon::ThrowWeapon()
{
	//get weapon mesh rotation in shooterHand and set it
	const FRotator WeaponRotation{0.f, GetMItemMesh()->GetComponentRotation().Yaw, 0.f};
	GetMItemMesh()->SetWorldRotation(WeaponRotation, false, nullptr, ETeleportType::TeleportPhysics);
	//get weapon forward vector and right vector
	[[maybe_unused]] const FVector WeaponForward{GetMItemMesh()->GetForwardVector()};
	const FVector WeaponRight{GetMItemMesh()->GetRightVector()};
	//impulse direction
	FVector ImpulseDirection{WeaponRight.RotateAngleAxis(FMath::FRandRange(-20.f, 30.f), FVector(0.f, 0.f, 1.f))};
	ImpulseDirection *= 5'000; //distance of throw
	GetMItemMesh()->AddImpulse(ImpulseDirection);
	MisWeaponFalling = true;
	GetWorldTimerManager().SetTimer(MWeaponFallTimerHandle, this, &AWeapon::StopFalling, MWeaponFallTime);
	EnableGlowMaterial();
}

void AWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GetItemState() == EItemState::EIS_Falling && MisWeaponFalling)
	{
		FRotator WeaponRotation{0.f, GetMItemMesh()->GetComponentRotation().Yaw, 0.f};
		GetMItemMesh()->SetWorldRotation(WeaponRotation, false, nullptr, ETeleportType::TeleportPhysics);
	}
	UpdatePistolSlideDisplacement();
}

bool AWeapon::MagazineClipFull() const
{
	return MWeaponAmmoAmt == MMagazineCapacity;
}

void AWeapon::StopFalling()
{
	MisWeaponFalling = false;
	SetItemState(EItemState::EIS_PickUp);
	StartPulseTimer();
}

void AWeapon::UpdatePistolSlideDisplacement()
{
	if (MPistolCurve && MisPistolSlideDisplaced)
	{
		//get elapsed time
		const float ElaspedTime{GetWorldTimerManager().GetTimerElapsed(MPistolSlideTimerHandle)};
		//get pistol curve value
		const float PistolCurveValue{MPistolCurve->GetFloatValue(ElaspedTime)};

		//init pistol slide displacement with scaled value of curve and maxDisplacement
		MPistolDisplacementValue = PistolCurveValue * MPistolMaxDisplacement;
	}
}

void AWeapon::StartPistolSlideTimer()
{
	MisPistolSlideDisplaced = true;
	GetWorldTimerManager().SetTimer(MPistolSlideTimerHandle, this, &AWeapon::EndPistolSlideTimer, MPistolSlideTime);
}

void AWeapon::EndPistolSlideTimer()
{
	MisPistolSlideDisplaced = false;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (MBoneToHide != FName{""})
	{
		GetMItemMesh()->HideBoneByName(MBoneToHide, PBO_None);
	}
	
}


void AWeapon::DecreaseAmmo()
{
	if (MWeaponAmmoAmt - 1 <= 0)
	{
		MWeaponAmmoAmt = 0;
	}
	else
	{
		--MWeaponAmmoAmt;
	}
}

void AWeapon::ReloadWeapon(int32 Ammo)
{
	checkf(MWeaponAmmoAmt+Ammo<=MMagazineCapacity, TEXT("Warning Magazine full"))
	MWeaponAmmoAmt += Ammo;
}
