"""Create project-local behavior tree asset placeholders.

Run:
  UnrealEditor-Cmd.exe TheNazareneAAA.uproject -run=pythonscript -script=Tools/create_behavior_tree_assets.py -unattended -nop4
"""

from __future__ import annotations

import unreal


def _ensure_dir(path: str) -> None:
    if not unreal.EditorAssetLibrary.does_directory_exist(path):
        unreal.EditorAssetLibrary.make_directory(path)


def _pick_source(candidates: list[str]) -> str | None:
    for path in candidates:
        if unreal.EditorAssetLibrary.does_asset_exist(path):
            return path
    return None


def _create_or_duplicate_bt(destination: str, candidates: list[str], blackboard: unreal.BlackboardData | None) -> None:
    if unreal.EditorAssetLibrary.does_asset_exist(destination):
        unreal.log(f"Already exists: {destination}")
        tree = unreal.load_asset(destination)
    else:
        tree = None
        source = _pick_source(candidates)
        if source and unreal.EditorAssetLibrary.duplicate_asset(source, destination):
            unreal.log(f"Duplicated BT: {source} -> {destination}")
            tree = unreal.load_asset(destination)
        else:
            asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
            package_path, asset_name = destination.rsplit("/", 1)
            tree = asset_tools.create_asset(asset_name, package_path, unreal.BehaviorTree, unreal.BehaviorTreeFactory())
            if tree:
                unreal.log(f"Created BT placeholder: {destination}")
            else:
                unreal.log_error(f"Failed to create behavior tree: {destination}")
                return

    if tree and blackboard:
        try:
            tree.set_editor_property("blackboard_asset", blackboard)
        except Exception:
            unreal.log_warning(f"Could not set blackboard on {destination}; continuing.")


def _create_or_load_blackboard(destination: str) -> unreal.BlackboardData | None:
    if unreal.EditorAssetLibrary.does_asset_exist(destination):
        asset = unreal.load_asset(destination)
        if asset:
            unreal.log(f"Already exists: {destination}")
            return asset

    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    package_path, asset_name = destination.rsplit("/", 1)
    asset = asset_tools.create_asset(asset_name, package_path, unreal.BlackboardData, unreal.BlackboardDataFactory())
    if asset:
        unreal.log(f"Created blackboard placeholder: {destination}")
    else:
        unreal.log_error(f"Failed to create blackboard: {destination}")
    return asset


def main() -> None:
    _ensure_dir("/Game/AI")
    _ensure_dir("/Game/AI/BehaviorTrees")
    _ensure_dir("/Game/AI/Blackboards")

    bt_candidates = [
        "/Engine/Tutorial/SubEditors/TutorialAssets/AI/Tutorial_BehaviorTree.Tutorial_BehaviorTree",
    ]

    enemy_blackboard = _create_or_load_blackboard("/Game/AI/Blackboards/BB_EnemyBase")

    _create_or_duplicate_bt("/Game/AI/BehaviorTrees/BT_MeleeShield", bt_candidates, enemy_blackboard)
    _create_or_duplicate_bt("/Game/AI/BehaviorTrees/BT_Spear", bt_candidates, enemy_blackboard)
    _create_or_duplicate_bt("/Game/AI/BehaviorTrees/BT_Ranged", bt_candidates, enemy_blackboard)
    _create_or_duplicate_bt("/Game/AI/BehaviorTrees/BT_Demon", bt_candidates, enemy_blackboard)
    _create_or_duplicate_bt("/Game/AI/BehaviorTrees/BT_Boss", bt_candidates, enemy_blackboard)

    unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
    unreal.log("Behavior tree placeholder generation complete.")


if __name__ == "__main__":
    main()
