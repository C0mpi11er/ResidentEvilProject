// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HealthBar.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERSERIES_API UHealthBar : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	//function sets enemy health in par with progress bar
	void InitializeEnemyHealth() const;

private:
	/*bind widget of  progress bar*/
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UProgressBar> MHealthProgressBar;
	/*Enemy that owns each widget class */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class AEnemy> MEnemyOwner;

public:
	FORCEINLINE void SetEnemyOwner(AEnemy* T_Enemy) { MEnemyOwner = T_Enemy; }
};
