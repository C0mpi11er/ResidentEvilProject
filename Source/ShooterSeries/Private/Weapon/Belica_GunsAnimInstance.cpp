// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Belica_GunsAnimInstance.h"
#include "Character/ShooterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Weapon/Weapon.h"

void UBelica_GunsAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	InitializeShooterCharacter();
	InitializeWeapon();
	
	
}


void UBelica_GunsAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
	if (!MOwningWeapon)
		MOwningWeapon = Cast<AWeapon>(GetOwningActor());
	if (!MShooterCharacter)
		MShooterCharacter = Cast<AShooterCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (MShooterCharacter && MShooterCharacter->GetCombatState() == ECombatState::ECS_Reloading)
	{
		if (IsWeaponClipMoving())
			InitializeHandCompTransform();
	}

	if (MOwningWeapon&&MOwningWeapon->GetWeaponType()==EWeaponType::EWT_Pistol)
	{
		MPistolDisplacement=MOwningWeapon->GetPistolDisplacementValue();
	}
}

void UBelica_GunsAnimInstance::InitializeShooterCharacter()
{
	MShooterCharacter=Cast<AShooterCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(),0));
	
	if(MShooterCharacter)
		UE_LOG(LogTemp,Warning,TEXT("shootercharbelica guns"))
	
}

void UBelica_GunsAnimInstance::InitializeWeapon()
{
	MOwningWeapon = Cast<AWeapon>(GetOwningActor());
	if (MOwningWeapon)
	{ 
		UE_LOG(LogTemp, Warning, TEXT("weapon belica guns"))
	}
	
}

void UBelica_GunsAnimInstance::InitializeHandCompTransform()
{

	if (MShooterCharacter)
	{
		MHandClipTransform=MShooterCharacter->GetHandClipComponent()->GetComponentTransform();
		UE_LOG(LogTemp,Warning,TEXT("hand comp belica guns called"))
	}
		
	
}

bool UBelica_GunsAnimInstance::IsWeaponClipMoving()
{

	if (MOwningWeapon)
	{
		UE_LOG(LogTemp,Warning,TEXT("moving clip belica guns called"))
		 MisMovingClip=MOwningWeapon->GetIsMovingClip();
		return MisMovingClip;
	}
		return false;
}
