// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include  "UObject/ObjectPtr.h"
#include "Interface/BulletHitInterface.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Explosive.generated.h"

UCLASS()
class SHOOTERSERIES_API AExplosive : public AActor,public IBulletHitInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExplosive();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void BulletHit_Implementation(FHitResult& T_HitResult, AActor* T_Shooter, AController* T_ShooterController) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	//mesh of the explosive object
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent>MExplosiveMeshComponent;
	//sound the object makes when it explodes
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase>MExplosiveSound;
  //the particle system that emits when explosive mesh is hit by bullet
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UParticleSystem>MExplosvieParticleSystem;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class USphereComponent>MDamageRadius;

	UPROPERTY(EditAnywhere)
	float MBaseDamage{15.f};
	


};
