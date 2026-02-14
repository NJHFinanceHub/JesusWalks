#include "NazareneAssetResolver.h"

#include "Misc/ConfigCacheIni.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"

namespace
{
    const TCHAR* OverrideSection = TEXT("NazareneAssetOverrides");

    bool TryGetOverrideValue(const TCHAR* Key, FString& OutValue)
    {
        OutValue.Reset();

        const FString OverrideIniPath = FPaths::ProjectConfigDir() / TEXT("NazareneAssetOverrides.ini");
        if (GConfig->GetString(OverrideSection, Key, OutValue, OverrideIniPath) && !OutValue.TrimStartAndEnd().IsEmpty())
        {
            OutValue = OutValue.TrimStartAndEnd();
            return true;
        }

        if (GConfig->GetString(OverrideSection, Key, OutValue, GGameIni) && !OutValue.TrimStartAndEnd().IsEmpty())
        {
            OutValue = OutValue.TrimStartAndEnd();
            return true;
        }

        return false;
    }

    bool DoesAssetPackageExist(const FString& AnyAssetPath)
    {
        if (AnyAssetPath.IsEmpty())
        {
            return false;
        }

        FString PackagePath = AnyAssetPath;
        if (PackagePath.Contains(TEXT(".")))
        {
            PackagePath = FPackageName::ObjectPathToPackageName(PackagePath);
        }

        return FPackageName::DoesPackageExist(PackagePath);
    }

    FString NormalizeObjectPath(const FString& AnyAssetPath)
    {
        FString Normalized = AnyAssetPath;
        if (Normalized.IsEmpty())
        {
            return Normalized;
        }

        if (Normalized.Contains(TEXT(".")))
        {
            return Normalized;
        }

        const FString ShortName = FPackageName::GetShortName(Normalized);
        return FString::Printf(TEXT("%s.%s"), *Normalized, *ShortName);
    }
}

FString NazareneAssetResolver::ResolveObjectPath(const TCHAR* OverrideKey, const FString& DefaultObjectPath, std::initializer_list<const TCHAR*> CandidateObjectPaths)
{
    FString OverridePath;
    if (TryGetOverrideValue(OverrideKey, OverridePath) && DoesAssetPackageExist(OverridePath))
    {
        return NormalizeObjectPath(OverridePath);
    }

    if (DoesAssetPackageExist(DefaultObjectPath))
    {
        return DefaultObjectPath;
    }

    for (const TCHAR* CandidatePath : CandidateObjectPaths)
    {
        if (CandidatePath == nullptr)
        {
            continue;
        }

        const FString CandidateString(CandidatePath);
        if (!CandidateString.IsEmpty() && DoesAssetPackageExist(CandidateString))
        {
            return NormalizeObjectPath(CandidateString);
        }
    }

    return DefaultObjectPath;
}

FName NazareneAssetResolver::ResolveMapPackage(const TCHAR* OverrideKey, FName DefaultPackagePath, std::initializer_list<const TCHAR*> CandidatePackagePaths)
{
    FString OverridePath;
    if (TryGetOverrideValue(OverrideKey, OverridePath) && DoesAssetPackageExist(OverridePath))
    {
        FString PackagePath = OverridePath;
        if (PackagePath.Contains(TEXT(".")))
        {
            PackagePath = FPackageName::ObjectPathToPackageName(PackagePath);
        }
        return FName(*PackagePath);
    }

    if (DoesAssetPackageExist(DefaultPackagePath.ToString()))
    {
        return DefaultPackagePath;
    }

    for (const TCHAR* CandidatePath : CandidatePackagePaths)
    {
        if (CandidatePath == nullptr)
        {
            continue;
        }

        const FString CandidateString(CandidatePath);
        if (!CandidateString.IsEmpty() && DoesAssetPackageExist(CandidateString))
        {
            FString PackagePath = CandidateString;
            if (PackagePath.Contains(TEXT(".")))
            {
                PackagePath = FPackageName::ObjectPathToPackageName(PackagePath);
            }
            return FName(*PackagePath);
        }
    }

    return DefaultPackagePath;
}

