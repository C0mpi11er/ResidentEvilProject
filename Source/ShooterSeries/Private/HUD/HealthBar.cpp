// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HealthBar.h"
#include "Components/ProgressBar.h"
#include "Enemy/Enemy.h"



void UHealthBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	InitializeEnemyHealth();
}

void UHealthBar::InitializeEnemyHealth() const
{
	if (MEnemyOwner&&MHealthProgressBar)
	{
	  const float EnemyHealth{MEnemyOwner->GetHealth()/MEnemyOwner->GetMaxHealth()};
		MHealthProgressBar->SetPercent(EnemyHealth);
	}
}
