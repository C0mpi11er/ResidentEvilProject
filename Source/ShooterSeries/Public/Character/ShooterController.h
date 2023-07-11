// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "UObject/ObjectPtr.h"
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShooterController.generated.h"


/**
 * 
 */
UCLASS()
class SHOOTERSERIES_API AShooterController : public APlayerController
{
	GENERATED_BODY()

public:
	AShooterController();
	virtual void Tick(float DeltaSeconds) override;
	UFUNCTION()
	void SetAmmoWidgetWeapon(class AWeapon* AmmoWidgetWeapon) const;
	//to set shooter char as health widget owner
	void InitHealthWidget();

protected:
	virtual void BeginPlay() override;
	// draw overlay widget on screen
	void DrawScreenWidget();

private:
	//for bp widget
	UPROPERTY()
	TObjectPtr<class UUserWidget> MOverlayWidget;
	//static class cpp widget
	UPROPERTY(EditAnywhere, Category=ControllerItems)
	TSubclassOf<UUserWidget> MOverlayWidgetClass;

	UPROPERTY()
	TObjectPtr<class UShooterCharHealthWidget> MShooterCharHealthWidget;

public:
	FORCEINLINE UShooterCharHealthWidget* GetHealthWidget() const { return MShooterCharHealthWidget; }
	FORCEINLINE UUserWidget* GetOverlayWidget() const { return MOverlayWidget; }
};
