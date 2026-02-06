#include "NazarenePrayerSite.h"

#include "Components/PointLightComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
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

    AltarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AltarMesh"));
    AltarMesh->SetupAttachment(InteractionSphere);
    AltarMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -70.0f));
    AltarMesh->SetRelativeScale3D(FVector(1.3f, 1.3f, 0.5f));
    AltarMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    PrayerLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("PrayerLight"));
    PrayerLight->SetupAttachment(InteractionSphere);
    PrayerLight->SetRelativeLocation(FVector(0.0f, 0.0f, 160.0f));
    PrayerLight->SetIntensity(3500.0f);
    PrayerLight->SetAttenuationRadius(1000.0f);
    PrayerLight->SetLightColor(FColor(255, 229, 189));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    if (CylinderMesh.Succeeded())
    {
        AltarMesh->SetStaticMesh(CylinderMesh.Object);
    }
}

void ANazarenePrayerSite::BeginPlay()
{
    Super::BeginPlay();

    InteractionSphere->SetSphereRadius(InteractionRadius);
    InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &ANazarenePrayerSite::HandleOverlapBegin);
    InteractionSphere->OnComponentEndOverlap.AddDynamic(this, &ANazarenePrayerSite::HandleOverlapEnd);
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
