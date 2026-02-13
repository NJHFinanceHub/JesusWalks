"""Validate Unreal asset naming and baseline quality rules.

Run headless:
  UnrealEditor-Cmd.exe TheNazareneAAA.uproject -run=pythonscript -script=Tools/validate_asset_pipeline.py -unattended
"""

from __future__ import annotations

import unreal

ROOT_PATH = "/Game"
EXCLUDED_PREFIXES = (
    "/Game/Developers",
    "/Game/Collections",
)

CLASS_PREFIX_RULES = {
    "StaticMesh": "SM_",
    "SkeletalMesh": "SK_",
    "Material": "M_",
    "MaterialInstanceConstant": "MI_",
    "Texture2D": "T_",
    "AnimSequence": "A_",
    "NiagaraSystem": "NS_",
    "SoundWave": "S_",
    "Blueprint": "BP_",
}

warnings: list[str] = []
failures: list[str] = []


def _is_excluded(asset_path: str) -> bool:
    return any(asset_path.startswith(prefix) for prefix in EXCLUDED_PREFIXES)


def _check_prefix(asset_data: unreal.AssetData) -> None:
    class_name = asset_data.asset_class_path.asset_name
    expected_prefix = CLASS_PREFIX_RULES.get(class_name)
    if not expected_prefix:
        return

    if not asset_data.asset_name.startswith(expected_prefix):
        failures.append(
            f"{asset_data.object_path}: expected name prefix '{expected_prefix}' for class '{class_name}'"
        )


def _check_static_mesh(asset_path: str) -> None:
    mesh = unreal.EditorAssetLibrary.load_asset(asset_path)
    if not mesh:
        failures.append(f"{asset_path}: static mesh could not be loaded")
        return

    lod_count = int(mesh.get_num_lods())
    if lod_count < 2:
        warnings.append(f"{asset_path}: static mesh has {lod_count} LOD(s); recommended >= 2")

    body_setup = mesh.get_editor_property("body_setup")
    if body_setup is None:
        warnings.append(f"{asset_path}: no body setup found (collision likely missing)")
        return

    agg_geom = body_setup.get_editor_property("agg_geom")
    has_simple_collision = False
    if agg_geom:
        shape_lists = (
            agg_geom.get_editor_property("box_elems"),
            agg_geom.get_editor_property("sphere_elems"),
            agg_geom.get_editor_property("sphyl_elems"),
            agg_geom.get_editor_property("convex_elems"),
            agg_geom.get_editor_property("tapered_capsule_elems"),
        )
        has_simple_collision = any(len(shape_list) > 0 for shape_list in shape_lists)

    if not has_simple_collision:
        warnings.append(f"{asset_path}: no simple collision shapes detected")


def main() -> None:
    asset_registry = unreal.AssetRegistryHelpers.get_asset_registry()
    assets = asset_registry.get_assets_by_path(ROOT_PATH, recursive=True)

    checked_count = 0
    for asset_data in assets:
        asset_path = str(asset_data.package_name)
        if _is_excluded(asset_path):
            continue

        checked_count += 1
        _check_prefix(asset_data)

        class_name = asset_data.asset_class_path.asset_name
        if class_name == "StaticMesh":
            _check_static_mesh(asset_data.object_path)

    for item in warnings:
        unreal.log_warning(item)
    for item in failures:
        unreal.log_error(item)

    unreal.log(
        f"Asset validation complete. Checked={checked_count}, Warnings={len(warnings)}, Failures={len(failures)}"
    )

    if failures:
        raise RuntimeError(f"Asset validation failed with {len(failures)} failure(s)")


if __name__ == "__main__":
    main()
