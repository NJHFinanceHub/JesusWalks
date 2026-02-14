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
    "/Game/Maps/Regions/Gethsemane/L_GardenGethsemane",
    "/Game/Maps/Regions/ViaDolorosa/L_ViaDolorosa",
    "/Game/Maps/Regions/EmptyTomb/L_EmptyTomb",
]


def _ensure_directory(path: str) -> None:
    if not unreal.EditorAssetLibrary.does_directory_exist(path):
        unreal.EditorAssetLibrary.make_directory(path)


def _region_profile(level_path: str) -> dict[str, float]:
    level_key = level_path.lower()
    profile: dict[str, float] = {
        "sun_pitch": -38.0,
        "sun_yaw": -30.0,
        "sun_intensity": 10.0,
        "sun_temperature": 6000.0,
        "sky_intensity": 1.0,
        "fog_density": 0.014,
        "exposure_bias": 0.2,
        "ground_width": 64.0,
        "ground_length": 64.0,
    }

    if "galilee" in level_key:
        profile.update({"sun_intensity": 10.6, "sun_temperature": 5900.0, "fog_density": 0.012, "exposure_bias": 0.30})
    elif "decapolis" in level_key:
        profile.update({"sun_intensity": 9.8, "sun_temperature": 6050.0, "fog_density": 0.016, "exposure_bias": 0.10})
    elif "wilderness" in level_key:
        profile.update({"sun_pitch": -44.0, "sun_yaw": -25.0, "sun_intensity": 11.5, "sun_temperature": 5200.0, "fog_density": 0.010, "exposure_bias": 0.40})
    elif "jerusalem" in level_key:
        profile.update({"sun_pitch": -34.0, "sun_yaw": -38.0, "sun_intensity": 10.2, "sun_temperature": 5750.0, "fog_density": 0.013, "exposure_bias": 0.24})
    elif "gethsemane" in level_key:
        profile.update({"sun_pitch": -18.0, "sun_yaw": 35.0, "sun_intensity": 4.8, "sun_temperature": 7600.0, "sky_intensity": 0.55, "fog_density": 0.022, "exposure_bias": -0.55})
    elif "viadolorosa" in level_key or "via_dolorosa" in level_key:
        profile.update({"sun_pitch": -30.0, "sun_yaw": -45.0, "sun_intensity": 8.6, "sun_temperature": 5600.0, "fog_density": 0.017, "exposure_bias": 0.0})
    elif "emptytomb" in level_key or "empty_tomb" in level_key:
        profile.update({"sun_pitch": -10.0, "sun_yaw": 20.0, "sun_intensity": 13.2, "sun_temperature": 6500.0, "sky_intensity": 1.1, "fog_density": 0.008, "exposure_bias": 0.45})

    return profile


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
    if "ExponentialHeightFog" not in existing_classes:
        unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.ExponentialHeightFog, unreal.Vector(0.0, 0.0, -100.0), unreal.Rotator(0.0, 0.0, 0.0))
    if "NavMeshBoundsVolume" not in existing_classes:
        nav = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.NavMeshBoundsVolume, unreal.Vector(0.0, 0.0, 0.0), unreal.Rotator(0.0, 0.0, 0.0))
        nav.set_actor_scale3d(unreal.Vector(35.0, 35.0, 8.0))


def _apply_region_lighting(level_path: str) -> None:
    profile = _region_profile(level_path)
    for actor in unreal.EditorLevelLibrary.get_all_level_actors():
        if not actor:
            continue

        class_name = actor.get_class().get_name()
        if class_name == "DirectionalLight":
            actor.set_actor_rotation(unreal.Rotator(profile["sun_pitch"], profile["sun_yaw"], 0.0), False)
            light_component = actor.get_editor_property("directional_light_component")
            light_component.set_editor_property("intensity", profile["sun_intensity"])
            light_component.set_editor_property("temperature", profile["sun_temperature"])
        elif class_name == "SkyLight":
            light_component = actor.get_editor_property("light_component")
            light_component.set_editor_property("intensity", profile["sky_intensity"])
        elif class_name == "ExponentialHeightFog":
            fog_component = actor.get_editor_property("component")
            fog_component.set_editor_property("fog_density", profile["fog_density"])
            fog_component.set_editor_property("fog_height_falloff", 0.24)
        elif class_name == "PostProcessVolume":
            settings = actor.get_editor_property("settings")
            settings.set_editor_property("auto_exposure_bias", profile["exposure_bias"])
            actor.set_editor_property("settings", settings)


def _ensure_blockout(level_path: str) -> None:
    if any(actor and any(tag == "AutoBlockout" for tag in actor.tags) for actor in unreal.EditorLevelLibrary.get_all_level_actors()):
        return

    profile = _region_profile(level_path)
    cube_mesh = unreal.EditorAssetLibrary.load_asset("/Engine/BasicShapes/Cube.Cube")
    if not cube_mesh:
        unreal.log_warning(f"Cube mesh not found for blockout in {level_path}")
        return

    blockout_points = [
        (unreal.Vector(0.0, 0.0, -120.0), unreal.Vector(profile["ground_width"], profile["ground_length"], 1.0)),
        (unreal.Vector(0.0, 900.0, 40.0), unreal.Vector(10.0, 1.0, 4.0)),
        (unreal.Vector(0.0, -900.0, 40.0), unreal.Vector(10.0, 1.0, 4.0)),
        (unreal.Vector(-900.0, 0.0, 40.0), unreal.Vector(1.0, 10.0, 4.0)),
        (unreal.Vector(900.0, 0.0, 40.0), unreal.Vector(1.0, 10.0, 4.0)),
    ]

    for location, scale in blockout_points:
        actor = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.StaticMeshActor, location, unreal.Rotator(0.0, 0.0, 0.0))
        if not actor:
            continue
        static_mesh_component = actor.get_component_by_class(unreal.StaticMeshComponent)
        if static_mesh_component:
            static_mesh_component.set_static_mesh(cube_mesh)
        actor.set_actor_scale3d(scale)
        actor.tags = list(actor.tags) + ["AutoBlockout"]


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
    _apply_region_lighting(level_path)
    _ensure_blockout(level_path)
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
