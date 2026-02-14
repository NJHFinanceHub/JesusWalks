#include "NazareneMenuCameraActor.h"

#include "Camera/CameraComponent.h"

ANazareneMenuCameraActor::ANazareneMenuCameraActor()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bTickEvenWhenPaused = true;

    MenuCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("MenuCamera"));
    RootComponent = MenuCamera;
    MenuCamera->SetFieldOfView(58.0f);
}

void ANazareneMenuCameraActor::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (CameraMode == ENazareneMenuCameraMode::PanOut)
    {
        const float SafeDuration = FMath::Max(PanDuration, 0.1f);
        PanAlpha += DeltaSeconds / SafeDuration;

        if (PanAlpha >= 1.0f)
        {
            if (bLoopPan)
            {
                PanAlpha = 0.0f;
            }
            else
            {
                PanAlpha = 1.0f;
            }
        }

        const float EasedAlpha = FMath::InterpEaseInOut(0.0f, 1.0f, PanAlpha, 2.15f);
        const FVector PanOffset = FMath::Lerp(PanStartOffset, PanEndOffset, EasedAlpha);
        const float VerticalDrift = FMath::Sin(PanAlpha * PI * 2.0f * 0.45f) * 9.0f;
        const FVector Position = OrbitCenter + PanOffset + FVector(0.0f, 0.0f, VerticalDrift);

        SetActorLocation(Position);

        const FVector LookAtTarget = OrbitCenter + PanLookAtOffset;
        SetActorRotation((LookAtTarget - Position).Rotation());
        return;
    }

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
