#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NazareneMenuCameraActor.generated.h"

class UCameraComponent;

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

private:
    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UCameraComponent> MenuCamera;

    float OrbitAngle = 0.0f;
};
