#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Actor.h"
#include "NazarenePrayerSite.generated.h"

class ANazarenePlayerCharacter;
class UPointLightComponent;
class UPrimitiveComponent;
class USphereComponent;
class UStaticMeshComponent;
struct FHitResult;

UCLASS()
class THENAZARENEAAA_API ANazarenePrayerSite : public AActor
{
    GENERATED_BODY()

public:
    ANazarenePrayerSite();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prayer Site")
    FName SiteId = FName(TEXT("galilee_site_01"));

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prayer Site")
    FString SiteName = TEXT("Prayer Site: Galilee Shores");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prayer Site")
    FString PromptText = TEXT("Press E to pray and rest");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prayer Site")
    float InteractionRadius = 220.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prayer Site")
    FString LoreText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prayer Site")
    float FaithRefillPercent = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prayer Site")
    FString VisualTheme = TEXT("Default");

    UFUNCTION(BlueprintCallable, Category = "Prayer Site")
    FVector GetRespawnLocation() const;

    UFUNCTION(BlueprintCallable, Category = "Prayer Site")
    FString GetPromptMessage() const;

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<USphereComponent> InteractionSphere;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UStaticMeshComponent> AltarMesh;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UPointLightComponent> PrayerLight;

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
