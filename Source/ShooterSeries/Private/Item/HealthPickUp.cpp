// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/HealthPickUp.h"
#include "Components/SphereComponent.h"
#include "Character/ShooterCharacter.h"
#include "Kismet/GameplayStatics.h"
// Sets default values
AHealthPickUp::AHealthPickUp():MHealthAmount(10.f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MHealthStaticMeshComponent=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HealthMesh"));
	MHealthStaticMeshComponent->SetupAttachment(GetRootComponent());
	
}

// Called when the game starts or when spawned
void AHealthPickUp::BeginPlay()
{
	Super::BeginPlay();
	
}

void AHealthPickUp::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (OtherActor)
	{
		if(const auto ShooterChar{Cast<AShooterCharacter>(OtherActor)};ShooterChar)
		{
			//if health pick is proportionate to max health or more fill up health bar
			if((ShooterChar->GetHealth()+MHealthAmount)>=ShooterChar->GetMaxHealth())
				ShooterChar->SetHealth(ShooterChar->GetMaxHealth());
			else
			{
				//add proportionate amount to the health bar
				ShooterChar->SetHealth(ShooterChar->GetHealth()+MHealthAmount);
			}

			if(MPickUpSound)
				UGameplayStatics::PlaySoundAtLocation(this,MPickUpSound,ShooterChar->GetActorLocation());
		}
		
	}
	Destroy();
}



// Called every frame
void AHealthPickUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

