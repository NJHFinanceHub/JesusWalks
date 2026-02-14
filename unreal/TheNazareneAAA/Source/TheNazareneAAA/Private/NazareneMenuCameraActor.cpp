#include "NazareneMenuCameraActor.h"

#include "Camera/CameraComponent.h"

ANazareneMenuCameraActor::ANazareneMenuCameraActor()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bTickEvenWhenPaused = true;

    MenuCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("MenuCamera"));
    RootComponent = MenuCamera;
    MenuCamera->SetFieldOfView(52.0f);
    MenuCamera->PostProcessSettings.bOverride_AutoExposureMethod = true;
    MenuCamera->PostProcessSettings.AutoExposureMethod = EAutoExposureMethod::AEM_Histogram;
    MenuCamera->PostProcessSettings.bOverride_AutoExposureMinBrightness = true;
    MenuCamera->PostProcessSettings.AutoExposureMinBrightness = 0.35f;
    MenuCamera->PostProcessSettings.bOverride_AutoExposureMaxBrightness = true;
    MenuCamera->PostProcessSettings.AutoExposureMaxBrightness = 1.8f;
    MenuCamera->PostProcessSettings.bOverride_AutoExposureBias = true;
    MenuCamera->PostProcessSettings.AutoExposureBias = -0.2f;
    MenuCamera->PostProcessSettings.bOverride_VignetteIntensity = true;
    MenuCamera->PostProcessSettings.VignetteIntensity = 0.35f;
    MenuCamera->PostProcessBlendWeight = 1.0f;
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
