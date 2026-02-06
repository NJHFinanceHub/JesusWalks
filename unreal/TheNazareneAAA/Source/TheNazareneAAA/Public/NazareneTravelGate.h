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

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UStaticMeshComponent> GateMesh;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UPointLightComponent> GateLight;

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
