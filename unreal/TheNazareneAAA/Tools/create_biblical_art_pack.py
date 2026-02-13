"""Create a project-local biblical art pack from available UE content.

Run:
  UnrealEditor-Cmd.exe TheNazareneAAA.uproject -run=pythonscript -script=Tools/create_biblical_art_pack.py -unattended -nop4
"""

from __future__ import annotations

import unreal


def _ensure_dir(path: str) -> None:
    if not unreal.EditorAssetLibrary.does_directory_exist(path):
        unreal.EditorAssetLibrary.make_directory(path)


def _duplicate_if_missing(source: str, destination: str) -> bool:
    if unreal.EditorAssetLibrary.does_asset_exist(destination):
        unreal.log(f"Already exists: {destination}")
        return True
    if not unreal.EditorAssetLibrary.does_asset_exist(source):
        unreal.log_error(f"Missing source asset: {source}")
        return False
    ok = unreal.EditorAssetLibrary.duplicate_asset(source, destination)
    if ok:
        unreal.log(f"Duplicated: {source} -> {destination}")
    else:
        unreal.log_error(f"Failed duplicate: {source} -> {destination}")
    return ok


def _create_material_instance(
    package_path: str,
    asset_name: str,
    parent_path: str,
    color: unreal.LinearColor,
) -> None:
    full_path = f"{package_path}/{asset_name}"
    if unreal.EditorAssetLibrary.does_asset_exist(full_path):
        unreal.log(f"Already exists: {full_path}")
        return

    parent = unreal.load_asset(parent_path)
    if parent is None:
        unreal.log_error(f"Missing material parent: {parent_path}")
        return

    tools = unreal.AssetToolsHelpers.get_asset_tools()
    factory = unreal.MaterialInstanceConstantFactoryNew()
    instance = tools.create_asset(asset_name, package_path, unreal.MaterialInstanceConstant, factory)
    if instance is None:
        unreal.log_error(f"Failed to create material instance: {full_path}")
        return

    instance.set_editor_property("parent", parent)
    unreal.MaterialEditingLibrary.set_material_instance_vector_parameter_value(instance, "Color", color)
    unreal.EditorAssetLibrary.save_loaded_asset(instance)
    unreal.log(f"Created material instance: {full_path}")


def main() -> None:
    asset_registry = unreal.AssetRegistryHelpers.get_asset_registry()
    asset_registry.scan_paths_synchronous(
        ["/Engine/Tutorial/SubEditors/TutorialAssets/Character", "/Engine/BasicShapes"],
        True,
    )

    _ensure_dir("/Game/Art")
    _ensure_dir("/Game/Art/Characters")
    _ensure_dir("/Game/Art/Characters/Player")
    _ensure_dir("/Game/Art/Characters/Enemies")
    _ensure_dir("/Game/Art/Characters/Common")
    _ensure_dir("/Game/Art/Environment")
    _ensure_dir("/Game/Art/Environment/Meshes")
    _ensure_dir("/Game/Art/Materials")
    _ensure_dir("/Game/Art/Animation")

    character_assets = [
        ("/Engine/Tutorial/SubEditors/TutorialAssets/Character/TutorialTPP.TutorialTPP", "/Game/Art/Characters/Player/SK_BiblicalHero"),
        ("/Engine/Tutorial/SubEditors/TutorialAssets/Character/TutorialTPP.TutorialTPP", "/Game/Art/Characters/Enemies/SK_BiblicalLegionary"),
        ("/Engine/Tutorial/SubEditors/TutorialAssets/Character/TutorialTPP.TutorialTPP", "/Game/Art/Characters/Common/SK_BiblicalHumanoid"),
        ("/Engine/Tutorial/SubEditors/TutorialAssets/Character/TutorialTPP_Skeleton.TutorialTPP_Skeleton", "/Game/Art/Characters/Common/SKEL_BiblicalHumanoid"),
        ("/Engine/Tutorial/SubEditors/TutorialAssets/Character/TutorialTPP_PhysicsAsset.TutorialTPP_PhysicsAsset", "/Game/Art/Characters/Common/PHYS_BiblicalHumanoid"),
        ("/Engine/Tutorial/SubEditors/TutorialAssets/Character/TutorialTPP_AnimBlueprint.TutorialTPP_AnimBlueprint", "/Game/Art/Animation/ABP_BiblicalHero"),
        ("/Engine/Tutorial/SubEditors/TutorialAssets/Character/TutorialTPP_AnimBlueprint.TutorialTPP_AnimBlueprint", "/Game/Art/Animation/ABP_BiblicalEnemy"),
        ("/Engine/Tutorial/SubEditors/TutorialAssets/Character/TutorialTPP_AnimBlueprint.TutorialTPP_AnimBlueprint", "/Game/Art/Animation/ABP_BiblicalHumanoid"),
        ("/Engine/Tutorial/SubEditors/TutorialAssets/Character/Tutorial_Idle.Tutorial_Idle", "/Game/Art/Animation/A_Biblical_Idle"),
        ("/Engine/Tutorial/SubEditors/TutorialAssets/Character/Tutorial_Walk_Fwd.Tutorial_Walk_Fwd", "/Game/Art/Animation/A_Biblical_WalkFwd"),
    ]

    environment_meshes = {
        "/Engine/BasicShapes/Cube.Cube": "/Game/Art/Environment/Meshes/SM_BiblicalBlock",
        "/Engine/BasicShapes/Cylinder.Cylinder": "/Game/Art/Environment/Meshes/SM_BiblicalColumn",
        "/Engine/BasicShapes/Cone.Cone": "/Game/Art/Environment/Meshes/SM_BiblicalTent",
        "/Engine/BasicShapes/Sphere.Sphere": "/Game/Art/Environment/Meshes/SM_BiblicalCanopy",
        "/Engine/BasicShapes/Plane.Plane": "/Game/Art/Environment/Meshes/SM_BiblicalFloorPlane",
    }

    for source, destination in character_assets:
        _duplicate_if_missing(source, destination)
    for source, destination in environment_meshes.items():
        _duplicate_if_missing(source, destination)

    parent = "/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"
    _create_material_instance("/Game/Art/Materials", "MI_Character_JesusRobe", parent, unreal.LinearColor(0.84, 0.80, 0.68, 1.0))
    _create_material_instance("/Game/Art/Materials", "MI_Character_RomanArmor", parent, unreal.LinearColor(0.48, 0.31, 0.24, 1.0))
    _create_material_instance("/Game/Art/Materials", "MI_Character_Demon", parent, unreal.LinearColor(0.18, 0.09, 0.09, 1.0))
    _create_material_instance("/Game/Art/Materials", "MI_Character_Boss", parent, unreal.LinearColor(0.55, 0.26, 0.17, 1.0))

    _create_material_instance("/Game/Art/Materials", "MI_Env_Stone", parent, unreal.LinearColor(0.56, 0.52, 0.45, 1.0))
    _create_material_instance("/Game/Art/Materials", "MI_Env_Sand", parent, unreal.LinearColor(0.64, 0.54, 0.36, 1.0))
    _create_material_instance("/Game/Art/Materials", "MI_Env_OliveLeaf", parent, unreal.LinearColor(0.22, 0.35, 0.21, 1.0))
    _create_material_instance("/Game/Art/Materials", "MI_Env_Wood", parent, unreal.LinearColor(0.37, 0.27, 0.17, 1.0))

    unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
    unreal.log("Biblical art pack creation complete.")


if __name__ == "__main__":
    main()
