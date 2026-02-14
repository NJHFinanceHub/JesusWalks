#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NazareneMenuCameraActor.generated.h"

class UCameraComponent;

UENUM(BlueprintType)
enum class ENazareneMenuCameraMode : uint8
{
    Orbit = 0,
    PanOut = 1
};

UCLASS()
class THENAZARENEAAA_API ANazareneMenuCameraActor : public AActor
{
    GENERATED_BODY()

public:
    ANazareneMenuCameraActor();
    virtual void Tick(float DeltaSeconds) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Camera")
    float OrbitSpeed = 8.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Camera")
    float OrbitRadius = 1800.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Camera")
    float CameraHeight = 400.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Camera")
    float HeightVariation = 80.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Camera")
    FVector OrbitCenter = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Camera")
    ENazareneMenuCameraMode CameraMode = ENazareneMenuCameraMode::PanOut;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Camera|Pan")
    FVector PanStartOffset = FVector(-220.0f, 0.0f, 110.0f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Camera|Pan")
    FVector PanEndOffset = FVector(1080.0f, 0.0f, 250.0f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Camera|Pan")
    FVector PanLookAtOffset = FVector(380.0f, 0.0f, 110.0f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Camera|Pan")
    float PanDuration = 14.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Camera|Pan")
    bool bLoopPan = true;

private:
    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UCameraComponent> MenuCamera;

    float OrbitAngle = 0.0f;
    float PanAlpha = 0.0f;
};
