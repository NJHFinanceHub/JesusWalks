#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "NazareneSaveGame.h"
#include "NazareneSaveSubsystem.generated.h"

class ANazareneHeroCharacter;
class UNazareneCampaignSubsystem;

UCLASS()
class THENAZARENEAAA_API UNazareneSaveSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    bool SaveCurrentRun(const FString& SlotName, int32 UserIndex = 0);

    UFUNCTION(BlueprintCallable)
    bool LoadRun(const FString& SlotName, int32 UserIndex = 0);

private:
    bool PopulateSaveGame(UNazareneSaveGame* SaveGameObject) const;
    bool ApplySaveGame(const UNazareneSaveGame* SaveGameObject) const;

    ANazareneHeroCharacter* ResolvePlayerCharacter() const;
    UNazareneCampaignSubsystem* ResolveCampaignSubsystem() const;
};
