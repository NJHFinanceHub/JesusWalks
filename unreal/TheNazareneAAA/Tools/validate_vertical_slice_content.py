"""Validate Opening Chapter vertical-slice content conventions.

Run headless:
  UnrealEditor-Cmd.exe TheNazareneAAA.uproject -run=pythonscript -script=Tools/validate_vertical_slice_content.py -unattended -nop4
"""

from __future__ import annotations

import unreal

REQUIRED_MAPS = (
    "/Game/Maps/NazareneCampaign",
)

REQUIRED_SKELETAL_MESH_PREFIXES = (
    "SK_Player_",
    "SK_Enemy_",
)

REQUIRED_SOUND_CUES = (
    "SC_BGM_Peace",
    "SC_BGM_Tension",
    "SC_BGM_Combat",
    "SC_BGM_Boss",
    "SC_BGM_Victory",
)

REQUIRED_MUSIC_SOUNDWAVES = (
    "S_BGM_Peace_",
    "S_BGM_Tension_",
    "S_BGM_Combat_",
    "S_BGM_Boss_",
)

warnings: list[str] = []
failures: list[str] = []


def _all_assets() -> list[unreal.AssetData]:
    registry = unreal.AssetRegistryHelpers.get_asset_registry()
    return list(registry.get_assets_by_path("/Game", recursive=True))


def _validate_maps() -> None:
    for map_path in REQUIRED_MAPS:
        if not unreal.EditorAssetLibrary.does_asset_exist(map_path):
            failures.append(f"Required map missing: {map_path}")


def _validate_skeletal_meshes(assets: list[unreal.AssetData]) -> None:
    meshes = [a for a in assets if a.asset_class_path.asset_name == "SkeletalMesh"]
    mesh_names = [str(a.asset_name) for a in meshes]

    for prefix in REQUIRED_SKELETAL_MESH_PREFIXES:
        if not any(name.startswith(prefix) for name in mesh_names):
            warnings.append(f"No skeletal mesh found with prefix '{prefix}'")



def _validate_music_assets(assets: list[unreal.AssetData]) -> None:
    cue_names = {str(a.asset_name) for a in assets if a.asset_class_path.asset_name == "SoundCue"}
    wave_names = {str(a.asset_name) for a in assets if a.asset_class_path.asset_name == "SoundWave"}

    for required_cue in REQUIRED_SOUND_CUES:
        if required_cue not in cue_names:
            warnings.append(f"Missing recommended music cue: {required_cue}")

    for required_wave_prefix in REQUIRED_MUSIC_SOUNDWAVES:
        if not any(name.startswith(required_wave_prefix) for name in wave_names):
            warnings.append(f"No music SoundWave found with prefix '{required_wave_prefix}'")



def main() -> None:
    assets = _all_assets()
    _validate_maps()
    _validate_skeletal_meshes(assets)
    _validate_music_assets(assets)

    for item in warnings:
        unreal.log_warning(item)
    for item in failures:
        unreal.log_error(item)

    unreal.log(
        "Vertical-slice validation complete. "
        f"Assets={len(assets)}, Warnings={len(warnings)}, Failures={len(failures)}"
    )

    if failures:
        raise RuntimeError(f"Vertical-slice validation failed with {len(failures)} error(s)")


if __name__ == "__main__":
    main()
