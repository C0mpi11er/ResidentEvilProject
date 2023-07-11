// Fill out your copyright notice in the Description page of Project Settings.

#include "Explosive/Explosive.h"

#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
// Sets default values
AExplosive::AExplosive()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MExplosiveMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ExplosiveMeshComp"));
	SetRootComponent(MExplosiveMeshComponent);
	MDamageRadius = CreateDefaultSubobject<USphereComponent>(TEXT("DamageRadius"));
	MDamageRadius->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AExplosive::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AExplosive::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AExplosive::BulletHit_Implementation(FHitResult& T_HitResult, AActor* T_Shooter, AController* T_ShooterController)
{
	IBulletHitInterface::BulletHit_Implementation(T_HitResult, T_Shooter, T_ShooterController);
	//spawn explosive particles
	if (MExplosvieParticleSystem)
		UGameplayStatics::SpawnEmitterAtLocation(this, MExplosvieParticleSystem, T_HitResult.Location);
	//play explosion sound
	if (MExplosiveSound)
		UGameplayStatics::PlaySoundAtLocation(this, MExplosiveSound, T_HitResult.Location);

	//init array of actors
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, ACharacter::StaticClass());
	for (const auto Actors : OverlappingActors)
	{
		UGameplayStatics::ApplyDamage(Actors, MBaseDamage, T_ShooterController, T_Shooter, UDamageType::StaticClass());
	}


	Destroy();
}
