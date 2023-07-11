// Fill out your copyright notice in the Description page of Project Settings.


#include "Ammo/Ammo.h"

#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "HUD/AmmoPopWidget.h"
#include "Character/ShooterCharacter.h"

AAmmo::AAmmo()
{
	PrimaryActorTick.bCanEverTick = true;
	MAmmoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AmmoMesh"));
	MAmmoMesh->SetupAttachment(RootComponent);
	GetMBoxComponent()->SetupAttachment(RootComponent);
	GetOverLappingSphereComponent()->SetupAttachment(RootComponent);
	GetPopUpWidgetComponent()->SetupAttachment(RootComponent);
	MAmmoCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AmmoColSphere"));
	MAmmoCollisionSphere->SetupAttachment(RootComponent);
	MAmmoCollisionSphere->SetSphereRadius(80.f);
}

void AAmmo::SetItemProperties(EItemState ItemState) const
{
	Super::SetItemProperties(ItemState);

	switch (ItemState)
	{
	case EItemState::EIS_PickUp: //item properties
		MAmmoMesh->SetSimulatePhysics(false);
		MAmmoMesh->SetEnableGravity(false);
		MAmmoMesh->SetVisibility(true);
		MAmmoMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		MAmmoMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;

	case EItemState::EIS_Equipped: //item properties
		MAmmoMesh->SetSimulatePhysics(false);
		MAmmoMesh->SetEnableGravity(false);
		MAmmoMesh->SetVisibility(true);
		MAmmoMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		MAmmoMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;

	case EItemState::EIS_Falling: //item properties
		MAmmoMesh->SetSimulatePhysics(true);
		MAmmoMesh->SetEnableGravity(true);
		MAmmoMesh->SetVisibility(true);
		MAmmoMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		MAmmoMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic,
		                                         ECollisionResponse::ECR_Block);
		MAmmoMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;

	case EItemState::EIS_EquipInterping: //item properties
		MAmmoMesh->SetSimulatePhysics(false);
		MAmmoMesh->SetEnableGravity(false);
		MAmmoMesh->SetVisibility(true);
		MAmmoMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		MAmmoMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;

	default: break;
	}
}

void AAmmo::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AAmmo::AmmoSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                              const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		if (AShooterCharacter* ShooterCharacter{Cast<AShooterCharacter>(OtherActor)})
		{
			StartItemCurve(ShooterCharacter, false);
		}
	}
	if (MAmmoCollisionSphere)
		MAmmoCollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AAmmo::EnableCustomDepth()
{
	MAmmoMesh->SetRenderCustomDepth(true);
}

void AAmmo::DisableCustomDepth()
{
	MAmmoMesh->SetRenderCustomDepth(false);
}



void AAmmo::BeginPlay()
{
	Super::BeginPlay();
	InitAmmoPopWidget();
	if (MAmmoCollisionSphere)
		MAmmoCollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AAmmo::AmmoSphereOverlap);
}

void AAmmo::InitAmmoPopWidget()
{
	if (GetPopUpWidgetComponent())
	{
		if (UAmmoPopWidget* AmmoPopWidget{(Cast<UAmmoPopWidget>(GetPopUpWidgetComponent()->GetUserWidgetObject()))})
		{
			AmmoPopWidget->SetAmmoReference(this);
		}
	}
}
