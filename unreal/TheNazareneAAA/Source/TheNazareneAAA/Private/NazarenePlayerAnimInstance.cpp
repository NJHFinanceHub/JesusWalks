#include "NazarenePlayerAnimInstance.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NazarenePlayerCharacter.h"

void UNazarenePlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    APawn* OwningPawn = TryGetPawnOwner();
    if (OwningPawn == nullptr)
    {
        GroundSpeed = 0.0f;
        ForwardSpeed = 0.0f;
        RightSpeed = 0.0f;
        bInAir = false;
        bBlocking = false;
        bDodging = false;
        bAttacking = false;
        bLockedOn = false;
        bDefeated = false;
        HurtTimeRemaining = 0.0f;
        return;
    }

    FVector HorizontalVelocity = OwningPawn->GetVelocity();
    HorizontalVelocity.Z = 0.0f;
    GroundSpeed = HorizontalVelocity.Size();

    const FVector ActorForward = OwningPawn->GetActorForwardVector().GetSafeNormal2D();
    const FVector ActorRight = OwningPawn->GetActorRightVector().GetSafeNormal2D();
    ForwardSpeed = FVector::DotProduct(HorizontalVelocity, ActorForward);
    RightSpeed = FVector::DotProduct(HorizontalVelocity, ActorRight);

    if (const ACharacter* Character = Cast<ACharacter>(OwningPawn))
    {
        const UCharacterMovementComponent* Movement = Character->GetCharacterMovement();
        bInAir = Movement != nullptr && Movement->IsFalling();
    }
    else
    {
        bInAir = false;
    }

    if (const ANazarenePlayerCharacter* Player = Cast<ANazarenePlayerCharacter>(OwningPawn))
    {
        bBlocking = Player->IsBlocking();
        bDodging = Player->IsDodging();
        bAttacking = Player->IsAttacking();
        bLockedOn = Player->HasLockTarget();
        bDefeated = Player->IsDefeated();
        HurtTimeRemaining = Player->GetHurtTimeRemaining();
    }
    else
    {
        bBlocking = false;
        bDodging = false;
        bAttacking = false;
        bLockedOn = false;
        bDefeated = false;
        HurtTimeRemaining = 0.0f;
    }
}

