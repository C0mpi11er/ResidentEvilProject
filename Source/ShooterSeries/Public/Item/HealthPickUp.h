// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Item.h"
#include "UObject/ObjectPtr.h"
#include "CoreMinimal.h"
#include "HealthPickUp.generated.h"


UCLASS()
class SHOOTERSERIES_API AHealthPickUp : public AItem
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AHealthPickUp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
   virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;	
	
 
private:
	//mesh of the health comp
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> MHealthStaticMeshComponent;
	
	float MHealthAmount;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
