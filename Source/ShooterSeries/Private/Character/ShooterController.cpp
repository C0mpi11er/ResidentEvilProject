// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/ShooterController.h"
#include "Character/ShooterCharHealthWidget.h"
#include  "Blueprint/UserWidget.h"
#include "HUD/AmmoCountWidget.h"
#include "Weapon/Weapon.h"

AShooterController::AShooterController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AShooterController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AShooterController::SetAmmoWidgetWeapon(AWeapon* AmmoWidgetWeapon) const
{
	if(!MOverlayWidget)return;
	if (const auto AmmoWidget{(Cast<UAmmoCountWidget>(MOverlayWidget->GetWidgetFromName(TEXT("BP_AmmoCount"))))})
	{
		if (AmmoWidgetWeapon)
		{
			AmmoWidget->SetWeapon(AmmoWidgetWeapon);
			UE_LOG(LogTemp, Warning, TEXT("Ammo widget gotten: %s"), *AmmoWidget->GetName());
		}
	}
}

void AShooterController::BeginPlay()
{
	Super::BeginPlay();
	DrawScreenWidget();
	InitHealthWidget();
}

void AShooterController::DrawScreenWidget()
{
	if (MOverlayWidgetClass)
	{
		MOverlayWidget = CreateWidget(this, MOverlayWidgetClass);
		if (MOverlayWidget)
			MOverlayWidget->AddToViewport(1);
	}
}

void AShooterController::InitHealthWidget()
{
	MShooterCharHealthWidget = Cast<UShooterCharHealthWidget>(MOverlayWidget->GetWidgetFromName(FName{"UMG_ShooterHealth"}));

	if (MShooterCharHealthWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("init health widget from controller"))
	}
}
