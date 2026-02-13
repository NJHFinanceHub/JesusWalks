#pragma once

#include "CoreMinimal.h"
#include "NazarenePlayerSettings.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "NazareneSettingsSubsystem.generated.h"

UCLASS()
class THENAZARENEAAA_API UNazareneSettingsSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, Category = "Settings")
    const FNazarenePlayerSettings& GetSettings() const;

    UFUNCTION(BlueprintCallable, Category = "Settings")
    void SetMouseSensitivity(float InValue);

    UFUNCTION(BlueprintCallable, Category = "Settings")
    void SetInvertLookY(bool bInValue);

    UFUNCTION(BlueprintCallable, Category = "Settings")
    void SetFieldOfView(float InValue);

    UFUNCTION(BlueprintCallable, Category = "Settings")
    void SetDisplayGamma(float InValue);

    UFUNCTION(BlueprintCallable, Category = "Settings")
    void SetFrameRateLimit(float InValue);

    UFUNCTION(BlueprintCallable, Category = "Settings")
    void SetMasterVolume(float InValue);

    UFUNCTION(BlueprintCallable, Category = "Settings")
    bool SaveSettings();

    UFUNCTION(BlueprintCallable, Category = "Settings")
    bool LoadSettings();

    UFUNCTION(BlueprintCallable, Category = "Settings")
    void ApplySettings();

private:
    static FString SettingsSlotName();
    static FNazarenePlayerSettings ClampSettings(const FNazarenePlayerSettings& InSettings);
    void ApplySettingsToPlayerPawn() const;

private:
    UPROPERTY()
    FNazarenePlayerSettings CurrentSettings;
};
