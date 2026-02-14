"""Enable `used_with_skeletal_mesh` on character materials and reparent unsafe MIs.

Run:
  UnrealEditor-Cmd.exe TheNazareneAAA.uproject -run=pythonscript -script=Tools/fix_skeletal_material_usage.py -unattended -nop4
"""

from __future__ import annotations

import unreal

CHARACTER_ROOTS = [
    "/Game/Art/Characters/Player",
    "/Game/Art/Characters/Enemies",
]
MATERIAL_ROOT = "/Game/Art/Materials"
PRIORITY_MATERIAL_INSTANCES = [
    "/Game/Art/Materials/MI_Character_RomanArmor",
]


def _iter_skeletal_meshes() -> list[str]:
    mesh_paths: list[str] = []
    for root in CHARACTER_ROOTS:
        if not unreal.EditorAssetLibrary.does_directory_exist(root):
            continue
        for asset in unreal.EditorAssetLibrary.list_assets(root, recursive=True, include_folder=False):
            obj = unreal.EditorAssetLibrary.load_asset(asset)
            if isinstance(obj, unreal.SkeletalMesh):
                mesh_paths.append(asset)
    return mesh_paths


def _resolve_base_material(material_interface: unreal.MaterialInterface | None) -> unreal.Material | None:
    if material_interface is None:
        return None
    if isinstance(material_interface, unreal.Material):
        return material_interface
    if isinstance(material_interface, unreal.MaterialInstance):
        return material_interface.get_base_material()
    return None


def _normalize_asset_path(path_or_object: str | unreal.Object) -> str:
    if isinstance(path_or_object, str):
        return path_or_object.split(".")[0]
    return path_or_object.get_path_name().split(".")[0]


def _get_or_create_project_material_copy(source_material: unreal.Material) -> unreal.Material | None:
    source_path = _normalize_asset_path(source_material)
    source_name = source_path.rsplit("/", 1)[-1]
    target_name = f"M_SK_{source_name}"
    target_path = f"{MATERIAL_ROOT}/{target_name}"

    if unreal.EditorAssetLibrary.does_asset_exist(target_path):
        copied = unreal.EditorAssetLibrary.load_asset(target_path)
        if isinstance(copied, unreal.Material):
            return copied
        return None

    if not unreal.EditorAssetLibrary.does_directory_exist(MATERIAL_ROOT):
        unreal.EditorAssetLibrary.make_directory(MATERIAL_ROOT)

    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    duplicated = asset_tools.duplicate_asset(target_name, MATERIAL_ROOT, source_material)
    if isinstance(duplicated, unreal.Material):
        unreal.log(f"Duplicated engine material for skeletal use: {source_path} -> {target_path}")
        return duplicated
    unreal.log_error(f"Failed to duplicate engine material for skeletal use: {source_path}")
    return None


def _ensure_skeletal_safe_material(base_material: unreal.Material, updated: set[str]) -> unreal.Material | None:
    base_path = _normalize_asset_path(base_material)

    if base_path.startswith("/Engine/"):
        copied = _get_or_create_project_material_copy(base_material)
        if copied is None:
            return None
        copied_path = _normalize_asset_path(copied)
        if not copied.get_editor_property("used_with_skeletal_mesh"):
            copied.set_editor_property("used_with_skeletal_mesh", True)
            updated.add(copied_path)
            unreal.log(f"Enabled used_with_skeletal_mesh on project copy: {copied_path}")
        return copied

    if not base_material.get_editor_property("used_with_skeletal_mesh"):
        base_material.set_editor_property("used_with_skeletal_mesh", True)
        updated.add(base_path)
        unreal.log(f"Enabled used_with_skeletal_mesh: {base_path}")
    return base_material


def _reparent_priority_instances(updated: set[str], updated_instances: set[str]) -> None:
    for instance_path in PRIORITY_MATERIAL_INSTANCES:
        if not unreal.EditorAssetLibrary.does_asset_exist(instance_path):
            unreal.log_warning(f"Priority MI not found: {instance_path}")
            continue

        asset = unreal.EditorAssetLibrary.load_asset(instance_path)
        if not isinstance(asset, unreal.MaterialInstance):
            unreal.log_warning(f"Asset is not a material instance: {instance_path}")
            continue

        base = asset.get_base_material()
        if base is None:
            unreal.log_warning(f"Material instance has no base material: {instance_path}")
            continue

        safe_parent = _ensure_skeletal_safe_material(base, updated)
        if safe_parent is None:
            continue

        current_parent = asset.get_editor_property("parent")
        current_parent_path = _normalize_asset_path(current_parent) if current_parent else ""
        safe_parent_path = _normalize_asset_path(safe_parent)
        if current_parent_path != safe_parent_path:
            asset.set_editor_property("parent", safe_parent)
            updated_instances.add(instance_path)
            unreal.log(f"Reparented priority MI for skeletal usage: {instance_path} -> {safe_parent_path}")


def _iter_material_instances(root: str) -> list[str]:
    instance_paths: list[str] = []
    if not unreal.EditorAssetLibrary.does_directory_exist(root):
        return instance_paths

    for asset_path in unreal.EditorAssetLibrary.list_assets(root, recursive=True, include_folder=False):
        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if isinstance(asset, unreal.MaterialInstance):
            instance_paths.append(asset_path)
    return instance_paths


def _repair_material_instance(instance_path: str, updated: set[str], updated_instances: set[str]) -> None:
    asset = unreal.EditorAssetLibrary.load_asset(instance_path)
    if not isinstance(asset, unreal.MaterialInstance):
        return

    base = asset.get_base_material()
    if base is None:
        return

    safe_parent = _ensure_skeletal_safe_material(base, updated)
    if safe_parent is None:
        return

    current_parent = asset.get_editor_property("parent")
    current_parent_path = _normalize_asset_path(current_parent) if current_parent else ""
    safe_parent_path = _normalize_asset_path(safe_parent)

    if current_parent_path != safe_parent_path:
        asset.set_editor_property("parent", safe_parent)
        updated_instances.add(instance_path)
        unreal.log(f"Reparented material instance for skeletal usage: {instance_path} -> {safe_parent_path}")


def main() -> None:
    updated: set[str] = set()
    updated_instances: set[str] = set()

    _reparent_priority_instances(updated, updated_instances)
    for instance_path in _iter_material_instances(MATERIAL_ROOT):
        _repair_material_instance(instance_path, updated, updated_instances)

    for mesh_path in _iter_skeletal_meshes():
        mesh = unreal.EditorAssetLibrary.load_asset(mesh_path)
        if not isinstance(mesh, unreal.SkeletalMesh):
            continue

        for material_slot in mesh.get_editor_property("materials"):
            material_interface = material_slot.get_editor_property("material_interface")
            base_material = _resolve_base_material(material_interface)
            if base_material is None:
                continue

            if isinstance(material_interface, unreal.MaterialInstance):
                instance_path = _normalize_asset_path(material_interface)
                _repair_material_instance(instance_path, updated, updated_instances)
                continue

            _ensure_skeletal_safe_material(base_material, updated)

    for asset_path in sorted(updated):
        unreal.EditorAssetLibrary.save_asset(asset_path, only_if_is_dirty=False)
    for instance_path in sorted(updated_instances):
        unreal.EditorAssetLibrary.save_asset(instance_path, only_if_is_dirty=False)

    unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
    unreal.log(
        "Skeletal material usage fix complete. "
        f"Updated {len(updated)} materials and {len(updated_instances)} material instances."
    )


if __name__ == "__main__":
    main()
