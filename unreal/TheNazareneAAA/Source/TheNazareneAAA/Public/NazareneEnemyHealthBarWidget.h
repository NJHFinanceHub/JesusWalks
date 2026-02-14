#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NazareneEnemyHealthBarWidget.generated.h"

class ANazareneEnemyCharacter;
class ANazarenePlayerCharacter;
class UBorder;
class UProgressBar;
class UTextBlock;

UCLASS()
class THENAZARENEAAA_API UNazareneEnemyHealthBarWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeOnInitialized() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    void BindToEnemy(ANazareneEnemyCharacter* InEnemy, ANazarenePlayerCharacter* InPlayer);
    ANazareneEnemyCharacter* GetBoundEnemy() const { return Enemy.Get(); }

private:
    void UpdateVisibilityState(float DeltaSeconds, bool bLockedOn, float HealthRatio, float DistanceToPlayer);

private:
    UPROPERTY()
    TObjectPtr<UBorder> RootBorder;

    UPROPERTY()
    TObjectPtr<UTextBlock> EnemyNameText;

    UPROPERTY()
    TObjectPtr<UProgressBar> HealthBar;

    UPROPERTY()
    TObjectPtr<UProgressBar> PoiseBar;

    UPROPERTY()
    TWeakObjectPtr<ANazareneEnemyCharacter> Enemy;

    UPROPERTY()
    TWeakObjectPtr<ANazarenePlayerCharacter> Player;

    float LastHealthRatio = 1.0f;
    float AutoFadeTimer = 0.0f;
};
