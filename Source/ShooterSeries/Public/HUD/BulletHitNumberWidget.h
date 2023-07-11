// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "UObject/ObjectPtr.h"
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BulletHitNumberWidget.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERSERIES_API UBulletHitNumberWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//sets the bullet hit amount for widget to either body shot or head
	UFUNCTION(BlueprintCallable)
	void SetBulletHitAmount(int HitAmount);
	//set color of the bullet hit amount dependency on body or head shot
	UFUNCTION(BlueprintCallable)
	void SetBulletHitColour(bool isHeadShot) const;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> MBulletHitAmount;
};
