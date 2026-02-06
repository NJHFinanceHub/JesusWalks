"""Create the initial campaign map and set game mode defaults.

Run from Unreal Editor:
  py "Tools/create_campaign_level.py"
or from command line:
  UnrealEditor-Cmd.exe TheNazareneAAA.uproject -run=pythonscript -script=Tools/create_campaign_level.py -unattended
"""

import unreal

MAP_PATH = "/Game/Maps/NazareneCampaign"
GAME_MODE_PATH = "/Script/TheNazareneAAA.NazareneCampaignGameMode"


def _ensure_content_directory(path: str) -> None:
    if not unreal.EditorAssetLibrary.does_directory_exist(path):
        unreal.EditorAssetLibrary.make_directory(path)


def _has_actor_of_class(actor_class: type) -> bool:
    for actor in unreal.EditorLevelLibrary.get_all_level_actors():
        if actor and actor.get_class() == actor_class:
            return True
    return False


def _spawn_if_missing(actor_class: type, location: unreal.Vector, rotation: unreal.Rotator) -> None:
    if _has_actor_of_class(actor_class):
        return
    actor = unreal.EditorLevelLibrary.spawn_actor_from_class(actor_class, location, rotation)
    if actor and actor_class == unreal.NavMeshBoundsVolume:
        actor.set_actor_scale3d(unreal.Vector(40.0, 40.0, 8.0))


def _create_or_open_map() -> None:
    _ensure_content_directory("/Game/Maps")

    if unreal.EditorAssetLibrary.does_asset_exist(MAP_PATH):
        unreal.EditorLoadingAndSavingUtils.load_map(MAP_PATH)
        unreal.log(f"Loaded existing campaign map: {MAP_PATH}")
        return

    created = unreal.EditorLevelLibrary.new_level(MAP_PATH)
    if not created:
        raise RuntimeError(f"Unable to create map at {MAP_PATH}")
    unreal.log(f"Created campaign map: {MAP_PATH}")


def _configure_world_settings() -> None:
    world = unreal.EditorLevelLibrary.get_editor_world()
    if not world:
        raise RuntimeError("Editor world is unavailable.")

    game_mode_class = unreal.load_class(None, GAME_MODE_PATH)
    if game_mode_class:
        world_settings = world.get_world_settings()
        world_settings.set_editor_property("default_game_mode", game_mode_class)
        unreal.log("Applied NazareneCampaignGameMode as level default game mode.")
    else:
        unreal.log_warning(f"Could not load game mode class: {GAME_MODE_PATH}")


def _seed_startup_actors() -> None:
    _spawn_if_missing(unreal.PlayerStart, unreal.Vector(0.0, 0.0, 180.0), unreal.Rotator(0.0, 0.0, 0.0))
    _spawn_if_missing(unreal.DirectionalLight, unreal.Vector(0.0, 0.0, 650.0), unreal.Rotator(-42.0, -30.0, 0.0))
    _spawn_if_missing(unreal.SkyLight, unreal.Vector(0.0, 0.0, 320.0), unreal.Rotator(0.0, 0.0, 0.0))
    _spawn_if_missing(unreal.NavMeshBoundsVolume, unreal.Vector(0.0, 0.0, 0.0), unreal.Rotator(0.0, 0.0, 0.0))


def _set_project_default_maps() -> None:
    maps_settings = unreal.GameMapsSettings.get_game_maps_settings()
    maps_settings.set_editor_property("game_default_map", MAP_PATH)
    maps_settings.set_editor_property("editor_startup_map", MAP_PATH)
    maps_settings.save_config()
    unreal.log(f"Updated project default maps to {MAP_PATH}")


def main() -> None:
    _create_or_open_map()
    _configure_world_settings()
    _seed_startup_actors()
    _set_project_default_maps()

    unreal.EditorAssetLibrary.save_asset(MAP_PATH, only_if_is_dirty=False)
    unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
    unreal.log("Campaign map bootstrap complete.")


if __name__ == "__main__":
    main()

