// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "UObject/ObjectPtr.h"
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShooterCharHealthWidget.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERSERIES_API UShooterCharHealthWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	// to set the progress bar percent


private:
	UPROPERTY()
	TObjectPtr<class AShooterCharacter> MShooterCharacter;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UProgressBar> MHealthProgressBar;

public:
	void SetShooterChar(class AShooterCharacter* T_ShooterCharacter);
	void SetHealthProgressBar(float T_Health) const;
};
