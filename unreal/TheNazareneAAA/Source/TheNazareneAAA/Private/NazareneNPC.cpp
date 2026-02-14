#include "NazareneNPC.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NazareneHUD.h"
#include "NazarenePlayerCharacter.h"

ANazareneNPC::ANazareneNPC()
{
	PrimaryActorTick.bCanEverTick = false;

	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->SetSphereRadius(InteractionRadius);
	InteractionSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	InteractionSphere->SetGenerateOverlapEvents(true);
	RootComponent = InteractionSphere;

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(RootComponent);
	BodyMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 55.0f));
	BodyMesh->SetRelativeScale3D(FVector(0.45f, 0.45f, 1.1f));
	BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UStaticMesh* CylinderMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (CylinderMesh != nullptr)
	{
		BodyMesh->SetStaticMesh(CylinderMesh);
	}

	HeadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeadMesh"));
	HeadMesh->SetupAttachment(RootComponent);
	HeadMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 170.0f));
	HeadMesh->SetRelativeScale3D(FVector(0.35f, 0.35f, 0.35f));
	HeadMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UStaticMesh* SphereMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMesh != nullptr)
	{
		HeadMesh->SetStaticMesh(SphereMesh);
	}
}

void ANazareneNPC::BeginPlay()
{
	Super::BeginPlay();

	InteractionSphere->SetSphereRadius(InteractionRadius);
	InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &ANazareneNPC::HandleOverlapBegin);
	InteractionSphere->OnComponentEndOverlap.AddDynamic(this, &ANazareneNPC::HandleOverlapEnd);

	// Tint NPC body blue-green to distinguish from enemies
	UMaterialInterface* BaseMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	if (BaseMaterial != nullptr)
	{
		UMaterialInstanceDynamic* BodyDynMat = UMaterialInstanceDynamic::Create(BaseMaterial, this);
		if (BodyDynMat != nullptr)
		{
			BodyDynMat->SetVectorParameterValue(TEXT("Color"), FVector(0.28f, 0.45f, 0.62f));
			BodyMesh->SetMaterial(0, BodyDynMat);
		}

		UMaterialInstanceDynamic* HeadDynMat = UMaterialInstanceDynamic::Create(BaseMaterial, this);
		if (HeadDynMat != nullptr)
		{
			HeadDynMat->SetVectorParameterValue(TEXT("Color"), FVector(0.72f, 0.58f, 0.44f));
			HeadMesh->SetMaterial(0, HeadDynMat);
		}
	}
}

void ANazareneNPC::HandleOverlapBegin(UPrimitiveComponent* /*OverlappedComponent*/, AActor* OtherActor, UPrimitiveComponent* /*OtherComp*/, int32 /*OtherBodyIndex*/, bool /*bFromSweep*/, const FHitResult& /*SweepResult*/)
{
	ANazarenePlayerCharacter* Player = Cast<ANazarenePlayerCharacter>(OtherActor);
	if (Player == nullptr)
	{
		return;
	}

	bPlayerInRange = true;
	CachedPlayer = Player;
	Player->SetActiveNPC(this);

	const FString Greeting = IdleGreeting.IsEmpty() ? FString::Printf(TEXT("Press E to speak with %s"), *NPCName) : IdleGreeting;
	Player->SetContextHint(Greeting);
}

void ANazareneNPC::HandleOverlapEnd(UPrimitiveComponent* /*OverlappedComponent*/, AActor* OtherActor, UPrimitiveComponent* /*OtherComp*/, int32 /*OtherBodyIndex*/)
{
	ANazarenePlayerCharacter* Player = Cast<ANazarenePlayerCharacter>(OtherActor);
	if (Player == nullptr)
	{
		return;
	}

	bPlayerInRange = false;
	Player->ClearActiveNPC(this);
	Player->SetContextHint(TEXT(""));
}

void ANazareneNPC::AdvanceDialogue()
{
	if (DialogueLines.Num() == 0)
	{
		return;
	}

	if (CurrentDialogueIndex >= DialogueLines.Num())
	{
		if (bLoopDialogue)
		{
			CurrentDialogueIndex = 0;
		}
		else
		{
			return;
		}
	}

	const FNazareneNPCDialogueLine& Line = DialogueLines[CurrentDialogueIndex];

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		if (ANazareneHUD* HUD = Cast<ANazareneHUD>(PC->GetHUD()))
		{
			const FString DisplayText = FString::Printf(TEXT("%s: \"%s\""), *NPCName, *Line.Text);
			HUD->ShowMessage(DisplayText, Line.DisplayDuration);
		}
	}

	if (Line.VoiceClip != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, Line.VoiceClip, GetActorLocation(), 1.0f);
	}

	++CurrentDialogueIndex;
}

void ANazareneNPC::ResetDialogue()
{
	CurrentDialogueIndex = 0;
}
