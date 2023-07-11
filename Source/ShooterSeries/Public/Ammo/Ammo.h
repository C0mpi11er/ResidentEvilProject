// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "ShooterSeries/AmmoType.h"
#include "UObject/ObjectPtr.h"
#include "CoreMinimal.h"
#include "Item/Item.h"
#include "Ammo.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERSERIES_API AAmmo : public AItem
{
	GENERATED_BODY()

	AAmmo();

public:
	virtual void SetItemProperties(EItemState ItemState) const override;
	virtual void Tick(float DeltaSeconds) override;
	UFUNCTION()
	void AmmoSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent*
	                       OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void EnableCustomDepth() override;
	virtual void DisableCustomDepth() override;
	

protected:
	virtual void BeginPlay() override;
	void InitAmmoPopWidget();

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> MAmmoMesh;

	UPROPERTY(EditAnywhere)
	EAmmoType MAmmoType{EAmmoType::EAT_9mmAmmo};

	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(AllowPrivateAccess="true"),Category=AmmoTexture)
	TObjectPtr<class UTexture2D>MAmmoIconTexture2D;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USphereComponent>MAmmoCollisionSphere;

public:
	FORCEINLINE EAmmoType GetAmmoType()const{return MAmmoType;}
	
	FORCEINLINE UStaticMeshComponent* GetAmmoMesh()const{return MAmmoMesh;}
};
