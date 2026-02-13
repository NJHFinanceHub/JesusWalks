#pragma once

#include "CoreMinimal.h"
#include "NazarenePlayerSettings.generated.h"

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
};
