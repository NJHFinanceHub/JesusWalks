#include "NazareneTravelGate.h"

#include "Components/PointLightComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NazareneCampaignGameMode.h"
#include "NazarenePlayerCharacter.h"
#include "UObject/ConstructorHelpers.h"

ANazareneTravelGate::ANazareneTravelGate()
{
    PrimaryActorTick.bCanEverTick = false;

    InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
    SetRootComponent(InteractionSphere);
    InteractionSphere->SetSphereRadius(260.0f);
    InteractionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    InteractionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    InteractionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    GateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GateMesh"));
    GateMesh->SetupAttachment(InteractionSphere);
    GateMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 130.0f));
    GateMesh->SetRelativeScale3D(FVector(1.5f, 1.5f, 2.2f));
    GateMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    GateLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("GateLight"));
    GateLight->SetupAttachment(InteractionSphere);
    GateLight->SetRelativeLocation(FVector(0.0f, 0.0f, 220.0f));
    GateLight->SetIntensity(4200.0f);
    GateLight->SetAttenuationRadius(1250.0f);
    GateLight->SetLightColor(FColor(255, 214, 166));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> GateShapeMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    if (GateShapeMesh.Succeeded())
    {
        GateMesh->SetStaticMesh(GateShapeMesh.Object);
    }
}

void ANazareneTravelGate::BeginPlay()
{
    Super::BeginPlay();
    InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &ANazareneTravelGate::HandleOverlapBegin);
    InteractionSphere->OnComponentEndOverlap.AddDynamic(this, &ANazareneTravelGate::HandleOverlapEnd);
    SetGateEnabled(false);
}

void ANazareneTravelGate::SetGateEnabled(bool bEnabled)
{
    bGateEnabled = bEnabled;
    SetActorHiddenInGame(!bGateEnabled);
    SetActorEnableCollision(bGateEnabled);
    InteractionSphere->SetGenerateOverlapEvents(bGateEnabled);
    GateLight->SetVisibility(bGateEnabled);
}

bool ANazareneTravelGate::IsGateEnabled() const
{
    return bGateEnabled;
}

void ANazareneTravelGate::RequestTravel()
{
    if (!bGateEnabled)
    {
        return;
    }

    ANazareneCampaignGameMode* Campaign = GetWorld() ? Cast<ANazareneCampaignGameMode>(GetWorld()->GetAuthGameMode()) : nullptr;
    if (Campaign != nullptr)
    {
        Campaign->RequestTravel(TargetRegionIndex);
    }
}

void ANazareneTravelGate::HandleOverlapBegin(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult
)
{
    if (!bGateEnabled)
    {
        return;
    }

    ANazarenePlayerCharacter* Player = Cast<ANazarenePlayerCharacter>(OtherActor);
    if (Player == nullptr)
    {
        return;
    }
    Player->SetActiveTravelGate(this);
}

void ANazareneTravelGate::HandleOverlapEnd(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex
)
{
    ANazarenePlayerCharacter* Player = Cast<ANazarenePlayerCharacter>(OtherActor);
    if (Player == nullptr)
    {
        return;
    }
    Player->ClearActiveTravelGate(this);
}
