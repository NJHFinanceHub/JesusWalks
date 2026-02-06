#include "NazareneEnemyAnimInstance.h"

#include "NazareneEnemyCharacter.h"

void UNazareneEnemyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    APawn* OwningPawn = TryGetPawnOwner();
    if (OwningPawn == nullptr)
    {
        GroundSpeed = 0.0f;
        State = ENazareneEnemyState::Idle;
        BossPhase = 1;
        bRedeemed = false;
        bParried = false;
        bStaggered = false;
        StateTimerRemaining = 0.0f;
        return;
    }

    FVector HorizontalVelocity = OwningPawn->GetVelocity();
    HorizontalVelocity.Z = 0.0f;
    GroundSpeed = HorizontalVelocity.Size();

    if (const ANazareneEnemyCharacter* Enemy = Cast<ANazareneEnemyCharacter>(OwningPawn))
    {
        State = Enemy->GetState();
        BossPhase = Enemy->BossPhase;
        bRedeemed = Enemy->IsRedeemed();
        bParried = Enemy->IsParried();
        bStaggered = State == ENazareneEnemyState::Staggered;
        StateTimerRemaining = Enemy->GetStateTimerRemaining();
    }
    else
    {
        State = ENazareneEnemyState::Idle;
        BossPhase = 1;
        bRedeemed = false;
        bParried = false;
        bStaggered = false;
        StateTimerRemaining = 0.0f;
    }
}

