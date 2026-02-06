#include "NazareneSaveSubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "NazareneSaveGame.h"

bool UNazareneSaveSubsystem::SavePayloadToSlot(int32 SlotId, const FNazareneSavePayload& Payload)
{
    if (SlotId < 1)
    {
        return false;
    }

    UNazareneSaveGame* SaveGame = Cast<UNazareneSaveGame>(UGameplayStatics::CreateSaveGameObject(UNazareneSaveGame::StaticClass()));
    if (SaveGame == nullptr)
    {
        return false;
    }

    SaveGame->SlotId = SlotId;
    SaveGame->bIsCheckpoint = false;
    SaveGame->Timestamp = FDateTime::UtcNow().ToString(TEXT("%Y-%m-%d %H:%M:%S UTC"));
    SaveGame->Payload = Payload;

    return UGameplayStatics::SaveGameToSlot(SaveGame, SlotNameForSlotId(SlotId), 0);
}

bool UNazareneSaveSubsystem::LoadPayloadFromSlot(int32 SlotId, FNazareneSavePayload& OutPayload) const
{
    OutPayload = FNazareneSavePayload();
    if (SlotId < 1)
    {
        return false;
    }

    if (!UGameplayStatics::DoesSaveGameExist(SlotNameForSlotId(SlotId), 0))
    {
        return false;
    }

    const UNazareneSaveGame* SaveGame = Cast<UNazareneSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotNameForSlotId(SlotId), 0));
    if (SaveGame == nullptr)
    {
        return false;
    }

    OutPayload = SaveGame->Payload;
    return true;
}

bool UNazareneSaveSubsystem::SlotExists(int32 SlotId) const
{
    if (SlotId < 1)
    {
        return false;
    }
    return UGameplayStatics::DoesSaveGameExist(SlotNameForSlotId(SlotId), 0);
}

FString UNazareneSaveSubsystem::GetSlotSummary(int32 SlotId) const
{
    if (SlotId < 1)
    {
        return FString(TEXT("Invalid slot"));
    }

    if (!UGameplayStatics::DoesSaveGameExist(SlotNameForSlotId(SlotId), 0))
    {
        return FString::Printf(TEXT("Slot %d: Empty"), SlotId);
    }

    const UNazareneSaveGame* SaveGame = Cast<UNazareneSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotNameForSlotId(SlotId), 0));
    if (SaveGame == nullptr)
    {
        return FString::Printf(TEXT("Slot %d: Corrupted"), SlotId);
    }

    return FString::Printf(TEXT("Slot %d: %s"), SlotId, *SaveGame->Timestamp);
}

bool UNazareneSaveSubsystem::SaveCheckpoint(const FNazareneSavePayload& Payload)
{
    UNazareneSaveGame* SaveGame = Cast<UNazareneSaveGame>(UGameplayStatics::CreateSaveGameObject(UNazareneSaveGame::StaticClass()));
    if (SaveGame == nullptr)
    {
        return false;
    }

    SaveGame->SlotId = 0;
    SaveGame->bIsCheckpoint = true;
    SaveGame->Timestamp = FDateTime::UtcNow().ToString(TEXT("%Y-%m-%d %H:%M:%S UTC"));
    SaveGame->Payload = Payload;

    return UGameplayStatics::SaveGameToSlot(SaveGame, CheckpointSlotName(), 0);
}

bool UNazareneSaveSubsystem::LoadCheckpoint(FNazareneSavePayload& OutPayload) const
{
    OutPayload = FNazareneSavePayload();
    if (!UGameplayStatics::DoesSaveGameExist(CheckpointSlotName(), 0))
    {
        return false;
    }

    const UNazareneSaveGame* SaveGame = Cast<UNazareneSaveGame>(UGameplayStatics::LoadGameFromSlot(CheckpointSlotName(), 0));
    if (SaveGame == nullptr)
    {
        return false;
    }

    OutPayload = SaveGame->Payload;
    return true;
}

bool UNazareneSaveSubsystem::CheckpointExists() const
{
    return UGameplayStatics::DoesSaveGameExist(CheckpointSlotName(), 0);
}

bool UNazareneSaveSubsystem::ClearCheckpoint()
{
    if (!UGameplayStatics::DoesSaveGameExist(CheckpointSlotName(), 0))
    {
        return true;
    }
    return UGameplayStatics::DeleteGameInSlot(CheckpointSlotName(), 0);
}

FString UNazareneSaveSubsystem::SlotNameForSlotId(int32 SlotId)
{
    return FString::Printf(TEXT("slot_%d"), SlotId);
}

FString UNazareneSaveSubsystem::CheckpointSlotName()
{
    return FString(TEXT("campaign_checkpoint"));
}

