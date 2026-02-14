#include "NazareneMenuCameraActor.h"

#include "Camera/CameraComponent.h"

ANazareneMenuCameraActor::ANazareneMenuCameraActor()
{
    PrimaryActorTick.bCanEverTick = true;

    MenuCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("MenuCamera"));
    RootComponent = MenuCamera;
}

void ANazareneMenuCameraActor::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    OrbitAngle += OrbitSpeed * DeltaSeconds;
    if (OrbitAngle >= 360.0f)
    {
        OrbitAngle -= 360.0f;
    }

    const float Rad = FMath::DegreesToRadians(OrbitAngle);
    const FVector Pos = OrbitCenter + FVector(
        FMath::Cos(Rad) * OrbitRadius,
        FMath::Sin(Rad) * OrbitRadius,
        CameraHeight + FMath::Sin(Rad * 0.3f) * HeightVariation
    );

    SetActorLocation(Pos);

    const FRotator LookAt = (OrbitCenter + FVector(0.0f, 0.0f, 80.0f) - Pos).Rotation();
    SetActorRotation(LookAt);
}
