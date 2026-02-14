#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "NazareneTypes.h"
#include "NazareneHUD.generated.h"

class UNazareneHUDWidget;

UCLASS()
class THENAZARENEAAA_API ANazareneHUD : public AHUD
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;
    virtual void DrawHUD() override;

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void SetRegionName(const FString& InRegionName);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void SetObjective(const FString& InObjective);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowMessage(const FString& InMessage, float Duration = 3.5f);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    bool TogglePauseMenu();

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void SetStartMenuVisible(bool bVisible);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    bool IsStartMenuVisible() const;

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void SetPauseMenuVisible(bool bVisible);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    bool IsPauseMenuVisible() const;

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowDamageNumber(const FVector& WorldLocation, float Amount, ENazareneDamageNumberType Type);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowDeathOverlay(int32 RetryCount);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void SetLoadingOverlayVisible(bool bVisible, const FString& LoreTip);

private:
    void ApplyMenuInputMode(bool bMenuVisible);

private:
    UPROPERTY()
    TObjectPtr<UNazareneHUDWidget> RuntimeWidget;

    FString RegionName = TEXT("Chapter 1: Galilee Shores");
    FString Objective = TEXT("Redeem the guardian.");
};

