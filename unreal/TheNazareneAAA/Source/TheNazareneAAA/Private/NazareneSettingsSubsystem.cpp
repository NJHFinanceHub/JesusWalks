#include "NazareneSettingsSubsystem.h"

#include "Engine/Engine.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/App.h"
#include "NazarenePlayerCharacter.h"
#include "NazareneSettingsSaveGame.h"

void UNazareneSettingsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    LoadSettings();
    ApplySettings();
}

const FNazarenePlayerSettings& UNazareneSettingsSubsystem::GetSettings() const
{
    return CurrentSettings;
}

void UNazareneSettingsSubsystem::SetMouseSensitivity(float InValue)
{
    CurrentSettings.MouseSensitivity = InValue;
    CurrentSettings = ClampSettings(CurrentSettings);
    ApplySettings();
}

void UNazareneSettingsSubsystem::SetInvertLookY(bool bInValue)
{
    CurrentSettings.bInvertLookY = bInValue;
    ApplySettings();
}

void UNazareneSettingsSubsystem::SetFieldOfView(float InValue)
{
    CurrentSettings.FieldOfView = InValue;
    CurrentSettings = ClampSettings(CurrentSettings);
    ApplySettings();
}

void UNazareneSettingsSubsystem::SetDisplayGamma(float InValue)
{
    CurrentSettings.DisplayGamma = InValue;
    CurrentSettings = ClampSettings(CurrentSettings);
    ApplySettings();
}

void UNazareneSettingsSubsystem::SetFrameRateLimit(float InValue)
{
    CurrentSettings.FrameRateLimit = InValue;
    CurrentSettings = ClampSettings(CurrentSettings);
    ApplySettings();
}

void UNazareneSettingsSubsystem::SetMasterVolume(float InValue)
{
    CurrentSettings.MasterVolume = InValue;
    CurrentSettings = ClampSettings(CurrentSettings);
    ApplySettings();
}

void UNazareneSettingsSubsystem::SetSubtitleTextScale(float InValue)
{
    CurrentSettings.SubtitleTextScale = InValue;
    CurrentSettings = ClampSettings(CurrentSettings);
    ApplySettings();
}

void UNazareneSettingsSubsystem::SetColorblindMode(ENazareneColorblindMode InMode)
{
    CurrentSettings.ColorblindMode = InMode;
    ApplySettings();
}

void UNazareneSettingsSubsystem::SetHighContrastHUD(bool bInValue)
{
    CurrentSettings.bHighContrastHUD = bInValue;
    ApplySettings();
}

void UNazareneSettingsSubsystem::SetScreenShakeReduction(bool bInValue)
{
    CurrentSettings.bScreenShakeReduction = bInValue;
    ApplySettings();
}

void UNazareneSettingsSubsystem::SetHUDScale(float InValue)
{
    CurrentSettings.HUDScale = InValue;
    CurrentSettings = ClampSettings(CurrentSettings);
    ApplySettings();
}

bool UNazareneSettingsSubsystem::SaveSettings()
{
    UNazareneSettingsSaveGame* SaveGame = Cast<UNazareneSettingsSaveGame>(UGameplayStatics::CreateSaveGameObject(UNazareneSettingsSaveGame::StaticClass()));
    if (SaveGame == nullptr)
    {
        return false;
    }

    SaveGame->Timestamp = FDateTime::UtcNow().ToString(TEXT("%Y-%m-%d %H:%M:%S UTC"));
    SaveGame->Settings = ClampSettings(CurrentSettings);
    return UGameplayStatics::SaveGameToSlot(SaveGame, SettingsSlotName(), 0);
}

bool UNazareneSettingsSubsystem::LoadSettings()
{
    CurrentSettings = FNazarenePlayerSettings();

    if (!UGameplayStatics::DoesSaveGameExist(SettingsSlotName(), 0))
    {
        return false;
    }

    const UNazareneSettingsSaveGame* SaveGame = Cast<UNazareneSettingsSaveGame>(UGameplayStatics::LoadGameFromSlot(SettingsSlotName(), 0));
    if (SaveGame == nullptr)
    {
        return false;
    }

    CurrentSettings = ClampSettings(SaveGame->Settings);
    return true;
}

void UNazareneSettingsSubsystem::ApplySettings()
{
    CurrentSettings = ClampSettings(CurrentSettings);

    if (GEngine != nullptr)
    {
        if (UGameUserSettings* GameUserSettings = GEngine->GetGameUserSettings())
        {
            GameUserSettings->SetFrameRateLimit(CurrentSettings.FrameRateLimit);
            GameUserSettings->ApplyNonResolutionSettings();
        }

        GEngine->DisplayGamma = CurrentSettings.DisplayGamma;
    }

    FApp::SetVolumeMultiplier(CurrentSettings.MasterVolume);
    ApplySettingsToPlayerPawn();
}

FString UNazareneSettingsSubsystem::SettingsSlotName()
{
    return TEXT("Nazarene_UserSettings");
}

FNazarenePlayerSettings UNazareneSettingsSubsystem::ClampSettings(const FNazarenePlayerSettings& InSettings)
{
    FNazarenePlayerSettings Clamped = InSettings;
    Clamped.MouseSensitivity = FMath::Clamp(Clamped.MouseSensitivity, 0.2f, 3.0f);
    Clamped.FieldOfView = FMath::Clamp(Clamped.FieldOfView, 60.0f, 110.0f);
    Clamped.DisplayGamma = FMath::Clamp(Clamped.DisplayGamma, 1.6f, 2.8f);
    Clamped.FrameRateLimit = FMath::Clamp(Clamped.FrameRateLimit, 30.0f, 240.0f);
    Clamped.MasterVolume = FMath::Clamp(Clamped.MasterVolume, 0.0f, 1.0f);
    Clamped.SubtitleTextScale = FMath::Clamp(Clamped.SubtitleTextScale, 0.75f, 2.0f);
    Clamped.HUDScale = FMath::Clamp(Clamped.HUDScale, 0.75f, 1.5f);
    return Clamped;
}

void UNazareneSettingsSubsystem::ApplySettingsToPlayerPawn() const
{
    const UGameInstance* GameInstance = GetGameInstance();
    if (GameInstance == nullptr || GameInstance->GetWorld() == nullptr)
    {
        return;
    }

    if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GameInstance->GetWorld(), 0))
    {
        if (ANazarenePlayerCharacter* PlayerCharacter = Cast<ANazarenePlayerCharacter>(PlayerController->GetPawn()))
        {
            PlayerCharacter->ApplyUserLookAndCameraSettings(CurrentSettings.MouseSensitivity, CurrentSettings.bInvertLookY, CurrentSettings.FieldOfView);
        }
    }
}
