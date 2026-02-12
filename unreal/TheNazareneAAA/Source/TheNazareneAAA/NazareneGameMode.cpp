#include "NazareneGameMode.h"
#include "NazareneHeroCharacter.h"

ANazareneGameMode::ANazareneGameMode()
{
    DefaultPawnClass = ANazareneHeroCharacter::StaticClass();
}
