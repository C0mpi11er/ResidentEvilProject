// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ShooterCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include  "Character/ShooterCharAnimInstance.h"
#include "Character/ShooterController.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Item/Item.h"
#include "Weapon/Weapon.h"
#include "Ammo/Ammo.h"
#include "Character/ShooterCharHealthWidget.h"
#include "Enemy/Enemy.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Interface/BulletHitInterface.h"
#include "Components/DecalComponent.h"
#include "Components/SceneComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/EnemyController.h"
// Sets default values
AShooterCharacter::AShooterCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetCapsuleComponent()->SetHiddenInGame(true);
	/*Creates Camera Boom SubObject*/
	MSpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	MSpringArmComponent->SetupAttachment(RootComponent);
	/*Length of Camera Boom To Shooter Character*/
	MSpringArmComponent->TargetArmLength = 180.f;
	/*Camera Boom to Follow Shooter Character Rotation*/
	MSpringArmComponent->bUsePawnControlRotation = true;
	/*Camera boom adjust view to shooter character not setting it in the middle*/
	GetMSpringArmComponent()->SocketOffset = FVector(0.f, 24.f, 70.f);
	/*spring arm lenght*/
	GetMSpringArmComponent()->TargetArmLength = 232.f;
	/*Create Camera SubObject*/
	MCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	/*Attach Camera To Spring Arm And Spring Arm Socket*/
	MCameraComponent->SetupAttachment(MSpringArmComponent, MSpringArmComponent->SocketName);
	/*Camera Follows Spring Arm Rotation Not Pawn Rotation*/
	MCameraComponent->bUsePawnControlRotation = false;

	/*configure character not to rotate along side controller CameraMovement*/
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = true;
	/*rotates Character to controller movement input*/
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->JumpZVelocity = 600.f;

	MHandClipComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HandClipComp"));
	/*interpolation components*/
	MWeaponInterpComp = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponInterpComp"));
	MInterpComp1 = CreateDefaultSubobject<USceneComponent>(TEXT("InterpComp1"));
	MInterpComp2 = CreateDefaultSubobject<USceneComponent>(TEXT("InterpComp2"));
	MInterpComp3 = CreateDefaultSubobject<USceneComponent>(TEXT("InterpComp3"));
	MInterpComp4 = CreateDefaultSubobject<USceneComponent>(TEXT("InterpComp4"));
	MInterpComp5 = CreateDefaultSubobject<USceneComponent>(TEXT("InterpComp5"));
	MInterpComp6 = CreateDefaultSubobject<USceneComponent>(TEXT("InterpComp6"));

	MWeaponInterpComp->SetupAttachment(GetMCameraComponent());
	MInterpComp1->SetupAttachment(GetMCameraComponent());
	MInterpComp2->SetupAttachment(GetMCameraComponent());
	MInterpComp3->SetupAttachment(GetMCameraComponent());
	MInterpComp4->SetupAttachment(GetMCameraComponent());
	MInterpComp5->SetupAttachment(GetMCameraComponent());
	MInterpComp6->SetupAttachment(GetMCameraComponent());
}

void AShooterCharacter::DefaultCamFov()
{
	if (MCameraComponent)
	{
		MDefaultCameraFov = MCameraComponent->FieldOfView;
		MCurrentCamFov = MDefaultCameraFov;
	}
}

void AShooterCharacter::EndStun()
{
	SetCombatState(ECombatState::ECS_UnOccupied);
	if (MisAimButtonPressed)Aim();
}

void AShooterCharacter::Die()
{
	if(bIsDead)return;
	
	if (const auto ShooterInstance{Cast<UShooterCharAnimInstance>(GetMesh()->GetAnimInstance())};MDeathMontage)
	{
		ShooterInstance->Montage_Play(MDeathMontage);
		ShooterInstance->Montage_JumpToSection(FName{"Death1"});
	}
	bIsDead=true;
}

void AShooterCharacter::FinishedDead()
{
	
	if(const auto PlayerController{UGameplayStatics::GetPlayerController(this,0)})
	{
		GetMesh()->bPauseAnims;
		DisableInput(PlayerController);
		DetachFromControllerPendingDestroy();
	}
		
}

float AShooterCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                                    AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if(bIsDead)return 0;
	if (MHealth > 0.f)
	{
		MHealth = FMath::Clamp(MHealth - DamageAmount, 0.f, MMaxHealth);

		if (FMath::RandRange(0, 1) >= MShooterStunnedChance)
			PlayStunnedMontage();
	}
	else if(MHealth<=0.f)
	{
		Die();
		if(const auto EnemyController{Cast<AEnemyController>(EventInstigator)})
			EnemyController->GetBlackBoardComponent()->SetValueAsBool(FName{"IsCharacterDead"},true);
	}
	return DamageAmount;
}


int32 AShooterCharacter::GetEmptyWeaponSlot()
{
	//loop through inventory and find & return  empty slot
	for (const auto Weapon : MItemInventory)
	{
		if (!Weapon)
		{
			return Weapon->GetSlotIndex();
		}
	}
	if (MItemInventory.Num() < MInventoryCapacity)
		return MItemInventory.Num();

	return -1; //no empty slots
}

void AShooterCharacter::FinishReloading()
{
	//sets combat state to unoccupied
	SetCombatState(ECombatState::ECS_UnOccupied);
	//update ammo map
	//check for equipped weapon and shooter has weapon ammo type
	if (MEquippedWeapon && MAmmoMap.Contains(MEquippedWeapon->GetAmmoType()))
	{
		//get amount of weapon type ammo shooter char is having
		int32 CarriedAmmo{MAmmoMap[MEquippedWeapon->GetAmmoType()]};
		//get amount of bullets missing in the magazine
		const int32 EmptyMagazineSpace{MEquippedWeapon->GetMagazineCapacity() - MEquippedWeapon->GetMWeaponAmmo()};
		//if the magazine slot bigger than ammo carried dump all ammo in the magazine
		//and update carried ammo to zero
		if (EmptyMagazineSpace > CarriedAmmo)
		{
			MEquippedWeapon->ReloadWeapon(CarriedAmmo);
			CarriedAmmo = 0;
			MAmmoMap.Add(MEquippedWeapon->GetAmmoType(), CarriedAmmo);
		}
		else
		{
			/*put amount needed to fill the shoot char magazine and update the carried magazine count*/
			MEquippedWeapon->ReloadWeapon(EmptyMagazineSpace);
			CarriedAmmo -= EmptyMagazineSpace;
			MAmmoMap.Add(MEquippedWeapon->GetAmmoType(), CarriedAmmo);
		}

		if (MisAimButtonPressed)
			Aim();
	}
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	/*init MCameraFov to follow camera fov*/
	DefaultCamFov();
	//give shooter char a default weapon
	EquipWeapon(SetDefaultWeapon(), false);
	if (MEquippedWeapon)MEquippedWeapon->SetSlotIndex(0);
	//init ammo map
	InitializeAmmoMap();
	InitFInterpolationLoc();

	if (AShooterController* controller{(Cast<AShooterController>(GetController()))})
	{
		MShooterController = controller;
	}


	/*if (const AShooterController*ShooterController{Cast<AShooterController>(GetController())})
	{
		if(ShooterController->GetHealthWidget())
		{
			ShooterController->GetHealthWidget()->SetShooterChar(this);
			UE_LOG(LogTemp,Warning,TEXT("init health widget owner"))
		}
	}*/
}

void AShooterCharacter::PickUpAmmo(AAmmo* T_Ammo)
{
	if (!MEquippedWeapon)return;
	if (T_Ammo)
	{
		/*first check if char has the ammo type*/
		if (MAmmoMap.Contains(T_Ammo->GetAmmoType()))
		{
			/*get number am ammo type carried and add pickup ammo amount */
			int32 CarriedAmmoAmt{MAmmoMap[T_Ammo->GetAmmoType()]};

			CarriedAmmoAmt += T_Ammo->GetItemCount();
			/*add to carried ammo*/
			MAmmoMap[T_Ammo->GetAmmoType()] = CarriedAmmoAmt;
			UE_LOG(LogTemp, Warning, TEXT("pickupammo function called"))
			UE_LOG(LogTemp, Warning, TEXT("carried ammo is :%d"), MStarting9MMAmmo)
		}

		if (MEquippedWeapon->GetAmmoType() == T_Ammo->GetAmmoType())
		{
			if (MEquippedWeapon->GetMWeaponAmmo() == 0)
			{
				ReloadWeapon();
			}
		}
	}

	T_Ammo->Destroy();
}

void AShooterCharacter::InitFInterpolationLoc()
{
	/*init struct for each interp scence comp*/
	FInterpolation WeaponInterpLoc{MWeaponInterpComp, 0};
	FInterpolation Ammo1InterpLoc{MInterpComp1, 0};
	FInterpolation Ammo2InterpLoc{MInterpComp2, 0};
	FInterpolation Ammo3InterpLoc{MInterpComp3, 0};
	FInterpolation Ammo4InterpLoc{MInterpComp4, 0};
	FInterpolation Ammo5InterpLoc{MInterpComp5, 0};
	FInterpolation Ammo6InterpLoc{MInterpComp6, 0};
	/*add to array*/
	MInterpLocationArray.Add(WeaponInterpLoc);
	MInterpLocationArray.Add(Ammo1InterpLoc);
	MInterpLocationArray.Add(Ammo2InterpLoc);
	MInterpLocationArray.Add(Ammo3InterpLoc);
	MInterpLocationArray.Add(Ammo4InterpLoc);
	MInterpLocationArray.Add(Ammo5InterpLoc);
	MInterpLocationArray.Add(Ammo6InterpLoc);
}

EPhysicalSurface AShooterCharacter::GetPhysicalSurfaceType()
{
	//implement line trace from actor location to below feet
	FHitResult FootResult;
	const FVector FootTraceStart{GetActorLocation()};
	FVector FootTraceEnd{FootTraceStart + FVector{0.f, 0.f, -400.f}};
	FCollisionQueryParams FootQueryParams;
	FootQueryParams.bReturnPhysicalMaterial = true;
	GetWorld()->LineTraceSingleByChannel(FootResult, FootTraceStart, FootTraceEnd, ECC_Visibility, FootQueryParams);

	//return the surface type from line trace
	return UPhysicalMaterial::DetermineSurfaceType(FootResult.PhysMaterial.Get());
}

int32 AShooterCharacter::FinterLowestIndex()
{
	/*lowest index starts at 1 because zero is for weapon*/
	int32 LowestInterpLocIndex{1};
	int32 LowestCount{INT_MAX};
	//loop through array from first ammo index
	for (int32 i = 1; i < MInterpLocationArray.Num(); ++i)
	{
		//init index and count with lowest figures   
		if (MInterpLocationArray[i].ItemCount < LowestCount)
		{
			LowestInterpLocIndex = i;
			LowestCount = MInterpLocationArray[i].ItemCount;
		}
	}
	return LowestInterpLocIndex;
}

void AShooterCharacter::IncreaseFInterpItemCount(int32 index, int32 Amount)
{
	/*if amount is more than one or less than -1 return*/
	if (Amount > 1 || Amount < -1)return;
	/*index must not surpass array size*/
	if (MInterpLocationArray.Num() > index)
	{
		MInterpLocationArray[index].ItemCount += Amount;
	}
}

void AShooterCharacter::HighlightIconsSlot()
{
	//get empty slot
	const int32 EmptySlot{GetEmptyWeaponSlot()};
	//trigger broadcast with empty slot
	MHighlightIconDelegate.Broadcast(EmptySlot, true);
	//init high_lighted_slot with empty slot
	MHighlightedSlot = EmptySlot;
}

void AShooterCharacter::UnHighlightIconSlot()
{
	//animation stops playing at the recent slot
	MHighlightIconDelegate.Broadcast(MHighlightedSlot, false);
	//init to default which means no slot is highlighted
	MHighlightedSlot = -1;
}

void AShooterCharacter::ResetPickupSoundTimer()
{
	MShouldPlayPickupSound = true;
}

void AShooterCharacter::ResetEquipSoundTimer()
{
	MShouldPlayEquipSound = true;
}

void AShooterCharacter::StartPickupSoundTimer()
{
	MShouldPlayPickupSound = false;
	GetWorldTimerManager().SetTimer(MPickUpSoundTimerHandle, this, &AShooterCharacter::ResetPickupSoundTimer,
	                                MPickupSoundResetTime);
}

void AShooterCharacter::StartEquipSoundTimer()
{
	MShouldPlayEquipSound = false;
	GetWorldTimerManager().SetTimer(MEquipSoundTimerHandle, this, &AShooterCharacter::ResetEquipSoundTimer,
	                                MEquipSoundResetTime);
}

void AShooterCharacter::PlayStunnedMontage() const
{   //dont stun when health is 0
	if(MHealth<=0.f)return;
	if (UShooterCharAnimInstance* ShooterInstance{(Cast<UShooterCharAnimInstance>(GetMesh()->GetAnimInstance()))};
		MStunnedMontage)
	{
		ShooterInstance->Montage_Play(MStunnedMontage);
		ShooterInstance->Montage_JumpToSection(FName{"Default"});
	}
}


void AShooterCharacter::MoveForward(float Value)
{
	if (GetController() && Value != 0.f)
	{
		/*Find Controller Forward Rotation*/
		const FRotator ControllerRotation{GetController()->GetControlRotation()};
		const FRotator ControllerYaw{0.f, ControllerRotation.Yaw, 0.f};
		const FVector ControllerForwardVector{FRotationMatrix{ControllerYaw}.GetUnitAxis(EAxis::X)};
		/*Add it to MovementInput*/
		AddMovementInput(ControllerForwardVector, Value);
	}
}

void AShooterCharacter::MoveRight(float Value)
{
	if (GetController() && Value != 0.f)
	{
		/*Find Controller Forward Rotation*/
		const FRotator ControllerRotation{GetController()->GetControlRotation()};
		const FRotator ControllerYaw{0.f, ControllerRotation.Yaw, 0.f};
		const FVector ControllerSideVector{FRotationMatrix{ControllerYaw}.GetUnitAxis(EAxis::Y)};
		/*Add it to MovementInput*/
		AddMovementInput(ControllerSideVector, Value);
	}
}

void AShooterCharacter::LookUpRate(float Value)
{
	if (GetWorld() && GetController())
	{
		AddControllerPitchInput(Value * MLookUpRateDegree * GetWorld()->GetDeltaSeconds());
	}
}

void AShooterCharacter::LookRightRate(float Value)
{
	if (GetWorld() && GetController())
	{
		AddControllerYawInput(Value * MLookRightRateDegree * GetWorld()->GetDeltaSeconds());
	}
}

void AShooterCharacter::InitializeViewPortSize(FVector2d& ViewPortSize)
{
	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewPortSize);
		ViewPortSize.X /= 2;
		ViewPortSize.Y /= 2;
	}
}

void AShooterCharacter::PlayHipFireMontage() const
{
	UShooterCharAnimInstance* ShooterAnimInstance{Cast<UShooterCharAnimInstance>(GetMesh()->GetAnimInstance())};
	if (ShooterAnimInstance && MHipFireMontage)
	{
		ShooterAnimInstance->Montage_Play(MHipFireMontage);
		ShooterAnimInstance->Montage_JumpToSection(FName{"FireStart"});
	}
}

void AShooterCharacter::PlayEquipRifleMontage()
{
	UShooterCharAnimInstance* ShooterAnimInst{(Cast<UShooterCharAnimInstance>(GetMesh()->GetAnimInstance()))};
	if (ShooterAnimInst && MEquipRifleMontage)
	{
		ShooterAnimInst->Montage_Play(MEquipRifleMontage);
		ShooterAnimInst->Montage_JumpToSection(FName{"Equip"});
		SetCombatState(ECombatState::ECS_Equipping);
	}
}

void AShooterCharacter::FinishEquipRifleMontage()
{
	SetCombatState(ECombatState::ECS_UnOccupied);
	if (MisAimButtonPressed)
	{
		Aim();
	}
}

bool AShooterCharacter::TempCrossHairTrace(FHitResult& OutHit, FVector& T_SmokeBeamEndPoint,
                                           const USkeletalMeshSocket* T_MuzzleSocket)
{
	/*get view port size*/
	FVector2d ViewPortSize;
	InitializeViewPortSize(ViewPortSize);
	/*if cross hair projected was successful its initialized to true*/
	bool ProjectionExecuted = false;
	FVector CrossHairStartLocation;
	FVector CrossHairEndLocation;
	/*get player controller and project cross-hair into screen*/
	if (const APlayerController* ShooterController{
		UGameplayStatics::GetPlayerController(this,
		                                      0)
	})
	{
		ProjectionExecuted = ProjectCrossHair(ViewPortSize, ShooterController,
		                                      CrossHairStartLocation, CrossHairEndLocation);
	}

	/*if projected success initialize cross hair TracEnd*/
	CrossHairEndLocation = CrossHairStartLocation + CrossHairEndLocation * 50'000;
	if (ProjectionExecuted)
	{
		CrossHairTrace(CrossHairStartLocation, CrossHairEndLocation, OutHit);
	}
	//if  Cross_Hair Has Hit 
	if (OutHit.IsValidBlockingHit())
	{
		//init smoke beam to blocked point
		T_SmokeBeamEndPoint = OutHit.ImpactPoint;
		/*init Weapon Trace from barrel and anything in_between muzzle and cross
		 *hair den set bullet beam end to its location*/
		InitGunBarrelTrace(GetWorld(), T_SmokeBeamEndPoint, T_MuzzleSocket);
		return true;
	}
	//init smoke beam to where Cross_Hair is Pointing to
	T_SmokeBeamEndPoint = CrossHairEndLocation;
	return false;
}

[[nodiscard]] AWeapon* AShooterCharacter::SetDefaultWeapon() const
{
	if (MDefaultWeaponClass)
	{
		AWeapon* Weapon{GetWorld()->SpawnActor<AWeapon>(MDefaultWeaponClass)};
		return Weapon;
	}

	return nullptr;
}

void AShooterCharacter::EquipWeapon(AWeapon* Weapon, bool T_bisSwapping)
{
	/*if weapon is valid*/
	if (Weapon)
	{
		/*make sphere and collision box ignore all collision*/
		Weapon->GetMBoxComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
		Weapon->GetOverLappingSphereComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
		/*if skeletal socket is not null attach weapon to it*/
		if (const USkeletalMeshSocket* ShooterMeshSocket{GetMesh()->GetSocketByName(FName{"RightHandSocket"})})
		{   //no equipped weapon
			if (!MEquippedWeapon)
			{
				MEquipItemDelegate.Broadcast(-1, Weapon->GetSlotIndex());
			}
			else if (!T_bisSwapping)
			{
				MEquipItemDelegate.Broadcast(MEquippedWeapon->GetSlotIndex(), Weapon->GetSlotIndex());
			}
            
			ShooterMeshSocket->AttachActor(Weapon, GetMesh());
			MEquippedWeapon = Weapon;
			MEquippedWeapon->SetOwner(this);    
			
		}


		if (MEquippedWeapon)
		{
			MEquippedWeapon->SetItemState(EItemState::EIS_Equipped);
			const AShooterController* ShooterController{(Cast<AShooterController>(GetController()))};
			if (ShooterController&&MEquippedWeapon)
			{
				ShooterController->SetAmmoWidgetWeapon(MEquippedWeapon);
			}
			MEquippedWeapon->DisableGlowMaterial();
			MEquippedWeapon->DisableCustomDepth();
			/*adding default weapon to inventory*/
			//only when the inventory is not filled up
			if(MItemInventory.Max()<=MInventoryCapacity-1)
			    MItemInventory.Add(MEquippedWeapon);
		}
	}
}

void AShooterCharacter::DropWeapon()
{
	if (MEquippedWeapon)
	{
		const FDetachmentTransformRules Detachment(EDetachmentRule::KeepWorld, true);
		MEquippedWeapon->DetachFromActor(Detachment);
		MEquippedWeapon->SetItemState(EItemState::EIS_Falling);
		MEquippedWeapon->ThrowWeapon();
	}
}

void AShooterCharacter::SwapWeapon(AWeapon* WeaponToSwap)
{
	//check if null
	if (WeaponToSwap)
	{
		//if inventory is not full swap equipped weapon slot with weapon to swap
		if (MItemInventory.Num() - 1 > MEquippedWeapon->GetSlotIndex())
		{
			MItemInventory[MEquippedWeapon->GetSlotIndex()] = WeaponToSwap;
			WeaponToSwap->SetSlotIndex(MEquippedWeapon->GetSlotIndex());
		}

		//swap weapons
		DropWeapon();
		EquipWeapon(WeaponToSwap, true);
	}
	//set to nullptr to avoid swap glitch.
	MLastTraceItem = nullptr;
}

void AShooterCharacter::ReloadWeapon()
{
	/*if not unoccupied and no equipped weapon*/
	if (MCombatState != ECombatState::ECS_UnOccupied)
	{
		return;
	}
	if (!MEquippedWeapon)
	{
		return;
	}
	if (IsCarryingAmmo() && !MEquippedWeapon->MagazineClipFull())
	{
		if (MisAimButtonPressed)
			StopAiming();

		SetCombatState(ECombatState::ECS_Reloading);
		if (UShooterCharAnimInstance* ShooterInstance{(Cast<UShooterCharAnimInstance>(GetMesh()->GetAnimInstance()))};
			ShooterInstance && MReloadMontage)
		{
			ShooterInstance->Montage_Play(MReloadMontage);
			ShooterInstance->Montage_JumpToSection(MEquippedWeapon->GetReloadSection());
		}
	}
}

void AShooterCharacter::SelectButtonPressed()
{
	if (MCombatState != ECombatState::ECS_UnOccupied)return;
	//check if valid and cast to a weapon
	if (MLastTraceItem)
	{
		MLastTraceItem->StartItemCurve(this, true);
		MLastTraceItem = nullptr;
	}
}

void AShooterCharacter::SelectButtonReleased()
{
}

void AShooterCharacter::InitiShooterWalkCrouchSpeed()
{
	if (MisCrouching)
	{
		GetCharacterMovement()->MaxWalkSpeed = MCrouchWalkSpeed;
	}

	else
	{
		GetCharacterMovement()->MaxWalkSpeed = MBaseWalkSpeed;
	}
}

void AShooterCharacter::InterpolateCapsuleHeight(float DeltaTime)
{
	if (MisCrouching)
	{
		MCurrentCapsuleHalfHeight = MCrouchCapsuleHeight;
	}
	else
	{
		MCurrentCapsuleHalfHeight = MStandingCapsuleHalfHeight;
	}
	if (GetCapsuleComponent())
	{
		const float InterpolatedCapsuleHeight{
			FMath::FInterpTo(GetCapsuleComponent()->GetScaledCapsuleHalfHeight(),
			                 MCurrentCapsuleHalfHeight, DeltaTime, 30.f)
		};
		/*if standing->crouching the delta capsule half height will be negative
		 *if crouching the delta standing the delta capsule half height will be positive*/
		const float DeltaHalfHeight{InterpolatedCapsuleHeight - GetCapsuleComponent()->GetScaledCapsuleHalfHeight()};

		const FVector MeshOffSet{0.f, 0.f, -DeltaHalfHeight};

		/*set capsule height*/
		GetCapsuleComponent()->SetCapsuleHalfHeight(InterpolatedCapsuleHeight);
		GetMesh()->AddLocalOffset(MeshOffSet);
	}
}

void AShooterCharacter::CrouchButtonPressed()
{
	if (!GetCharacterMovement()->IsFalling())
	{
		MisCrouching = !MisCrouching;
	}
	/*set walk speed to crouch speed if crouching
	 * and back to default after crouching
	 */
	InitiShooterWalkCrouchSpeed();
	if (MisCrouching)
	{
		GetCharacterMovement()->GroundFriction = MCrouchFrictionAmt;
	}
	else
	{
		GetCharacterMovement()->GroundFriction = MStandFrictionAmt;
	}
}

void AShooterCharacter::ReloadButtonPressed()
{
	if (MCombatState == ECombatState::ECS_Stunned)return;
	ReloadWeapon();
}


void AShooterCharacter::PlayGunSound() const
{
	if (!MEquippedWeapon)return;
	if (MEquippedWeapon->GetWeaponFireSound())
	{
		UGameplayStatics::PlaySoundAtLocation(this, MEquippedWeapon->GetWeaponFireSound(),
		                                      GetActorLocation(), GetActorRotation());
	}
}

void AShooterCharacter::EmitMuzzleFlash(const USkeletalMeshSocket*& ShooterMuzzleSocket) const
{
	if (!MEquippedWeapon)return;
	if (MEquippedWeapon->GetMuzzleFlash() && ShooterMuzzleSocket)
	{
		/*plays emitter at muzzle location*/
		if (MEquippedWeapon)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MEquippedWeapon->GetMuzzleFlash(),
			                                         ShooterMuzzleSocket->GetSocketTransform(
				                                         MEquippedWeapon->GetMItemMesh()));
		}
	}
}


void AShooterCharacter::InitGunBarrelTrace(const UWorld* World,
                                           FVector& BulletBeamEnd,
                                           const USkeletalMeshSocket* ShooterMuzzleSocket) const
{
	if (ShooterMuzzleSocket)
	{
		const FVector WeaponBarrelStart{ShooterMuzzleSocket->GetSocketLocation(GetMesh())};
		/*end trace is crossHair End trace
		 */
		FHitResult WeaponHitResult;
		/*getting vector from weapon muzzle to beam end*/
		FVector WeaponStartToEnd{BulletBeamEnd - WeaponBarrelStart};
		/*init weapon end vector to stretch a little further frm beam hit to endeavour hit*/
		const FVector WeaponBarrelEnd{WeaponBarrelStart + WeaponStartToEnd * 1.25};
		World->LineTraceSingleByChannel(WeaponHitResult, WeaponBarrelStart, WeaponBarrelEnd,
		                                ECC_Visibility);

		/*if theres an object between weapon barrel and trace channel initialize bullet beam to location*/
		if (WeaponHitResult.IsValidBlockingHit())
		{
			//init bullet beam with muzzle trace vector impact
			BulletBeamEnd = WeaponHitResult.ImpactPoint;
		}
	}
}

UParticleSystemComponent* AShooterCharacter::SpawnSmokeBeam(const USkeletalMeshSocket* ShooterMuzzleSocket) const
{
	return UGameplayStatics::SpawnEmitterAtLocation(this,
	                                                MBulletBeamFx,
	                                                ShooterMuzzleSocket->GetSocketLocation(
		                                                MEquippedWeapon->GetMItemMesh()));
}

void AShooterCharacter::EmitBulletImpactParticle(const FVector& BulletBeamEnd) const
{
	UGameplayStatics::SpawnEmitterAtLocation(this, MBulletImpactFx,
	                                         BulletBeamEnd);
}

void AShooterCharacter::SmokeBeamTrace(const FVector& BulletBeamEnd,
                                       const USkeletalMeshSocket* ShooterMuzzleSocket) const
{
	UParticleSystemComponent* Beam{(SpawnSmokeBeam(ShooterMuzzleSocket))};
	Beam->SetVectorParameter(FName{"Target"}, BulletBeamEnd);
}

void AShooterCharacter::EmitBeam_BulletFX(const FVector& BulletBeamEnd,
                                          const USkeletalMeshSocket* ShooterMuzzleSocket) const
{
	if (MBulletBeamFx && MBulletImpactFx)
	{
		/*spawn beam at muzzle socket location*/

		SmokeBeamTrace(BulletBeamEnd, ShooterMuzzleSocket);
		//spawn only when bullet hit staic mesh
		if (!MIsBulletValidHit)
			EmitBulletImpactParticle(BulletBeamEnd);
	}
}

bool AShooterCharacter::ProjectCrossHair(const FVector2d& CrossHairScreenLocation,
                                         const APlayerController* ShooterController, FVector& CrossHairStartLocation,
                                         FVector& CrossHairEndLocation)
{
	return UGameplayStatics::DeprojectScreenToWorld(ShooterController, CrossHairScreenLocation,
	                                                CrossHairStartLocation,
	                                                CrossHairEndLocation);
}

void AShooterCharacter::CrossHairTrace(const FVector& CrossHairStartLocation, const FVector& CrossHairEndLocation,
                                       FHitResult& CrossHairTraceHitResult) const
{
	GetWorld()->LineTraceSingleByChannel(CrossHairTraceHitResult, CrossHairStartLocation,
	                                     CrossHairEndLocation,
	                                     ECC_Visibility);
}

UDecalComponent* AShooterCharacter::GetBulletDecal(FHitResult CrossHairTraceHitResult, FRotator DecalRo) const
{
	return UGameplayStatics::SpawnDecalAtLocation(
		this, MEquippedWeapon->GetBulletDecal(),
		FVector(FMath::RandRange(8, 12), FMath::RandRange(8, 12), FMath::RandRange(8, 12)),
		CrossHairTraceHitResult.ImpactPoint,
		FRotator(DecalRo.Pitch, DecalRo.Yaw, FMath::RandRange(-180, 180)), 10.f);
}

void AShooterCharacter::ReleaseBullet(const USkeletalMeshSocket*& ShooterMuzzleSocket)
{
	if (!MEquippedWeapon)return;

	FHitResult CrossHairTraceHitResult;
	//Bullet/smokeBeamEndPoint
	FVector BulletBeamEnd;
	if (TempCrossHairTrace(CrossHairTraceHitResult, BulletBeamEnd, ShooterMuzzleSocket))
	{
		//if actor hit by trace has bullet hit interface
		if (CrossHairTraceHitResult.GetActor())
		{
			//check if  enemy has  interface function
			if (const IBulletHitInterface* ActorBulletHitInterface{
				Cast<IBulletHitInterface>(CrossHairTraceHitResult.GetActor())
			})
			{
				//bullet impacted object with interface
				MIsBulletValidHit = true;
				ActorBulletHitInterface->Execute_BulletHit(CrossHairTraceHitResult.GetActor(), CrossHairTraceHitResult,this,GetController());
				//init bullet beam end to enemy impact location

				if (AEnemy* Enemy{Cast<AEnemy>(CrossHairTraceHitResult.GetActor())})
				{
					if (CrossHairTraceHitResult.BoneName == Enemy->GetHeadBoneName())
					{
						//Head Shot Damage
						UGameplayStatics::ApplyDamage(Enemy, MEquippedWeapon->GetWeaponHeadShotDmgAmount(),
						                              GetController(), this, UDamageType::StaticClass());
						Enemy->ShowBulletHitNumber(MEquippedWeapon->GetWeaponHeadShotDmgAmount(),
						                           CrossHairTraceHitResult.ImpactPoint, true);
					}
					else
					{
						//Body shot
						if (MEquippedWeapon)
						{
							UGameplayStatics::ApplyDamage(Enemy, MEquippedWeapon->GetWeaponDamageAmount(),
							                              GetController(),
							                              this, UDamageType::StaticClass());
							Enemy->ShowBulletHitNumber(MEquippedWeapon->GetWeaponDamageAmount(),
							                           CrossHairTraceHitResult.ImpactPoint, false);
						}
					}
				}
			}
			else
			{
				//bullet met a static mesh
				MIsBulletValidHit = false;
				if (MEquippedWeapon && MEquippedWeapon->GetWeaponBulletFlyBy())
					UGameplayStatics::PlaySoundAtLocation(this, MEquippedWeapon->GetWeaponBulletFlyBy(), BulletBeamEnd);

				//DecalRotation trace normal initial point
				const FRotator DecalRotation{(CrossHairTraceHitResult.Normal * -1).Rotation()};
				// if there a decal material
				if (MEquippedWeapon->GetBulletDecal())
				{
					//spawn decal and hold address
					UDecalComponent* Decal = GetBulletDecal(CrossHairTraceHitResult, DecalRotation);

					const FAttachmentTransformRules AttachmentRules{FAttachmentTransformRules::KeepWorldTransform};
					//
					if (Decal)
						Decal->AttachToComponent(CrossHairTraceHitResult.GetActor()->GetRootComponent(),
						                         AttachmentRules);


					UE_LOG(LogTemp, Warning, TEXT("decalspawn"))
				}
			}
		}

		/*check for beam and bullet fx and emit*/
		/*emit fx at hit location*/
		EmitBeam_BulletFX(BulletBeamEnd, ShooterMuzzleSocket);
		/*keeps check of cross hair expansion and shrinking*/
		StartCrossHairFiringTimer();
	}
}

void AShooterCharacter::InitializeMuzzleSocket([[maybe_unused]] const USkeletalMeshSocket*& ShooterMuzzleSocket)
{
	if (MEquippedWeapon)
	{
		ShooterMuzzleSocket = MEquippedWeapon->GetMItemMesh()->GetSocketByName(FName{"MuzzleSocket"});
	}
}

void AShooterCharacter::FireWeapon()
{
	//if no weapon cnt fire
	if (!MEquippedWeapon)
	{
		return;
	}
	//if shooter character is not unoccupied actor cant fire
	if (MCombatState != ECombatState::ECS_UnOccupied)
	{
		return;
	}

	/*plays the hipFire animMontage*/
	PlayHipFireMontage();

	if (IsWeaponHavingAmmo())
	{
		/*play gun sound and emit muzzle flash despite not hit*/
		PlayGunSound();

		const USkeletalMeshSocket* ShooterMuzzleSocket{nullptr};
		InitializeMuzzleSocket(ShooterMuzzleSocket);

		EmitMuzzleFlash(ShooterMuzzleSocket);

		ReleaseBullet(ShooterMuzzleSocket);
		//reduce ammo
		if (MEquippedWeapon)
		{
			MEquippedWeapon->DecreaseAmmo();
		}

		StartAutomaticFireTimer();

		if (MEquippedWeapon && MEquippedWeapon->GetWeaponType() == EWeaponType::EWT_Pistol)
		{
			//start PistolSlideDisplacement
			MEquippedWeapon->StartPistolSlideTimer();
		}
	}
	else
	{
		ReloadWeapon();
	}


	UE_LOG(LogTemp, Warning, TEXT("FiredGun"))
}

void AShooterCharacter::Aim()
{
	MisZoomedAim = true;
	/*speed should not be fast when aiming*/
	GetCharacterMovement()->MaxWalkSpeed = MCrouchWalkSpeed;
}

void AShooterCharacter::AimButtonPressed()
{
	MisAimButtonPressed = true;
	if (MCombatState != ECombatState::ECS_Reloading && MCombatState != ECombatState::ECS_Equipping && MCombatState !=
		ECombatState::ECS_Stunned)
		Aim();
}

void AShooterCharacter::StopAiming()
{
	MisZoomedAim = false;
	if (!MisCrouching)
		GetCharacterMovement()->MaxWalkSpeed = MBaseWalkSpeed;
}

void AShooterCharacter::AimButtonReleased()
{
	MisAimButtonPressed = false;
	StopAiming();
}

void AShooterCharacter::InterpolationWeaponAim(float DeltaTime)
{
	/*check if weapon is zoomed in*/
	if (MisZoomedAim && GetMCameraComponent())
	{
		/*interpolate to the zoomed in camera view*/
		MCurrentCamFov = FMath::FInterpTo(MCurrentCamFov, MCameraZoomedFov, DeltaTime, MAimZoomInterpolationSpeed);
		GetMCameraComponent()->SetFieldOfView(MCurrentCamFov);
	}
	else
	{
		/*interpolate out of the zoomed in view back to the default camera view*/
		MCurrentCamFov = FMath::FInterpTo(MCurrentCamFov, MDefaultCameraFov, DeltaTime, MAimZoomInterpolationSpeed);
		GetMCameraComponent()->SetFieldOfView(MCurrentCamFov);
	}
}

void AShooterCharacter::SetLookSensitivity()
{
	if (MisZoomedAim)
	{
		SetMLookUpRateDegree(MAimingLookUpRateDegree);
		SetMLookRightRateDegree(MAimingLookRightRateDegree);
	}
	else
	{
		SetMLookUpRateDegree(MHipLookUpRateDegree);
		SetMLookRightRateDegree(MHipLookRightRateDegree);
	}
}

void AShooterCharacter::FKeyPressed()
{
	if (MEquippedWeapon->GetSlotIndex() == 0)return;

	SwitchInventoryWeapon(MEquippedWeapon->GetSlotIndex(), 0);
}

void AShooterCharacter::OneKeyPressed()
{
	if (MEquippedWeapon->GetSlotIndex() ==1)return;
	SwitchInventoryWeapon(MEquippedWeapon->GetSlotIndex(), 1);
}

void AShooterCharacter::TwoKeyPressed()
{
	if (MEquippedWeapon->GetSlotIndex() == 2)return;
	SwitchInventoryWeapon(MEquippedWeapon->GetSlotIndex(), 2);
}

void AShooterCharacter::SwitchInventoryWeapon(int32 T_CurrentSlotIndex, int32 T_NextSlotIndex)
{
	if ((T_CurrentSlotIndex == T_NextSlotIndex) || (T_NextSlotIndex >=MItemInventory.Num()) || (MCombatState !=
		ECombatState::ECS_UnOccupied))
		return;

	//check if shooter char is aiming
	if (MisZoomedAim)
	{
		StopAiming();
	}
	const auto OldWeapon{MEquippedWeapon};
	const auto LatestWeapon{Cast<AWeapon>(MItemInventory[T_NextSlotIndex])};
	if (LatestWeapon)
	{
		EquipWeapon(LatestWeapon, false);
		/*LatestWeapon->SetItemState(EItemState::EIS_Equipped);*/
		if (OldWeapon->GetItemState() != EItemState::EIS_PickedUp)
			OldWeapon->SetItemState(EItemState::EIS_PickedUp);
		
		PlayEquipRifleMontage();
		LatestWeapon->PlayEquipSound(true);
	}
}

void AShooterCharacter::CalculateCrossHairSpread(float DeltaTime)
{
	/*getting simultaneous range for run speed range and CrossHair Velocity Range
	 */
	const FVector2d WalkRunRange{0.f, 600.f};
	const FVector2d CrossHairVelocityRange{0.f, 1.f};
	FVector ShooterCharVelocity{GetVelocity()};
	ShooterCharVelocity.Z = 0.f;
	MCrossHairVelocity = FMath::GetMappedRangeValueClamped(WalkRunRange,
	                                                       CrossHairVelocityRange,
	                                                       /*check if shooter character is in air*/
	                                                       ShooterCharVelocity.Size());
	if (GetCharacterMovement()->IsFalling())
	{
		/*init CrossHair Air Component with interpolated value*/
		MCrossAirComponent = FMath::FInterpTo(MCrossAirComponent, 5.f, DeltaTime, 5.f);
	}
	else
	{
		MCrossAirComponent = FMath::FInterpTo(MCrossAirComponent, 0.f, DeltaTime, 20.f);
	}
	/*if shooter char is aiming*/
	if (MisZoomedAim)
	{
		/*shrink crossHair interpolate*/
		MCrossHairAimComponent = FMath::FInterpTo(MCrossAirComponent, -5.f, DeltaTime, 5.f);
	}
	else
	{
		/*return to default value*/
		MCrossHairAimComponent = FMath::FInterpTo(MCrossAirComponent, 0.f, DeltaTime, 5.f);
	}
	/*check if firing*/
	if (MisCrossHairFiring)
	{
		/*expand cross hair size by Target size*/
		MCrossHairShootingComponent = FMath::FInterpTo(MCrossAirComponent, 0.5f, DeltaTime, 60.f);
	}
	else
	{
		/*shrink crossHair to default size*/
		MCrossHairShootingComponent = FMath::FInterpTo(MCrossHairAimComponent, 0.f, DeltaTime, 60.f);
	}


	/*all interpolated Component are added to CrossHairMultiplier
	 */
	MCrossHairMultiplier = 0.5f + MCrossHairVelocity + MCrossAirComponent + MCrossHairAimComponent +
		MCrossHairShootingComponent;
}

void AShooterCharacter::StartCrossHairFiringTimer()
{
	/*if shooter char is firing*/
	MisCrossHairFiring = true;
	/*start timer to for cross hair to return to initial position after shots*/
	GetWorldTimerManager().SetTimer(MCrossHairTimerHandle,
	                                this, &AShooterCharacter::EndCrossHairFiringTimer,
	                                MCrossHairRecoilTime);
}

void AShooterCharacter::EndCrossHairFiringTimer()
{
	MisCrossHairFiring = false;
}

void AShooterCharacter::PressFireButton()
{
	if (MCombatState == ECombatState::ECS_Stunned)return;
	MisfireButtonPressed = true;
	FireWeapon();
}

void AShooterCharacter::ReleaseFireButton()
{
	MisfireButtonPressed = false;
}

void AShooterCharacter::StartAutomaticFireTimer()
{
	if (!MEquippedWeapon)return;
	SetCombatState(ECombatState::ECS_Firing);
	/*time manager that keeps check of trigger rate pull time*/
	GetWorldTimerManager().SetTimer(MAutoFireTimeHandle, this, &AShooterCharacter::AutoResetFiringTimer,
	                                MEquippedWeapon->GetWeaponFireRate());
}

void AShooterCharacter::AutoResetFiringTimer()
{
	SetCombatState(ECombatState::ECS_UnOccupied);

	if (MisfireButtonPressed)
	{
		if (!MEquippedWeapon)return;
		if (IsWeaponHavingAmmo() && MEquippedWeapon->GetIsAutomaticFire())
		{
			FireWeapon();
		}

		else if (!IsWeaponHavingAmmo())
		{
			ReloadWeapon();
		}
	}
}

void AShooterCharacter::InitializeAmmoMap()
{
	MAmmoMap.Add(EAmmoType::EAT_9mmAmmo, MStarting9MMAmmo);
	MAmmoMap.Add(EAmmoType::EAT_AssaultRifleAmmo, MStartingAssaultAmmo);
}

bool AShooterCharacter::IsWeaponHavingAmmo() const
{
	//if equipped weapon and weapon has ammo
	if (MEquippedWeapon && MEquippedWeapon->GetMWeaponAmmo() > 0)
	{
		return true;
	}
	//weapon dont have ammo
	return false;
}

bool AShooterCharacter::IsCarryingAmmo() const
{
	//if shooter is unoccupied and has an equipped weapon
	if (MCombatState == ECombatState::ECS_UnOccupied && MEquippedWeapon)
	{
		//check ammo array for weapon ammo type
		for (const auto Ammo : MAmmoMap)
		{
			//if theres an ammo type of equipped weapon
			//and if ammo value is above zero
			if (Ammo.Key == MEquippedWeapon->GetAmmoType())
			{
				return Ammo.Value > 0;
			}
		}
	}
	return false;
}

void AShooterCharacter::GrabClip()
{
	if (MEquippedWeapon)
	{
		//get weapon clip index.  
		const int32 WeaponClipIndex{
			MEquippedWeapon->GetMItemMesh()->GetBoneIndex(MEquippedWeapon->GetWeaponClipBoneName())
		};
		//get clip transform
		const FTransform ClipTransform{MEquippedWeapon->GetMItemMesh()->GetBoneTransform(WeaponClipIndex)};
		const FAttachmentTransformRules AttachmentRules{EAttachmentRule::KeepRelative, true};
		if (MHandClipComponent)
		{
			MHandClipComponent->AttachToComponent(GetMesh(), AttachmentRules, FName{TEXT("LeftHandSocket")});
			MHandClipComponent->SetWorldTransform(ClipTransform);
		}
		MEquippedWeapon->SetIsMovingClip(true);
		UE_LOG(LogTemp, Warning, TEXT("GrabclipFunctionCalled"))
	}
}

void AShooterCharacter::ReturnClip()
{
	if (MEquippedWeapon)
	{
		MEquippedWeapon->SetIsMovingClip(false);
	}

	UE_LOG(LogTemp, Warning, TEXT("ReleaseclipFunctionCalled"))
}

float AShooterCharacter::GetMCrossHairMultiplier() const
{
	return MCrossHairMultiplier;
}

FInterpolation AShooterCharacter::GetFInterLocation(int32 index)
{
	if (index < MInterpLocationArray.Num())
	{
		return MInterpLocationArray[index];
	}
	return FInterpolation();
}


void AShooterCharacter::CheckOverlappingItem(bool IsOverlappingItem)
{
	/*if shooter char is overlapping with item init @Property:MisOverlapping to true*/
	if (IsOverlappingItem)
	{
		MisOverLappingItem = true;
	}


	else
	{
		MisOverLappingItem = false;
	}
}

void AShooterCharacter::CheckcharInventory()
{
	if (MItemInventory.Num() >= MInventoryCapacity)
	{
		MLastTraceItem->SetisCharInevtoryFull(true);
	}
	else
	{
		MLastTraceItem->SetisCharInevtoryFull(false);
	}
}

void AShooterCharacter::CheckForItem()
{
	FHitResult TemptHitResult;
	/*if shooter char overlapping*/
	if (MisOverLappingItem)
	{
		//not used
		[[maybe_unused]] FVector Vector{FVector::ZeroVector};
		/*if cross hair trace succeed*/
		if (TempCrossHairTrace(TemptHitResult, Vector, nullptr))
		{
			/*check if traced actor is of type item*/
			if (AItem* ShooterITem{Cast<AItem>(TemptHitResult.GetActor())})
			{
				/*init @Property last trace item to item actor*/
				MLastTraceItem = ShooterITem;
				//if actor is a weapon
				if ([[maybe_unused]] AWeapon* Weapon{Cast<AWeapon>(MLastTraceItem)})
				{
					//if a slot is not currently highlighted
					if (MHighlightedSlot == -1)
						HighlightIconsSlot(); //highlight slot
				}


				if (MLastTraceItem && MLastTraceItem->GetPopUpWidgetComponent())
				{
					/*set visible*/
					if (MisOverLappingItem)
					{
						MLastTraceItem->GetPopUpWidgetComponent()->SetVisibility(true);
						MLastTraceItem->EnableCustomDepth();
						CheckcharInventory();
					}
				}
			}
		}
	}
	/*if shooter char isn't overlapping*/
	else
	{
		//check if its valid first
		if (MLastTraceItem)
		{
			//set pop up widget to not visible
			MLastTraceItem->InitializeWidgetInvisibility();
			MLastTraceItem->DisableCustomDepth();
			//if a slot was previously highlighted deactivate it
			if (MHighlightedSlot != -1)UnHighlightIconSlot();
		}


		//make null as we outside the sphere
		MLastTraceItem = nullptr;
	}
}


//function no longer used
/*FVector AShooterCharacter::GetCamInterpolationLocation()
{
	//get cam forward vector
	const FVector CameraForward{GetMCameraComponent()->GetForwardVector()};
	//get camera location
	const FVector CameraLocation{GetMCameraComponent()->GetComponentLocation()};
	//location weapon will interpolate to.
	const FVector CamInterpolationLocation{
		CameraLocation + CameraForward * MCamWeapInterpolationDistance + FVector(0.f, 0.f, MCamWeapInterpolationHeight)
	};
	return CamInterpolationLocation;
}*/

void AShooterCharacter::GetPickUpItem(AItem* ItemToPickUp)
{
	if (ItemToPickUp)
	{
		if (AWeapon* Weapon{Cast<AWeapon>(ItemToPickUp)})
		{
			if (MItemInventory.Num() < MInventoryCapacity)//new change
			{
				//add weapon to inventory
				MItemInventory.Add(Weapon);
				//set new weapon slot index to added inventory element index
				Weapon->SetSlotIndex(MItemInventory.Num() - 1);
				UE_LOG(LogTemp, Warning, TEXT("weapons slot %d"), Weapon->GetSlotIndex())
				UE_LOG(LogTemp, Warning, TEXT("inventorynumber : %d"), MItemInventory.Num());

				Weapon->SetItemState(EItemState::EIS_PickedUp);
			}

			else //inventory is full swap weapon with default weapon
			{
				SwapWeapon(Weapon);
			}
		}
		if (AAmmo* Ammo{Cast<AAmmo>(ItemToPickUp)})
		{
			PickUpAmmo(Ammo);
		}
	}
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	/*interpolates between weapon zoom in and out*/
	InterpolationWeaponAim(DeltaTime);
	/*set look sensitivity when aiming and not aiming*/
	SetLookSensitivity();
	/*handles all CrossHair Shrink and Expansion*/
	CalculateCrossHairSpread(DeltaTime);
	/*reveals items details if shooter char is close enough*/
	CheckForItem();

	InterpolateCapsuleHeight(DeltaTime);

	if (MShooterController->GetHealthWidget())
		MShooterController->GetHealthWidget()->SetHealthProgressBar(MHealth / MMaxHealth);
}

void AShooterCharacter::Jump()
{
	if (MisCrouching)
	{
		MisCrouching = false;
	}

	else
	{
		ACharacter::Jump();
	}
}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent)
	PlayerInputComponent->BindAxis(FName{"MoveForward"}, this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis(FName{"MoveRight"}, this, &AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis(FName{"LookUp"}, this, &AShooterCharacter::LookUpRate);
	PlayerInputComponent->BindAxis(FName{"LookRight"}, this, &AShooterCharacter::LookRightRate);
	PlayerInputComponent->BindAction(FName{"Jump"}, IE_Pressed, this, &AShooterCharacter::Jump);
	PlayerInputComponent->BindAction(FName{"Jump"}, IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction(FName{"FireButton"}, IE_Pressed, this,
	                                 &AShooterCharacter::PressFireButton);
	PlayerInputComponent->BindAction(FName{"FireButton"}, IE_Released, this,
	                                 &AShooterCharacter::ReleaseFireButton);
	PlayerInputComponent->BindAction(FName{"Aim"}, IE_Pressed, this,
	                                 &AShooterCharacter::AimButtonPressed);
	PlayerInputComponent->BindAction(FName{"Aim"}, IE_Released, this,
	                                 &AShooterCharacter::AimButtonReleased);
	PlayerInputComponent->BindAction(FName{"Select"}, IE_Pressed, this,
	                                 &AShooterCharacter::SelectButtonPressed);
	PlayerInputComponent->BindAction(FName{"Select"}, IE_Released, this,
	                                 &AShooterCharacter::SelectButtonReleased);
	PlayerInputComponent->BindAction(FName{"Reload"}, IE_Pressed, this,
	                                 &AShooterCharacter::ReloadButtonPressed);

	PlayerInputComponent->BindAction(FName{"Crouch"}, IE_Pressed, this,
	                                 &AShooterCharacter::CrouchButtonPressed);

	PlayerInputComponent->BindAction(FName{"FKey"}, IE_Pressed, this, &AShooterCharacter::FKeyPressed);

	PlayerInputComponent->BindAction(FName{"1Key"}, IE_Pressed, this, &AShooterCharacter::OneKeyPressed);

	PlayerInputComponent->BindAction(FName{"2Key"}, IE_Pressed, this, &AShooterCharacter::TwoKeyPressed);
}
