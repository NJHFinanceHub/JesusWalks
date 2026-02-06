#include "NazareneHUD.h"

#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "NazareneHUDWidget.h"

void ANazareneHUD::BeginPlay()
{
    Super::BeginPlay();

    APlayerController* PlayerController = GetOwningPlayerController();
    if (PlayerController == nullptr)
    {
        return;
    }

    RuntimeWidget = CreateWidget<UNazareneHUDWidget>(PlayerController, UNazareneHUDWidget::StaticClass());
    if (RuntimeWidget == nullptr)
    {
        return;
    }

    RuntimeWidget->AddToViewport(0);
    RuntimeWidget->SetRegionName(RegionName);
    RuntimeWidget->SetObjective(Objective);
}

void ANazareneHUD::DrawHUD()
{
    Super::DrawHUD();
}

void ANazareneHUD::SetRegionName(const FString& InRegionName)
{
    RegionName = InRegionName;
    if (RuntimeWidget != nullptr)
    {
        RuntimeWidget->SetRegionName(InRegionName);
    }
}

void ANazareneHUD::SetObjective(const FString& InObjective)
{
    Objective = InObjective;
    if (RuntimeWidget != nullptr)
    {
        RuntimeWidget->SetObjective(InObjective);
    }
}

void ANazareneHUD::ShowMessage(const FString& InMessage, float Duration)
{
    if (RuntimeWidget != nullptr)
    {
        RuntimeWidget->ShowMessage(InMessage, Duration);
    }
}

bool ANazareneHUD::TogglePauseMenu()
{
    if (RuntimeWidget == nullptr)
    {
        return false;
    }

    const bool bNextVisible = !RuntimeWidget->IsPauseMenuVisible();
    SetPauseMenuVisible(bNextVisible);
    return bNextVisible;
}

void ANazareneHUD::SetPauseMenuVisible(bool bVisible)
{
    if (RuntimeWidget != nullptr)
    {
        RuntimeWidget->SetPauseMenuVisible(bVisible);
    }

    APlayerController* PlayerController = GetOwningPlayerController();
    if (PlayerController == nullptr)
    {
        return;
    }

    UGameplayStatics::SetGamePaused(this, bVisible);
    PlayerController->bShowMouseCursor = bVisible;

    if (bVisible)
    {
        FInputModeGameAndUI InputMode;
        InputMode.SetHideCursorDuringCapture(false);
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PlayerController->SetInputMode(InputMode);
    }
    else
    {
        PlayerController->SetInputMode(FInputModeGameOnly());
    }
}

bool ANazareneHUD::IsPauseMenuVisible() const
{
    return RuntimeWidget != nullptr && RuntimeWidget->IsPauseMenuVisible();
}

