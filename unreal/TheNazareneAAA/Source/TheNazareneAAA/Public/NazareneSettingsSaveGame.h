#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "NazarenePlayerSettings.h"
#include "NazareneSettingsSaveGame.generated.h"

UCLASS()
class THENAZARENEAAA_API UNazareneSettingsSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32 SaveVersion = 1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FString Timestamp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FNazarenePlayerSettings Settings;
};
