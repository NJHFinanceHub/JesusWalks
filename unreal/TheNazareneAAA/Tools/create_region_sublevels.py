"""Create region sublevels used by campaign level streaming.

Run:
  UnrealEditor-Cmd.exe TheNazareneAAA.uproject -run=pythonscript -script=Tools/create_region_sublevels.py -unattended -nop4
"""

from __future__ import annotations

import unreal


REGION_LEVELS = [
    "/Game/Maps/Regions/Galilee/L_GalileeShores",
    "/Game/Maps/Regions/Decapolis/L_DecapolisRuins",
    "/Game/Maps/Regions/Wilderness/L_WildernessTemptation",
    "/Game/Maps/Regions/Jerusalem/L_JerusalemApproach",
]


def _ensure_directory(path: str) -> None:
    if not unreal.EditorAssetLibrary.does_directory_exist(path):
        unreal.EditorAssetLibrary.make_directory(path)


def _ensure_base_lighting() -> None:
    world = unreal.EditorLevelLibrary.get_editor_world()
    if not world:
        return

    existing_classes = {actor.get_class().get_name() for actor in unreal.EditorLevelLibrary.get_all_level_actors() if actor}

    if "DirectionalLight" not in existing_classes:
        unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.DirectionalLight, unreal.Vector(0.0, 0.0, 650.0), unreal.Rotator(-38.0, -30.0, 0.0))
    if "SkyLight" not in existing_classes:
        unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.SkyLight, unreal.Vector(0.0, 0.0, 320.0), unreal.Rotator(0.0, 0.0, 0.0))
    if "PostProcessVolume" not in existing_classes:
        volume = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.PostProcessVolume, unreal.Vector(0.0, 0.0, 0.0), unreal.Rotator(0.0, 0.0, 0.0))
        volume.set_editor_property("unbound", True)
        settings = volume.get_editor_property("settings")
        settings.set_editor_property("auto_exposure_bias", 0.2)
        volume.set_editor_property("settings", settings)
    if "NavMeshBoundsVolume" not in existing_classes:
        nav = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.NavMeshBoundsVolume, unreal.Vector(0.0, 0.0, 0.0), unreal.Rotator(0.0, 0.0, 0.0))
        nav.set_actor_scale3d(unreal.Vector(35.0, 35.0, 8.0))


def _create_or_update_region_level(level_path: str) -> None:
    region_dir = "/".join(level_path.split("/")[:-1])
    _ensure_directory(region_dir)

    if unreal.EditorAssetLibrary.does_asset_exist(level_path):
        unreal.EditorLoadingAndSavingUtils.load_map(level_path)
        unreal.log(f"Loaded existing region level: {level_path}")
    else:
        created = unreal.EditorLevelLibrary.new_level(level_path)
        if not created:
            raise RuntimeError(f"Failed to create level: {level_path}")
        unreal.log(f"Created region level: {level_path}")

    _ensure_base_lighting()
    unreal.EditorAssetLibrary.save_asset(level_path, only_if_is_dirty=False)


def main() -> None:
    _ensure_directory("/Game/Maps")
    _ensure_directory("/Game/Maps/Regions")

    for level_path in REGION_LEVELS:
        _create_or_update_region_level(level_path)

    unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
    unreal.log("Region sublevel generation complete.")


if __name__ == "__main__":
    main()
