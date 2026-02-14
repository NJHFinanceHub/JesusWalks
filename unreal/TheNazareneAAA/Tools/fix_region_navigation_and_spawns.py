"""Ensure region maps contain PlayerStart and NavMeshBoundsVolume placeholders.

Run:
  UnrealEditor-Cmd.exe TheNazareneAAA.uproject -run=pythonscript -script=Tools/fix_region_navigation_and_spawns.py -unattended -nop4
"""

from __future__ import annotations

import unreal

MAP_SPAWN_DATA: dict[str, unreal.Vector] = {
    "/Game/Maps/NazareneCampaign": unreal.Vector(0.0, 0.0, 220.0),
    "/Game/Maps/Regions/Galilee/L_GalileeShores": unreal.Vector(0.0, 0.0, 220.0),
    "/Game/Maps/Regions/Decapolis/L_DecapolisRuins": unreal.Vector(0.0, 0.0, 220.0),
    "/Game/Maps/Regions/Wilderness/L_WildernessTemptation": unreal.Vector(0.0, 0.0, 220.0),
    "/Game/Maps/Regions/Jerusalem/L_JerusalemApproach": unreal.Vector(0.0, 0.0, 220.0),
}


def _actors() -> list[unreal.Actor]:
    return [actor for actor in unreal.EditorLevelLibrary.get_all_level_actors() if actor]


def _ensure_player_start(spawn_location: unreal.Vector) -> bool:
    for actor in _actors():
        if isinstance(actor, unreal.PlayerStart):
            return False

    start = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.PlayerStart, spawn_location, unreal.Rotator(0.0, 0.0, 0.0))
    if start:
        start.set_actor_label("PlayerStart_Main")
        unreal.log(f"Added PlayerStart at {spawn_location}.")
        return True
    return False


def _ensure_nav_bounds(spawn_location: unreal.Vector) -> bool:
    for actor in _actors():
        if isinstance(actor, unreal.NavMeshBoundsVolume):
            return False

    nav_location = unreal.Vector(spawn_location.x, spawn_location.y, spawn_location.z - 40.0)
    nav_volume = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.NavMeshBoundsVolume, nav_location, unreal.Rotator(0.0, 0.0, 0.0))
    if nav_volume:
        nav_volume.set_actor_scale3d(unreal.Vector(40.0, 40.0, 10.0))
        nav_volume.set_actor_label("NavMeshBounds_Main")
        unreal.log(f"Added NavMeshBoundsVolume at {nav_location}.")
        return True
    return False


def main() -> None:
    updated_maps = 0
    for map_path, spawn_location in MAP_SPAWN_DATA.items():
        unreal.log(f"Processing map: {map_path}")
        unreal.EditorLevelLibrary.load_level(map_path)

        changed = False
        changed |= _ensure_player_start(spawn_location)
        changed |= _ensure_nav_bounds(spawn_location)

        if changed:
            try:
                unreal.EditorLevelLibrary.build_paths()
            except Exception as exc:  # pragma: no cover - editor behavior varies
                unreal.log_warning(f"Build paths failed for {map_path}: {exc}")

            unreal.EditorLevelLibrary.save_current_level()
            updated_maps += 1
            unreal.log(f"Saved map updates for {map_path}")
        else:
            unreal.log(f"No changes needed for {map_path}")

    unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
    unreal.log(f"Region navigation/spawn fix complete. Updated {updated_maps} map(s).")


if __name__ == "__main__":
    main()
