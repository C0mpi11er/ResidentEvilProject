// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ShooterCharHealthWidget.h"
#include "Character/ShooterCharacter.h"
#include  "Components/ProgressBar.h"


void UShooterCharHealthWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}


void UShooterCharHealthWidget::SetShooterChar(AShooterCharacter* T_ShooterCharacter)
{
	MShooterCharacter = T_ShooterCharacter;
}

void UShooterCharHealthWidget::SetHealthProgressBar(float T_Health) const
{
	if (MHealthProgressBar)
		MHealthProgressBar->SetPercent(T_Health);
}
