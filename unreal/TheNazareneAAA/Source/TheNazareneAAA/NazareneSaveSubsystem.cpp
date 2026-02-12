#include "NazareneSaveSubsystem.h"

#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "NazareneCampaignSubsystem.h"
#include "NazareneHeroCharacter.h"

bool UNazareneSaveSubsystem::SaveCurrentRun(const FString& SlotName, const int32 UserIndex)
{
    if (SlotName.IsEmpty())
    {
        return false;
    }

    UNazareneSaveGame* SaveGameObject = Cast<UNazareneSaveGame>(UGameplayStatics::CreateSaveGameObject(UNazareneSaveGame::StaticClass()));
    if (!SaveGameObject || !PopulateSaveGame(SaveGameObject))
    {
        return false;
    }

    return UGameplayStatics::SaveGameToSlot(SaveGameObject, SlotName, UserIndex);
}

bool UNazareneSaveSubsystem::LoadRun(const FString& SlotName, const int32 UserIndex)
{
    if (SlotName.IsEmpty() || !UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex))
    {
        return false;
    }

    const UNazareneSaveGame* SaveGameObject = Cast<UNazareneSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex));
    if (!SaveGameObject)
    {
        return false;
    }

    return ApplySaveGame(SaveGameObject);
}

bool UNazareneSaveSubsystem::PopulateSaveGame(UNazareneSaveGame* SaveGameObject) const
{
    if (!SaveGameObject)
    {
        return false;
    }

    if (ANazareneHeroCharacter* Hero = ResolvePlayerCharacter())
    {
        SaveGameObject->PlayerHealth = Hero->CurrentHealth;
        SaveGameObject->PlayerFaith = Hero->CurrentFaith;
        SaveGameObject->PlayerWorldLocation = Hero->GetActorLocation();
    }

    if (const UNazareneCampaignSubsystem* Campaign = ResolveCampaignSubsystem())
    {
        SaveGameObject->CampaignState = Campaign->GetCampaignState();
    }

    return true;
}

bool UNazareneSaveSubsystem::ApplySaveGame(const UNazareneSaveGame* SaveGameObject) const
{
    if (!SaveGameObject)
    {
        return false;
    }

    if (ANazareneHeroCharacter* Hero = ResolvePlayerCharacter())
    {
        Hero->CurrentHealth = FMath::Clamp(SaveGameObject->PlayerHealth, 0.0f, Hero->MaxHealth);
        Hero->CurrentFaith = FMath::Clamp(SaveGameObject->PlayerFaith, 0.0f, Hero->MaxFaith);
        Hero->SetActorLocation(SaveGameObject->PlayerWorldLocation);
    }

    if (UNazareneCampaignSubsystem* Campaign = ResolveCampaignSubsystem())
    {
        Campaign->ApplyLoadedCampaignState(SaveGameObject->CampaignState);
    }

    return true;
}

ANazareneHeroCharacter* UNazareneSaveSubsystem::ResolvePlayerCharacter() const
{
    if (!GetGameInstance() || !GetGameInstance()->GetWorld())
    {
        return nullptr;
    }

    const APlayerController* PlayerController = GetGameInstance()->GetWorld()->GetFirstPlayerController();
    if (!PlayerController)
    {
        return nullptr;
    }

    return Cast<ANazareneHeroCharacter>(PlayerController->GetPawn());
}

UNazareneCampaignSubsystem* UNazareneSaveSubsystem::ResolveCampaignSubsystem() const
{
    return GetGameInstance() ? GetGameInstance()->GetSubsystem<UNazareneCampaignSubsystem>() : nullptr;
}
