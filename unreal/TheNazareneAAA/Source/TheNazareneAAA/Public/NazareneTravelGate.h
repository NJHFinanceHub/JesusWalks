#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Actor.h"
#include "NazareneTravelGate.generated.h"

class ANazarenePlayerCharacter;
class UPrimitiveComponent;
class USphereComponent;
class UStaticMeshComponent;
class UPointLightComponent;
struct FHitResult;

UCLASS()
class THENAZARENEAAA_API ANazareneTravelGate : public AActor
{
    GENERATED_BODY()

public:
    ANazareneTravelGate();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gate")
    int32 TargetRegionIndex = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gate")
    FString PromptText = TEXT("Press E to travel onward");

    UFUNCTION(BlueprintCallable, Category = "Gate")
    void SetGateEnabled(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category = "Gate")
    bool IsGateEnabled() const;

    UFUNCTION(BlueprintCallable, Category = "Gate")
    void RequestTravel();

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<USphereComponent> InteractionSphere;

    /** Primary fog wall mesh - tall vertical plane. */
    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UStaticMeshComponent> GateMesh;

    /** Secondary fog layer for depth. */
    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UStaticMeshComponent> FogLayerMesh;

    /** Primary ethereal glow at fog wall center. */
    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UPointLightComponent> GateLight;

    /** Secondary pulsing light at fog wall edges. */
    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UPointLightComponent> EdgeGlowLight;

    /** Ground-level light for fog wall ground scatter. */
    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UPointLightComponent> GroundFogLight;

    UPROPERTY()
    bool bGateEnabled = false;

    UFUNCTION()
    void HandleOverlapBegin(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

    UFUNCTION()
    void HandleOverlapEnd(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex
    );
};
