#include "NazarenePrayerSite.h"

#include "Components/PointLightComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NazarenePlayerCharacter.h"
#include "UObject/ConstructorHelpers.h"

ANazarenePrayerSite::ANazarenePrayerSite()
{
    PrimaryActorTick.bCanEverTick = false;

    InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
    SetRootComponent(InteractionSphere);
    InteractionSphere->SetSphereRadius(InteractionRadius);
    InteractionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    InteractionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    InteractionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    // Stone ring base - Dark Souls bonfire ring of stones
    StoneRingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StoneRingMesh"));
    StoneRingMesh->SetupAttachment(InteractionSphere);
    StoneRingMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -85.0f));
    StoneRingMesh->SetRelativeScale3D(FVector(2.2f, 2.2f, 0.18f));
    StoneRingMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // Altar block - weathered stone pedestal
    AltarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AltarMesh"));
    AltarMesh->SetupAttachment(InteractionSphere);
    AltarMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -55.0f));
    AltarMesh->SetRelativeScale3D(FVector(0.85f, 0.85f, 0.72f));
    AltarMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // Sacred flame mesh - warm ember glow hovering above altar
    FlameMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlameMesh"));
    FlameMesh->SetupAttachment(InteractionSphere);
    FlameMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 65.0f));
    FlameMesh->SetRelativeScale3D(FVector(0.28f, 0.28f, 0.55f));
    FlameMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // Inner glow - bright warm light close to flame (bonfire core)
    PrayerLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("PrayerLight"));
    PrayerLight->SetupAttachment(InteractionSphere);
    PrayerLight->SetRelativeLocation(FVector(0.0f, 0.0f, 80.0f));
    PrayerLight->SetIntensity(5500.0f);
    PrayerLight->SetAttenuationRadius(650.0f);
    PrayerLight->SetLightColor(FColor(255, 192, 96));
    PrayerLight->SetSourceRadius(12.0f);

    // Outer glow - dim warm fill for sanctuary area
    OuterGlowLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("OuterGlowLight"));
    OuterGlowLight->SetupAttachment(InteractionSphere);
    OuterGlowLight->SetRelativeLocation(FVector(0.0f, 0.0f, 200.0f));
    OuterGlowLight->SetIntensity(2200.0f);
    OuterGlowLight->SetAttenuationRadius(1400.0f);
    OuterGlowLight->SetLightColor(FColor(255, 220, 160));
    OuterGlowLight->SetSourceRadius(28.0f);

    // Ground scatter - low amber for bonfire ground glow
    GroundScatterLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("GroundScatterLight"));
    GroundScatterLight->SetupAttachment(InteractionSphere);
    GroundScatterLight->SetRelativeLocation(FVector(0.0f, 0.0f, -30.0f));
    GroundScatterLight->SetIntensity(1800.0f);
    GroundScatterLight->SetAttenuationRadius(800.0f);
    GroundScatterLight->SetLightColor(FColor(255, 168, 72));
    GroundScatterLight->SetSourceRadius(40.0f);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (CubeMesh.Succeeded())
    {
        AltarMesh->SetStaticMesh(CubeMesh.Object);
        StoneRingMesh->SetStaticMesh(CubeMesh.Object);
    }

    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    if (SphereMesh.Succeeded())
    {
        FlameMesh->SetStaticMesh(SphereMesh.Object);
    }
}

void ANazarenePrayerSite::BeginPlay()
{
    Super::BeginPlay();

    InteractionSphere->SetSphereRadius(InteractionRadius);
    InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &ANazarenePrayerSite::HandleOverlapBegin);
    InteractionSphere->OnComponentEndOverlap.AddDynamic(this, &ANazarenePrayerSite::HandleOverlapEnd);

    // Apply Dark Souls bonfire material tints
    UMaterialInterface* BaseMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
    if (BaseMaterial != nullptr)
    {
        // Weathered dark stone for altar
        UMaterialInstanceDynamic* AltarMat = UMaterialInstanceDynamic::Create(BaseMaterial, this);
        if (AltarMat != nullptr)
        {
            AltarMat->SetVectorParameterValue(TEXT("Color"), FVector(0.22f, 0.18f, 0.14f));
            AltarMesh->SetMaterial(0, AltarMat);
        }

        // Dark stone ring
        UMaterialInstanceDynamic* RingMat = UMaterialInstanceDynamic::Create(BaseMaterial, this);
        if (RingMat != nullptr)
        {
            RingMat->SetVectorParameterValue(TEXT("Color"), FVector(0.16f, 0.14f, 0.12f));
            StoneRingMesh->SetMaterial(0, RingMat);
        }

        // Sacred flame - bright golden ember
        UMaterialInstanceDynamic* FlameMat = UMaterialInstanceDynamic::Create(BaseMaterial, this);
        if (FlameMat != nullptr)
        {
            FlameMat->SetVectorParameterValue(TEXT("Color"), FVector(1.6f, 0.85f, 0.25f));
            FlameMesh->SetMaterial(0, FlameMat);
        }
    }
}

FVector ANazarenePrayerSite::GetRespawnLocation() const
{
    return GetActorLocation() + FVector(0.0f, 0.0f, 180.0f);
}

FString ANazarenePrayerSite::GetPromptMessage() const
{
    return FString::Printf(TEXT("%s: %s | Save F1-F3 | Load F5-F7"), *SiteName, *PromptText);
}

void ANazarenePrayerSite::HandleOverlapBegin(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult
)
{
    ANazarenePlayerCharacter* Player = Cast<ANazarenePlayerCharacter>(OtherActor);
    if (Player == nullptr)
    {
        return;
    }
    Player->SetActivePrayerSite(this);
}

void ANazarenePrayerSite::HandleOverlapEnd(
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
    Player->ClearActivePrayerSite(this);
}
