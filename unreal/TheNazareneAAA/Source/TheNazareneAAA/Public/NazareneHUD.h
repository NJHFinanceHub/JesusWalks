#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "NazareneHUD.generated.h"

UCLASS()
class THENAZARENEAAA_API ANazareneHUD : public AHUD
{
    GENERATED_BODY()

public:
    virtual void DrawHUD() override;

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void SetRegionName(const FString& InRegionName);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void SetObjective(const FString& InObjective);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowMessage(const FString& InMessage, float Duration = 3.5f);

private:
    FString RegionName = TEXT("Chapter 1: Galilee Shores");
    FString Objective = TEXT("Redeem the guardian.");
    FString TransientMessage;
    float MessageTimer = 0.0f;
};

