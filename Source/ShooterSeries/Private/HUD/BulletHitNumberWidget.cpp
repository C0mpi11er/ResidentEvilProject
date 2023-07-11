// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/BulletHitNumberWidget.h"
#include "Components/TextBlock.h"

void UBulletHitNumberWidget::SetBulletHitAmount(int HitAmount)
{
	//convert to string
	const FString HitString{FString::Printf(TEXT("%d"),HitAmount)};

	const FText HitText{FText::FromString(HitString)};
	if(MBulletHitAmount)
		MBulletHitAmount->SetText(HitText);
	
}

void UBulletHitNumberWidget::SetBulletHitColour(bool isHeadShot) const
{
	const FSlateColor BodyShotColor{FColor::White};
	const FSlateColor HeadshotColor{FColor::Orange};

	if(isHeadShot)
	{
		if(MBulletHitAmount)
			MBulletHitAmount->SetColorAndOpacity(HeadshotColor);
	}

	else
	{
		if(MBulletHitAmount)
			MBulletHitAmount->SetColorAndOpacity(BodyShotColor);
	}
		
	
	
	
}
