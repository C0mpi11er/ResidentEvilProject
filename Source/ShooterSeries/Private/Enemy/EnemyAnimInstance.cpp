// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAnimInstance.h"

#include "Enemy/Enemy.h"

void UEnemyAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
   if(!MEnemyOwner)
   	 MEnemyOwner=Cast<AEnemy>(TryGetPawnOwner());
   
   if (MEnemyOwner)
   {
	  FVector Speed{ MEnemyOwner->GetVelocity()};
   	//zero z axis of velocity bcs it not needed.
   	Speed.Z=0.f;
   	MEnemyGroundSpeed=Speed.Size();
   }


	
}

void UEnemyAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
	UpdateAnimationProperties(DeltaSeconds);
	UpdateDeathPose();
	UpdateIsDead();
}

void UEnemyAnimInstance::UpdateDeathPose()
{
	if (MEnemyOwner)
		MEnemyDeathPose=MEnemyOwner->GetDeathPose();
}

void UEnemyAnimInstance::UpdateIsDead()
{
	if(MEnemyOwner)
		bisDead=MEnemyOwner->GetIsDead();
}
