#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "NazareneTypes.h"
#include "NazareneSaveSubsystem.generated.h"

UCLASS()
class THENAZARENEAAA_API UNazareneSaveSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    bool SavePayloadToSlot(int32 SlotId, const FNazareneSavePayload& Payload);

    UFUNCTION(BlueprintCallable)
    bool LoadPayloadFromSlot(int32 SlotId, FNazareneSavePayload& OutPayload) const;

    UFUNCTION(BlueprintCallable)
    bool SlotExists(int32 SlotId) const;

    UFUNCTION(BlueprintCallable)
    FString GetSlotSummary(int32 SlotId) const;

    UFUNCTION(BlueprintCallable)
    bool SaveCheckpoint(const FNazareneSavePayload& Payload);

    UFUNCTION(BlueprintCallable)
    bool LoadCheckpoint(FNazareneSavePayload& OutPayload) const;

    UFUNCTION(BlueprintCallable)
    bool CheckpointExists() const;

    UFUNCTION(BlueprintCallable)
    bool ClearCheckpoint();

private:
    static FString SlotNameForSlotId(int32 SlotId);
    static FString CheckpointSlotName();
};

