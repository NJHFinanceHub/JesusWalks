"""Create baseline combat/music audio assets for runtime slot wiring.

The script duplicates available engine sounds into project-local naming targets.

Run:
  UnrealEditor-Cmd.exe TheNazareneAAA.uproject -run=pythonscript -script=Tools/create_audio_pack.py -unattended -nop4
"""

from __future__ import annotations

import unreal


def _ensure_dir(path: str) -> None:
    if not unreal.EditorAssetLibrary.does_directory_exist(path):
        unreal.EditorAssetLibrary.make_directory(path)


def _discover_engine_audio_assets(limit: int = 12) -> list[str]:
    registry = unreal.AssetRegistryHelpers.get_asset_registry()
    registry.scan_paths_synchronous(["/Engine"], True)
    assets = registry.get_assets_by_path("/Engine", recursive=True)
    results: list[str] = []
    for data in assets:
        class_name = str(getattr(data, "asset_class", ""))
        if hasattr(data, "asset_class_path"):
            class_name = str(data.asset_class_path.asset_name)
        if class_name in ("SoundWave", "SoundCue", "MetaSoundSource"):
            object_path = f"{data.package_name}.{data.asset_name}"
            results.append(str(object_path))
        if len(results) >= limit:
            break
    return results


def _first_existing(paths: list[str]) -> str | None:
    for path in paths:
        if unreal.EditorAssetLibrary.does_asset_exist(path):
            return path
    return None


def _duplicate_with_fallback(destination: str, candidates: list[str]) -> None:
    if unreal.EditorAssetLibrary.does_asset_exist(destination):
        unreal.log(f"Already exists: {destination}")
        return

    source = _first_existing(candidates)
    if source is None:
        unreal.log_warning(f"No source found for {destination}")
        return

    if unreal.EditorAssetLibrary.duplicate_asset(source, destination):
        unreal.log(f"Duplicated: {source} -> {destination}")
    else:
        unreal.log_error(f"Failed duplicate: {source} -> {destination}")


def main() -> None:
    _ensure_dir("/Game/Audio")
    _ensure_dir("/Game/Audio/SFX")
    _ensure_dir("/Game/Audio/Music")

    discovered = _discover_engine_audio_assets()
    if not discovered:
        unreal.log_warning("No engine audio assets discovered; audio placeholders cannot be generated.")
        return

    sfx_sources = discovered

    _duplicate_with_fallback("/Game/Audio/SFX/S_AttackWhoosh", sfx_sources)
    _duplicate_with_fallback("/Game/Audio/SFX/S_Impact", sfx_sources)
    _duplicate_with_fallback("/Game/Audio/SFX/S_Dodge", sfx_sources)
    _duplicate_with_fallback("/Game/Audio/SFX/S_Hurt", sfx_sources)
    _duplicate_with_fallback("/Game/Audio/SFX/S_Parry", sfx_sources)
    _duplicate_with_fallback("/Game/Audio/SFX/S_MiracleShimmer", sfx_sources)

    _duplicate_with_fallback("/Game/Audio/Music/S_Music_Galilee", sfx_sources)
    _duplicate_with_fallback("/Game/Audio/Music/S_Music_Decapolis", sfx_sources)
    _duplicate_with_fallback("/Game/Audio/Music/S_Music_Wilderness", sfx_sources)
    _duplicate_with_fallback("/Game/Audio/Music/S_Music_Jerusalem", sfx_sources)

    unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
    unreal.log("Audio pack generation complete.")


if __name__ == "__main__":
    main()
