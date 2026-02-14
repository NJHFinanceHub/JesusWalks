#pragma once

#include "CoreMinimal.h"

namespace NazareneAssetResolver
{
    FString ResolveObjectPath(const TCHAR* OverrideKey, const FString& DefaultObjectPath, std::initializer_list<const TCHAR*> CandidateObjectPaths);
    FName ResolveMapPackage(const TCHAR* OverrideKey, FName DefaultPackagePath, std::initializer_list<const TCHAR*> CandidatePackagePaths);
}

