#pragma once

#include "CoreMinimal.h"
#include "NazarenePlayerSettings.generated.h"

UENUM(BlueprintType)
enum class ENazareneColorblindMode : uint8
{
    None = 0 UMETA(DisplayName = "None"),
    Deuteranopia = 1 UMETA(DisplayName = "Deuteranopia"),
    Protanopia = 2 UMETA(DisplayName = "Protanopia"),
    Tritanopia = 3 UMETA(DisplayName = "Tritanopia")
};

USTRUCT(BlueprintType)
struct FNazarenePlayerSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
    float MouseSensitivity = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
    bool bInvertLookY = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Video")
    float FieldOfView = 68.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Video")
    float DisplayGamma = 2.2f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Video")
    float FrameRateLimit = 120.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    float MasterVolume = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accessibility")
    float SubtitleTextScale = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accessibility")
    ENazareneColorblindMode ColorblindMode = ENazareneColorblindMode::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accessibility")
    bool bHighContrastHUD = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accessibility")
    bool bScreenShakeReduction = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accessibility")
    float HUDScale = 1.0f;
};
