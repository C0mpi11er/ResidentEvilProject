// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "UObject/ObjectPtr.h"
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AmmoPopWidget.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERSERIES_API UAmmoPopWidget : public UUserWidget
{
	GENERATED_BODY()
private:
	/*ammo class that owns the widget*/
	UPROPERTY(BlueprintReadOnly,meta=(AllowPrivateAccess="true"),Category=AmmoPropteties)
	TObjectPtr<class AAmmo>MAmmoOwner;

public:
	void SetAmmoReference(AAmmo*T_Ammo);
};
