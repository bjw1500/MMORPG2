// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/NPC.h"
#include "Object/RPGPlayer.h"
#include "Network/GameManager.h"
#include "Managers/UIManager.h"
#include "UI/ShopItemList.h"
#include "UI/NPCUI.h"

// Sets default values
ANPC::ANPC()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InteractCollision = CreateDefaultSubobject<USphereComponent>("InteractCollision");
	InteractCollision->SetupAttachment(GetMesh());
	InteractCollision->OnComponentBeginOverlap.AddDynamic(this, &ANPC::OnInteractSphereOverlap);
	InteractCollision->OnComponentEndOverlap.AddDynamic(this, &ANPC::OnInteractSphereOverlapEnd);
}

// Called when the game starts or when spawned
void ANPC::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ANPC::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ANPC::OnInteractSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ARPGPlayer* player = Cast<ARPGPlayer>(OtherActor);
	if (IsValid(player) == false)
		return;

	player->CanInteractNPC = this;
}

void ANPC::OnInteractSphereOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ARPGPlayer* player = Cast<ARPGPlayer>(OtherActor);
	if (IsValid(player) == false)
		return;
	player->CanInteractNPC = nullptr;
	if (player->ThisMasterOtherClient == false)
	{
		if (IsValid(GameInstance->GetUIManager()->PopUI) == true)
		{
			GameInstance->GetUIManager()->PopUI->RemoveFromViewport();
			GameInstance->GetUIManager()->PopUI = nullptr;
			return;
		}
	}

	//GameInstance->GetUIManager()->GetMainUI()->SetVisibility(ESlateVisibility::Visible);
}

void ANPC::InteractPlayer()
{
	Utils::DebugLog(TEXT("Interact Player"));

	//GameInstance->GetUIManager()->GetMainUI()->SetVisibility(ESlateVisibility::Hidden);
	//이미 창이 열려 있다면 닫아주기
	if (IsValid(GameInstance->GetUIManager()->PopUI) == true)
	{
		GameInstance->GetUIManager()->PopUI->RemoveFromViewport();
		GameInstance->GetUIManager()->PopUI = nullptr;
		return;
	}
	//TODO 나중에는 상인들마다 각자 다른 ShopList를 건네주게 해주기
	if (IsValid(NPCUIClass) == false)
		return;

	UNPCUI* npcUI = CreateWidget<UNPCUI>(GetWorld(), NPCUIClass);
	GameInstance->GetUIManager()->PopUI = npcUI;
	npcUI->AddToViewport();
	npcUI->Master = this;

}

