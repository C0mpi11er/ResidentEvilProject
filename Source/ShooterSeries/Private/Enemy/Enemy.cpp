// Fill out your copyright notice in the Description page of Project Settings.
#include "Enemy/Enemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/EnemyController.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Enemy/EnemyAnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "HUD/HealthBar.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SphereComponent.h"
#include "Character/ShooterCharacter.h"
#include "Components/BoxComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
AEnemy::AEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//enemy capsule comp blocks visibilty
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);

	GetMesh()->SetGenerateOverlapEvents(true);
	//enemy mesh blocks visibility channel
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECR_Block);
	MHealthBarComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarComponent"));
	MHealthBarComponent->SetupAttachment(GetRootComponent());
	MAgroSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	MAgroSphereComponent->SetupAttachment(GetRootComponent());
	MAgroSphereComponent->SetSphereRadius(400.f);
	MAttackSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("AttackRadius"));
	MAttackSphereComponent->SetupAttachment(GetRootComponent());
	//set mesh and capsule comp
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	//setting up left and right hand box components
	MRightBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("RightHandBoxComp"));
	MRightBoxComponent->SetupAttachment(GetMesh(), FName{TEXT("RightHandSocket")});
	MRightBoxComponent->SetCollisionObjectType(ECC_WorldDynamic);
	MRightBoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MRightBoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	MRightBoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);

	MLeftBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftHandBoxComp"));
	MLeftBoxComponent->SetupAttachment(GetMesh(), FName{TEXT("LeftHandSocket")});
	MLeftBoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MLeftBoxComponent->SetCollisionObjectType(ECC_WorldDynamic);
	MLeftBoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	MLeftBoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void AEnemy::FinishedDying()
{
	//to set deathPose
	constexpr float DestroyTime{10.f};
	FTimerHandle DestroyHandle;
	GetWorldTimerManager().SetTimer(DestroyHandle, this, &AEnemy::EnemyDestroy, DestroyTime);
}

void AEnemy::EnemyDestroy()
{
	Destroy();
}


void AEnemy::InitializeHealthBarComponent()
{
	if (MHealthBarComponent)
		if (UHealthBar* HealthBar{Cast<UHealthBar>(MHealthBarComponent->GetUserWidgetObject())})
			HealthBar->SetEnemyOwner(this);
}


FVector AEnemy::PatrolToWorldVector(const FVector& T_PatrolVector) const
{
	return UKismetMathLibrary::TransformLocation(GetActorTransform(), T_PatrolVector);
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	MEnemyController = Cast<AEnemyController>(GetController());

   if(MEnemyController)
   	    MEnemyController->GetBlackBoardComponent()->SetValueAsBool(FName{TEXT("CanAttack")},true);
	
	GetCharacterMovement()->MaxWalkSpeed = MMovementWalkSpeed;
	
	
	if (MAgroSphereComponent)
	{
		MAgroSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnAgroSphereOverlap);
	}


	if (MAttackSphereComponent)
	{
		MAttackSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnAttackSphereOverlap);
		MAttackSphereComponent->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnAttackSphereEndOverlap);
	}

	if (MRightBoxComponent)
	{
		MRightBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnRightHandBeginOverlap);
	}


	if (MLeftBoxComponent)
	{
		MLeftBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnLeftHandBeginOverlap);
	}


	InitializeHealthBarComponent();
	HideHealthBar();

	const FVector WorldPatrolPoint{(PatrolToWorldVector(MPatrolPoint))};
	const FVector WorldPatrolPoint2{PatrolToWorldVector(MPatrolPoint2)};

	if (MEnemyController && MEnemyController->GetBlackBoardComponent())
	{
		//init black board key patrol point
		MEnemyController->GetBlackBoardComponent()->SetValueAsVector(FName{"PatrolPoint"}, WorldPatrolPoint);

		MEnemyController->GetBlackBoardComponent()->SetValueAsVector(FName{"PatrolPoint2"}, WorldPatrolPoint2);

		MEnemyController->RunBehaviorTree(MEnemyBehaviorTree);
	}
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                         AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	//health reduction should be reduce but always withing range of clamped figures
	if (MHealth > 0.f)
	{
		MHealth = FMath::Clamp(MHealth - DamageAmount, 0.f, MMaxHealth);
		ShowHealthBar();

		if (Cast<AShooterCharacter>(DamageCauser) && MEnemyController)
		{
			MEnemyController->GetBlackBoardComponent()->SetValueAsObject(FName{"Target"}, DamageCauser);
			bIsHavingTarget = true;
		}
	}
	else
	{
		Die();
	}

	return DamageAmount;
}

void AEnemy::PlayHitReactMontage(const FName& T_MontageSection, float PlayRate) const
{
	if (UEnemyAnimInstance* EnemyAnimInstance{(Cast<UEnemyAnimInstance>(GetMesh()->GetAnimInstance()))};
		EnemyAnimInstance && MHitReactMontage)
	{
		EnemyAnimInstance->Montage_Play(MHitReactMontage);
		EnemyAnimInstance->Montage_JumpToSection(T_MontageSection);
	}
}

void AEnemy::ShowHealthBar_Implementation()
{
	//clear any already  ticking time
	GetWorldTimerManager().ClearTimer(MHealthBarTimerHandle);
	//set a new countdown to hide health bar
	GetWorldTimerManager().SetTimer(MHealthBarTimerHandle, this, &AEnemy::HideHealthBar, MHealthBarTimer);
}

void AEnemy::DestroyBulletHitWidget(UUserWidget* T_HitNumberWidget)
{
	//Remove Bullet Hit From Map
	MBulletHitMap.Remove(T_HitNumberWidget);
	//Remove widget from screen
	T_HitNumberWidget->RemoveFromParent();
}

void AEnemy::SetStunnedState(bool T_isStunned)
{
	//first set the enemy stunned value
	MisStunned = T_isStunned;

	if (MEnemyController)
	{
		MEnemyController->GetBlackBoardComponent()->SetValueAsBool(FName{"Stunned"}, T_isStunned);
	}
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateBulletHitNumberWidget();
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::BulletHit_Implementation(FHitResult& T_HitResult, AActor* T_Shooter, AController* T_ShooterController)
{
	if (bisDead)return;
	IBulletHitInterface::BulletHit_Implementation(T_HitResult, T_Shooter, T_ShooterController);
	//spawn particle at bullet impact point
	if (MParticleSystem)
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, MParticleSystem, T_HitResult.ImpactPoint);

	//play bullet impact sound at enemy location
	if (MEnemyHitSound)
		UGameplayStatics::PlaySoundAtLocation(this, MEnemyHitSound, GetActorLocation());

	if (MHealthBarComponent)
		ShowHealthBar();

	const float Stunned{FMath::RandRange(0.1f, 1.f)};
	if (Stunned <= MChanceOfBeingStunned)
	{
		if (bMCanPlayHitReact)
		{
			SetStunnedState(true);
				PlayHitReactMontage("HitReact1");
			
				bMCanPlayHitReact = false;
				GetWorldTimerManager().SetTimer(MHitReactTimerHandle, this, &AEnemy::ResetCanHitReact,
												FMath::RandRange(MHitReactMinTime, MHitReactMaxTime));
			
		     	
		}
	}


	UE_LOG(LogTemp, Warning, TEXT("enemyBullet hit called"))
}

void AEnemy::PlayAttackMontage(const FName& T_AttackSection, float T_PlayRate)
{
	if (UEnemyAnimInstance* EnemyAnimInstance{(Cast<UEnemyAnimInstance>(GetMesh()->GetAnimInstance()))};
		EnemyAnimInstance && MAttackMontage)
	{
		EnemyAnimInstance->Montage_Play(MAttackMontage);
		EnemyAnimInstance->Montage_JumpToSection(T_AttackSection);
	}

	bCanAttack = false;
	if (MEnemyController)
		MEnemyController->GetBlackBoardComponent()->SetValueAsBool(FName{"CanAttack"}, false);
	GetWorldTimerManager().SetTimer(CanAttackTimerHandle, this, &AEnemy::ResetCanAttack, MAttackWaitTime);
}

FName AEnemy::GetRandAttackSection() const
{
	switch (FMath::RandRange(1, 5))
	{
	case 1: return MAttackSection1;
	case 2: return MAttackSection2;
	case 3: return MAttackSection3;
	case 4: return MAttackSection4;
	default: return MAttackSection1;
	}
}

void AEnemy::ActivateRightAttack()
{
	MRightBoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	UE_LOG(LogTemp, Warning, TEXT("ActivateRight"))
}

void AEnemy::DeActivateRightAttack()
{
	MRightBoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::ActivateLeftAttack()
{
	MLeftBoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeActivateLeftAttack()
{
	MLeftBoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::DoDamage(AShooterCharacter* Victim)
{
	if (Victim)
	{
		UGameplayStatics::ApplyDamage(Victim, MBaseDamage, MEnemyController, this,
		                              UDamageType::StaticClass());

		if (Victim->GetImpactMeleeSound())
			UGameplayStatics::PlaySoundAtLocation(this, Victim->GetImpactMeleeSound(), GetActorLocation(),
			                                      GetActorRotation());
	}
}

void AEnemy::ResetCanAttack()
{
	bCanAttack = true;
	if (MEnemyController)
		MEnemyController->GetBlackBoardComponent()->SetValueAsBool(FName{"CanAttack"}, true);
}

void AEnemy::PlayDeathMontage()
{
	if (const auto EnemyInstance{Cast<UEnemyAnimInstance>(GetMesh()->GetAnimInstance())}; MDeathMontage)
	{
		EnemyInstance->Montage_Play(MDeathMontage);
		FName SelectionName{""};
		switch (FMath::RandRange(1, 5))
		{
		case 1: SelectionName = TEXT("Death1");
			MDeathPose = EDeathPose::EDP_DeathPose1;
			break;
		case 2: SelectionName = TEXT("Death2");
			MDeathPose = EDeathPose::EDP_DeathPose2;
			break;
		case 3: SelectionName = TEXT("Death3");
			MDeathPose = EDeathPose::EDP_DeathPose3;
			break;
		case 4: SelectionName = TEXT("Death4");
			MDeathPose = EDeathPose::EDP_DeathPose4;
			break;
		case 5: SelectionName = TEXT("Death5");
			MDeathPose = EDeathPose::EDP_DeathPose5;
			break;
		default: break;
		}
		EnemyInstance->Montage_JumpToSection(SelectionName);
	}
}

void AEnemy::OnRightHandBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                     const FHitResult& SweepResult)
{
	//cast to shooter char
	const auto Shooter{CastToShooterCharacter(OtherActor)};
	if (Shooter) //do damage if not null
		DoDamage(Shooter);

	FTransform SocketTransform;
	if (const USkeletalMeshSocket* HandSocket{GetMesh()->GetSocketByName(MRightHandSocket)})
		SocketTransform = HandSocket->GetSocketTransform(GetMesh());

	if (Shooter && Shooter->GetImpactBlood())
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, Shooter->GetImpactBlood(), SocketTransform.GetLocation());
}

AShooterCharacter* AEnemy::CastToShooterCharacter(AActor* T_Actor) const
{
	if (T_Actor)
	{
		if (AShooterCharacter* Character{Cast<AShooterCharacter>(T_Actor)})
			return Character;
	}
	return nullptr;
}


void AEnemy::OnLeftHandBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                    const FHitResult& SweepResult)
{
	const auto Shooter{CastToShooterCharacter(OtherActor)};
	if (Shooter) //do damage if not null
		DoDamage(Shooter);

	FTransform SocketTransform;
	if (const USkeletalMeshSocket* HandSocket{GetMesh()->GetSocketByName(MLeftHandSocket)})
		SocketTransform = HandSocket->GetSocketTransform(GetMesh());

	if (Shooter && Shooter->GetImpactBlood())
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, Shooter->GetImpactBlood(), SocketTransform.GetLocation());
}


void AEnemy::SetInAttackRadius(bool T_isInAttackRadius)
{
	// for enemy to know shooter char is in attack radius
	bMisInAttackRadius = T_isInAttackRadius;

	if (MEnemyController)
	{
		//for black board key to know if it in attack radius or not
		MEnemyController->GetBlackBoardComponent()->SetValueAsBool(FName{"InAttackRange"}, true);
	}
}

void AEnemy::OnAttackSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                   const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		if (const AShooterCharacter* ShooterCharacter{Cast<AShooterCharacter>(OtherActor)}; ShooterCharacter)
			SetInAttackRadius(true);

		
	}
}

void AEnemy::OnAttackSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		if (Cast<AShooterCharacter>(OtherActor))
			SetInAttackRadius(false);
		
	}
}

void AEnemy::StoreHitNumberAndLocation(UUserWidget* T_HitNumberWidget, const FVector& BulletHit)
{
	if (bisDead) return;
	MBulletHitMap.Add(T_HitNumberWidget, BulletHit);
	FTimerHandle BulletHitDestroyHandle;
	FTimerDelegate BulletHitDestroyDelegate;
	//bind call back function to delegate
	BulletHitDestroyDelegate.BindUFunction(this, FName{"DestroyBulletHitWidget"}, T_HitNumberWidget);
	GetWorldTimerManager().SetTimer(BulletHitDestroyHandle, BulletHitDestroyDelegate, MBulletHitDestroyTimer, false);
}

void AEnemy::Die()
{
	if (bisDead)return;
	bisDead = true;
	HideHealthBar();
	PlayDeathMontage();
	if (MEnemyController)
	{
		MEnemyController->StopMovement();
		MEnemyController->GetBlackBoardComponent()->SetValueAsBool(FName{"isDead"}, true);
		MEnemyController->PawnPendingDestroy(this);
	}
}

void AEnemy::ResetCanHitReact()
{
	bMCanPlayHitReact = true;
	/*if(MEnemyController)
		MEnemyController->GetBlackBoardComponent()->SetValueAsBool(FName{"Stunned"},false);*/
}

void AEnemy::UpdateBulletHitNumberWidget()
{
	for (const auto Pair : MBulletHitMap)
	{
		//init keys and values of bulletMap
		UUserWidget* UserWidget{Pair.Key};
		FVector NumberLocation{Pair.Value};
		//screen position that widget will be project to at runtime
		FVector2d ScreenPosition;
		UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(), NumberLocation, ScreenPosition);
		if (UserWidget)
			UserWidget->SetPositionInViewport(ScreenPosition);
	}
}

void AEnemy::OnAgroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                 const FHitResult& SweepResult)
{
	if(!OtherActor)return;
	if (AShooterCharacter* ShooterCharacter{Cast<AShooterCharacter>(OtherActor)}; MEnemyController)
	{
		MEnemyController->GetBlackBoardComponent()->SetValueAsObject(FName{"Target"}, ShooterCharacter);
		bIsHavingTarget = true;
	}
}
