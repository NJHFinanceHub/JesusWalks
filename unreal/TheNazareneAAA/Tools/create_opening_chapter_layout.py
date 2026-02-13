"""Author opening chapter traversal and encounter blockout on NazareneCampaign.

Run:
  UnrealEditor-Cmd.exe TheNazareneAAA.uproject -run=pythonscript -script=Tools/create_opening_chapter_layout.py -unattended -nop4
"""

from __future__ import annotations

import math
import unreal

MAP_PATH = "/Game/Maps/NazareneCampaign"
BLOCKOUT_TAG = "OpeningChapterBlockout"


def _load_map() -> None:
    if not unreal.EditorAssetLibrary.does_asset_exist(MAP_PATH):
        raise RuntimeError(f"Map does not exist: {MAP_PATH}")
    unreal.EditorLoadingAndSavingUtils.load_map(MAP_PATH)


def _get_world() -> unreal.World:
    subsystem = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    world = subsystem.get_editor_world()
    if not world:
        raise RuntimeError("Editor world is unavailable.")
    return world


def _clear_previous_blockout(world: unreal.World) -> None:
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    actors = actor_subsystem.get_all_level_actors()
    for actor in actors:
        if actor and BLOCKOUT_TAG in actor.tags:
            actor_subsystem.destroy_actor(actor)


def _spawn_static_mesh(
    world: unreal.World,
    mesh_path: str,
    location: unreal.Vector,
    scale: unreal.Vector,
    rotation: unreal.Rotator = unreal.Rotator(0.0, 0.0, 0.0),
    color: unreal.LinearColor | None = None,
) -> unreal.StaticMeshActor:
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    actor = actor_subsystem.spawn_actor_from_class(unreal.StaticMeshActor, location, rotation)
    actor.tags = [BLOCKOUT_TAG]

    sm_component = actor.get_component_by_class(unreal.StaticMeshComponent)
    mesh = unreal.load_asset(mesh_path)
    sm_component.set_editor_property("static_mesh", mesh)
    sm_component.set_world_scale3d(scale)

    if color:
        material = unreal.load_asset("/Engine/BasicShapes/BasicShapeMaterial")
        sm_component.set_material(0, material)
        sm_component.set_color_parameter_value_on_materials("Color", color)

    return actor


def _spawn_marker(world: unreal.World, label: str, location: unreal.Vector) -> None:
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    marker = actor_subsystem.spawn_actor_from_class(unreal.TextRenderActor, location, unreal.Rotator(0.0, 0.0, 0.0))
    marker.tags = [BLOCKOUT_TAG]
    component = marker.get_component_by_class(unreal.TextRenderComponent)
    component.set_editor_property("text", unreal.Text(label))
    component.set_editor_property("horizontal_alignment", unreal.HorizTextAligment.EHTA_CENTER)
    component.set_editor_property("world_size", 64.0)
    component.set_editor_property("text_render_color", unreal.Color(255, 220, 180, 255))


def _ensure_player_start(world: unreal.World) -> None:
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    actors = actor_subsystem.get_all_level_actors()
    for actor in actors:
        if actor and actor.get_class().get_name() == "PlayerStart":
            return

    start = actor_subsystem.spawn_actor_from_class(
        unreal.PlayerStart,
        unreal.Vector(0.0, 0.0, 220.0),
        unreal.Rotator(0.0, 0.0, 0.0),
    )
    start.tags = [BLOCKOUT_TAG]


def _ensure_navmesh_bounds(world: unreal.World) -> None:
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    actors = actor_subsystem.get_all_level_actors()
    for actor in actors:
        if actor and actor.get_class().get_name() == "NavMeshBoundsVolume":
            return

    nav = actor_subsystem.spawn_actor_from_class(
        unreal.NavMeshBoundsVolume,
        unreal.Vector(0.0, -1200.0, 0.0),
        unreal.Rotator(0.0, 0.0, 0.0),
    )
    nav.tags = [BLOCKOUT_TAG]
    nav.set_actor_scale3d(unreal.Vector(55.0, 85.0, 10.0))


def _build_layout(world: unreal.World) -> None:
    earth = unreal.LinearColor(0.37, 0.31, 0.24, 1.0)
    stone = unreal.LinearColor(0.53, 0.49, 0.44, 1.0)
    accent = unreal.LinearColor(0.76, 0.65, 0.42, 1.0)

    # Main traversal ribbon from spawn to boss arena.
    _spawn_static_mesh(world, "/Engine/BasicShapes/Cube.Cube", unreal.Vector(0.0, -800.0, -55.0), unreal.Vector(14.0, 85.0, 1.0), color=earth)
    _spawn_static_mesh(world, "/Engine/BasicShapes/Cylinder.Cylinder", unreal.Vector(0.0, -2300.0, -40.0), unreal.Vector(12.0, 12.0, 0.4), color=accent)

    # Encounter 1: shore skirmish.
    _spawn_static_mesh(world, "/Engine/BasicShapes/Cube.Cube", unreal.Vector(500.0, -500.0, 80.0), unreal.Vector(1.8, 1.2, 2.8), color=stone)
    _spawn_static_mesh(world, "/Engine/BasicShapes/Cube.Cube", unreal.Vector(-500.0, -520.0, 80.0), unreal.Vector(1.8, 1.2, 2.8), color=stone)
    _spawn_marker(world, "Encounter 1: Shore Skirmish", unreal.Vector(0.0, -420.0, 250.0))

    # Encounter 2: ridge choke.
    for x in (-800.0, -300.0, 300.0, 800.0):
        _spawn_static_mesh(world, "/Engine/BasicShapes/Cylinder.Cylinder", unreal.Vector(x, -1400.0, 110.0), unreal.Vector(0.65, 0.65, 3.6), color=stone)
    _spawn_marker(world, "Encounter 2: Ridge Choke", unreal.Vector(0.0, -1360.0, 290.0))

    # Boss arena ring.
    for i in range(10):
        angle = i * 36.0
        radians = math.radians(angle)
        pos = unreal.Vector(math.cos(radians) * 1450.0, -2300.0 + math.sin(radians) * 1450.0, 40.0)
        _spawn_static_mesh(world, "/Engine/BasicShapes/Cylinder.Cylinder", pos, unreal.Vector(0.45, 0.45, 3.0), color=accent)
    _spawn_marker(world, "Encounter 3: Legion Sovereign Arena", unreal.Vector(0.0, -2300.0, 330.0))

    # Travel gate destination silhouette.
    _spawn_static_mesh(world, "/Engine/BasicShapes/Cube.Cube", unreal.Vector(0.0, 1900.0, 170.0), unreal.Vector(1.2, 1.2, 5.8), color=accent)
    _spawn_marker(world, "Travel Gate / Chapter Exit", unreal.Vector(0.0, 1900.0, 420.0))


def main() -> None:
    _load_map()
    world = _get_world()
    _clear_previous_blockout(world)
    _ensure_player_start(world)
    _ensure_navmesh_bounds(world)
    _build_layout(world)

    unreal.EditorAssetLibrary.save_asset(MAP_PATH, only_if_is_dirty=False)
    unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
    unreal.log("Opening chapter layout blockout complete.")


if __name__ == "__main__":
    main()
