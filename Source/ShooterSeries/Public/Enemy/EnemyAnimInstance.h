// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/ObjectPtr.h"
#include "CoreMinimal.h"
#include "Enemy.h"
#include "Animation/AnimInstance.h"
#include "EnemyAnimInstance.generated.h"

enum class EDeathPose : uint8;

/**
 * 
 */
UCLASS

()
class SHOOTERSERIES_API UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	void UpdateAnimationProperties(float DeltaTime);

	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

	void UpdateDeathPose();

	void UpdateIsDead();

private:
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TObjectPtr<class AEnemy> MEnemyOwner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category=MovementProperties)
	float MEnemyGroundSpeed{0.f};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category=Death)
	EDeathPose MEnemyDeathPose{EDeathPose::EDP_DeathPose1};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category=Death)
	bool bisDead{false};


};
