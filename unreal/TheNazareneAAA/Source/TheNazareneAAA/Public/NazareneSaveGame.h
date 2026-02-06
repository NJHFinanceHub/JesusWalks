#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "NazareneTypes.h"
#include "NazareneSaveGame.generated.h"

UCLASS()
class THENAZARENEAAA_API UNazareneSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32 SaveVersion = 1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32 SlotId = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    bool bIsCheckpoint = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FString Timestamp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FNazareneSavePayload Payload;
};

