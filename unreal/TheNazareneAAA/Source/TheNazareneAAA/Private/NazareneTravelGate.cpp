#include "NazareneTravelGate.h"

#include "Components/PointLightComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NazareneCampaignGameMode.h"
#include "NazarenePlayerCharacter.h"
#include "UObject/ConstructorHelpers.h"

ANazareneTravelGate::ANazareneTravelGate()
{
    PrimaryActorTick.bCanEverTick = false;

    InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
    SetRootComponent(InteractionSphere);
    InteractionSphere->SetSphereRadius(280.0f);
    InteractionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    InteractionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    InteractionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    // Primary fog wall mesh - tall vertical plane (Dark Souls fog gate aesthetic)
    GateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GateMesh"));
    GateMesh->SetupAttachment(InteractionSphere);
    GateMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));
    GateMesh->SetRelativeScale3D(FVector(4.0f, 0.08f, 4.5f));
    GateMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // Secondary fog depth layer for parallax depth
    FogLayerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FogLayerMesh"));
    FogLayerMesh->SetupAttachment(InteractionSphere);
    FogLayerMesh->SetRelativeLocation(FVector(0.0f, 20.0f, 180.0f));
    FogLayerMesh->SetRelativeScale3D(FVector(3.6f, 0.06f, 4.0f));
    FogLayerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // Central ethereal glow - cool white-blue
    GateLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("GateLight"));
    GateLight->SetupAttachment(InteractionSphere);
    GateLight->SetRelativeLocation(FVector(0.0f, 0.0f, 220.0f));
    GateLight->SetIntensity(6000.0f);
    GateLight->SetAttenuationRadius(900.0f);
    GateLight->SetLightColor(FColor(200, 220, 255));
    GateLight->SetSourceRadius(35.0f);

    // Edge glow at fog wall top
    EdgeGlowLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("EdgeGlowLight"));
    EdgeGlowLight->SetupAttachment(InteractionSphere);
    EdgeGlowLight->SetRelativeLocation(FVector(0.0f, 0.0f, 380.0f));
    EdgeGlowLight->SetIntensity(3200.0f);
    EdgeGlowLight->SetAttenuationRadius(1100.0f);
    EdgeGlowLight->SetLightColor(FColor(180, 200, 240));
    EdgeGlowLight->SetSourceRadius(20.0f);

    // Ground fog scatter
    GroundFogLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("GroundFogLight"));
    GroundFogLight->SetupAttachment(InteractionSphere);
    GroundFogLight->SetRelativeLocation(FVector(0.0f, 0.0f, -20.0f));
    GroundFogLight->SetIntensity(1800.0f);
    GroundFogLight->SetAttenuationRadius(600.0f);
    GroundFogLight->SetLightColor(FColor(160, 190, 230));
    GroundFogLight->SetSourceRadius(50.0f);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (CubeMesh.Succeeded())
    {
        GateMesh->SetStaticMesh(CubeMesh.Object);
        FogLayerMesh->SetStaticMesh(CubeMesh.Object);
    }
}

void ANazareneTravelGate::BeginPlay()
{
    Super::BeginPlay();
    InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &ANazareneTravelGate::HandleOverlapBegin);
    InteractionSphere->OnComponentEndOverlap.AddDynamic(this, &ANazareneTravelGate::HandleOverlapEnd);

    // Apply fog wall materials - ethereal translucent look
    UMaterialInterface* BaseMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
    if (BaseMaterial != nullptr)
    {
        // Primary fog plane - bright ethereal white-blue
        UMaterialInstanceDynamic* FogMat = UMaterialInstanceDynamic::Create(BaseMaterial, this);
        if (FogMat != nullptr)
        {
            FogMat->SetVectorParameterValue(TEXT("Color"), FVector(0.72f, 0.80f, 0.95f));
            GateMesh->SetMaterial(0, FogMat);
        }

        // Secondary fog layer - dimmer depth
        UMaterialInstanceDynamic* FogDepthMat = UMaterialInstanceDynamic::Create(BaseMaterial, this);
        if (FogDepthMat != nullptr)
        {
            FogDepthMat->SetVectorParameterValue(TEXT("Color"), FVector(0.55f, 0.62f, 0.78f));
            FogLayerMesh->SetMaterial(0, FogDepthMat);
        }
    }

    SetGateEnabled(false);
}

void ANazareneTravelGate::SetGateEnabled(bool bEnabled)
{
    bGateEnabled = bEnabled;
    SetActorHiddenInGame(!bGateEnabled);
    SetActorEnableCollision(bGateEnabled);
    InteractionSphere->SetGenerateOverlapEvents(bGateEnabled);
    GateLight->SetVisibility(bGateEnabled);
    EdgeGlowLight->SetVisibility(bGateEnabled);
    GroundFogLight->SetVisibility(bGateEnabled);
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
