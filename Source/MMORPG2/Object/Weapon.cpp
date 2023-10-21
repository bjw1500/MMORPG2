// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Weapon.h"
#include "MyItem.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Network/Protocol.pb.h"
#include "Network/GameManager.h"
#include "Managers/DataTableManager.h"
#include "GameStruct.h"
#include "Creature.h"
#include "RPGPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"
#include "Camera/CameraShakeBase.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponCollision = CreateDefaultSubobject<UCapsuleComponent>("WeaponCollision");
	WeaponCollision->SetupAttachment(ItemMeshComponent);
	WeaponCollision->SetRelativeLocation(FVector(0, 0, 40.f));
	WeaponCollision->SetCapsuleSize(22, 66);
	WeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	WeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnWeaponOverlap);
	WeaponCollision->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnWeaponOverlapEnd);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

}

void AWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AWeapon::OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//아직 무기 주인이 없다면,
	if(IsValid(Master) == false)
		return;
	if (Master->ThisMasterOtherClient == true)
		return;

	if (IsValid(OtherActor) == true)
	{
		if (OtherActor == Master)
			return;
		if (OtherComp->GetFName() != FName(TEXT("HitBoxCollision")))
			return;

		FString name = OtherActor->GetName();
		//Utils::DebugLog(name);
		Protocol::Stat infoStat = Master->GetInfo()->stat();
		FDamageEvent damageEvent;
		UGameplayStatics::ApplyDamage(OtherActor, Master->Stat->GetDamage(), Master->GetController(), Master, nullptr);

		//OtherActor->TakeDamage(Master->Stat->GetDamage(), damageEvent, Master->GetController(), Master);
		WeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			if(IsValid(Master->SoundCue) == true)
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), Master->SoundCue, Master->GetActorLocation());
			if (IsValid(Master->AttackImpact) == true)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Master->AttackImpact, OtherActor->GetActorLocation());
			}
			if (IsValid(Master->ShakeClass) == true && Master->GetController() == GetWorld()->GetFirstPlayerController())
			{
				GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StartCameraShake(Master->ShakeClass, 1.0f, ECameraShakePlaySpace::CameraLocal, FRotator(0.f));
			}
	}
}

void AWeapon::OnWeaponOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}
