// Fill out your copyright notice in the Description page of Project Settings.
#include "Character/ShooterCharAnimInstance.h"
#include "Character/ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapon/Weapon.h"

void UShooterCharAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	MShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	
}

void UShooterCharAnimInstance::InitEquppiedWeaponType()
{
	if (MShooterCharacter->GetEquippedWeapon())
	{
		MEquippedWeaponType=MShooterCharacter->GetEquippedWeapon()->GetWeaponType();
	}
}

void UShooterCharAnimInstance::UpdateAnimationProperty(float DeltaTime)
{
	/*if shooter char is null initialize it*/
	if (!MShooterCharacter)MShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
		/*if not null perform task*/
	else if (MShooterCharacter)
	{
		/*init char pitch*/
		InitializePitch();
		/*init char yaw*/
		InitializeYaw();
		/*init reloading*/
		InitializeIsReloading();
	

		
		InitializeCrouching();
		//init equipped weapon type
		InitEquppiedWeaponType();
		//true if shooter character is firing or unoccupied.
		MShouldUseFABRIk = MShooterCharacter->GetCombatState() == ECombatState::ECS_UnOccupied || MShooterCharacter->
			GetCombatState() == ECombatState::ECS_Firing;
		
        /*init equipping with shooter char equipping combat state*/
		MIsEquipping=MShooterCharacter->GetCombatState()==ECombatState::ECS_Equipping;
		/*initializing inAir with char movement is falling bool */
		MbIsInAir = MShooterCharacter->GetCharacterMovement()->IsFalling();
		/*runtime check if shooter char is accelerating or not*/
		if (MShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f)MbIsAccelerating = true;
		else MbIsAccelerating = false;
		/*initializing Shooter Char speed ignoring the z unit axis*/
		FVector ShooterCharacterVelocity{MShooterCharacter->GetCharacterMovement()->Velocity};
		ShooterCharacterVelocity.Z = 0.f;
		/*init ground speed with shooter char velocity*/
		MGroundSpeed = ShooterCharacterVelocity.Size();
		/*init base aim rotation and movement velocity xAxis rotation
		 * inti the movementOffsetYaw with the diff
		 */
		const FRotator BaseAimRotator{MShooterCharacter->GetBaseAimRotation()};
		const FRotator MovementVelocityRotation{UKismetMathLibrary::MakeRotFromX(ShooterCharacterVelocity)};
		MMovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementVelocityRotation, BaseAimRotator).Yaw;
		/*init lastMovement yaw with MovementYaw Before it turns 0*/
		if (ShooterCharacterVelocity.Size() > 0.f)MLastMovOffsetYaw = MMovementOffsetYaw;

	
		TurnInPlace();
		Lean(DeltaTime);
      
	}
}

void UShooterCharAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	if(MShooterCharacter)
		/*init is weapon aimed at animation runtime*/
		MisWeaponAimed = MShooterCharacter->GetMisZoomedAim();
	
	/*set and aim offsetState*/
	InitializeAimOffset();
	
	
	
}

void UShooterCharAnimInstance::InitializeYaw()
{
	if (MShooterCharacter)
	{
		MYaw = MRootBoneYawOffset * -1;
	}
}

void UShooterCharAnimInstance::InitializePitch()
{
	if (MShooterCharacter)
	{
		MPitch = MShooterCharacter->GetBaseAimRotation().Pitch;
	}
}

void UShooterCharAnimInstance::InitializeIsReloading()
{
	if (MShooterCharacter)
	{
		MisReloading = MShooterCharacter->GetCombatState() == ECombatState::ECS_Reloading;
	}
}

void UShooterCharAnimInstance::InitializeCrouching()
{
	if (MShooterCharacter)
	{
		SetCrouching(MShooterCharacter->GetIsCrouching());
	}
}

void UShooterCharAnimInstance::InitializeAimOffset()
{
	if (MisReloading)
	{
		SetAimOffSet(EAimOffSet::EAOS_Reloading);
		UE_LOG(LogTemp,Warning,TEXT("aimoffset:Reloading"))
	}
	else if (MbIsInAir)
	{
		SetAimOffSet(EAimOffSet::EAOS_InAir);
		
	}
	else if (MisWeaponAimed)
	{
		SetAimOffSet(EAimOffSet::EAOS_Aiming);
	}
	else
	{
		SetAimOffSet(EAimOffSet::EAOS_Hip);
	}
}

void UShooterCharAnimInstance::TurnInPlace()
{
	//check for shooter character and speed must not be above zero
	if (MShooterCharacter && !MbIsAccelerating)
	{
		/*init char yaw last frame with the previous character yaw*/
		MTipCharacterYawLastFrame = MTipCurrentCharacterYaw;

		MTipCurrentCharacterYaw = MShooterCharacter->GetActorRotation().Yaw;
		/*get difference between frames*/
		const float YawDelta{MTipCurrentCharacterYaw - MTipCharacterYawLastFrame};
		/*clamp root bone yaw offset to 180and -180*/
		MRootBoneYawOffset = UKismetMathLibrary::NormalizeAxis(MRootBoneYawOffset - YawDelta);
		/*note if minus from the root bone when char is turning left root bone
		 * turns left to try an catch up with controller camera and vice versa
		 */

		/*get the turning curve in animation
		 * if animation is playing ->it returns one and vice versa
		 */
		if ([[maybe_unused]] const float TurningMetaData{GetCurveValue(TEXT("TurningMetaData"))})
		{
			MisTurning=true;
			/*init rotation last frame with rotation-curve and rotation curve with value from rotation curve anim*/
			MRotationCurveLastFrame = MRotationCurve;
			MRotationCurve = GetCurveValue(TEXT("Rotation"));
			const float RotationDelta{MRotationCurve - MRotationCurveLastFrame};
			/*if bone yaw offset is positive->char is turning left and vice versa*/
			MRootBoneYawOffset > 0 ? MRootBoneYawOffset -= RotationDelta : MRootBoneYawOffset += RotationDelta;
			/*getting excess of and minus or adding to root bone yaw depending on if root bone yaw
			 * is positive or negative
			 */
			const float ABSRootBoneOffset{FMath::Abs(MRootBoneYawOffset)};
			if (ABSRootBoneOffset > 90.f)
			{
				const float RootBoneExcess{ABSRootBoneOffset - 90.f};
				MRootBoneYawOffset > 0 ? MRootBoneYawOffset -= RootBoneExcess : MRootBoneYawOffset += RootBoneExcess;
			}

			
		}
	}
	else if (MShooterCharacter && MbIsAccelerating || MbIsInAir)
	{
		/*this snippet set bone yaw offset and other relevant variables to zero
		 * when character begins accelerating to avoid movement bug
		 */
		MRootBoneYawOffset = 0.f;
		MTipCurrentCharacterYaw = MShooterCharacter->GetActorRotation().Yaw;
		MTipCharacterYawLastFrame = MTipCurrentCharacterYaw;
		MRotationCurve = 0.f;
		MRotationCurveLastFrame = 0.f;
	}
     //set recoil weight
	InitAndSetRecoilWeight();
}

void UShooterCharAnimInstance::Lean(float DeltaTime)
{
	if (MShooterCharacter)
	{
		MLeanCharYawLastFrame = MLeanCharYaw;
		MLeanCharYaw = MShooterCharacter->GetActorRotation();
		/*to make target have a bigger value divide,->if multiplied target will have smaller value
		 *get delta of rotators and interpolate from yaw delta to target
		 */
		const FRotator DeltaRotation{UKismetMathLibrary::NormalizedDeltaRotator(MLeanCharYaw, MLeanCharYawLastFrame)};
		const auto Target{DeltaRotation.Yaw / DeltaTime};
		const double LeanInterpolation{FMath::FInterpTo(MLeanCharDeltaYaw, Target, DeltaTime, 6.f)};
		MLeanCharDeltaYaw = FMath::Clamp(LeanInterpolation, -90, 90);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(1, -1, FColor::Green,
			                                 FString::Printf(TEXT("LeanDelta:%f"), MLeanCharDeltaYaw));
		}
	}
}

void UShooterCharAnimInstance::InitAndSetRecoilWeight()
{
	/*if shooter char is turning */
	if (MisTurning)
	{
		/*while turning if reloading or equipping*/
		if(MisReloading||MIsEquipping)
			MRecoilWeight=1.f;
		else MRecoilWeight=0.1;
	}/*if shooter char is crouching*/
	else if (MCrouching)
	{   /*while crouching if reloading or equipping*/
		if(MisReloading||MIsEquipping)
			MRecoilWeight=1.f;
		else MRecoilWeight=0.1;
	}/*shooter char i aiming weapon*/
	else if (MisWeaponAimed)
	{
		MRecoilWeight=0.5f;
	}
	else
	{
		MRecoilWeight=0.5;
	}
	
}

void UShooterCharAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	Montage_Play(MLevelStartMontage);
	Montage_JumpToSection(FName{"LevelStart"});

	
}


