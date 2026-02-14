"""
Detects imported biblical/ancient UE assets and writes runtime overrides.

Run:
  UnrealEditor-Cmd.exe TheNazareneAAA.uproject -run=pythonscript -script=Tools/configure_biblical_starter_pack.py -unattended -nop4
"""

from __future__ import annotations

import os
import re
import unreal


CONFIG_PATH = os.path.join(unreal.Paths.project_config_dir(), "NazareneAssetOverrides.ini")
SECTION_NAME = "NazareneAssetOverrides"


def _class_name(asset: unreal.AssetData) -> str:
    class_path = asset.asset_class_path
    try:
        return str(class_path.asset_name)
    except Exception:
        return str(class_path)


def _asset_path(asset: unreal.AssetData) -> str:
    try:
        return asset.get_soft_object_path().to_string()
    except Exception:
        package_name = str(getattr(asset, "package_name", ""))
        asset_name = str(getattr(asset, "asset_name", ""))
        if package_name and asset_name:
            return f"{package_name}.{asset_name}"
        return asset_name or package_name


def _score_asset(asset: unreal.AssetData, include_terms: list[str], exclude_terms: list[str], bonus_terms: list[str]) -> int:
    name = str(asset.asset_name).lower()
    path = _asset_path(asset).lower()
    text = f"{name} {path}"

    score = 0
    for term in include_terms:
        if term in text:
            score += 4
    for term in bonus_terms:
        if term in text:
            score += 2
    for term in exclude_terms:
        if term in text:
            score -= 5

    # Prefer Fab-import style content directories.
    if "/fab/" in path or "/marketplace/" in path:
        score += 2
    if "/demo/" in path or "/test/" in path:
        score -= 2
    return score


def _choose_best(
    assets: list[unreal.AssetData],
    class_whitelist: set[str],
    include_terms: list[str],
    exclude_terms: list[str],
    bonus_terms: list[str] | None = None,
    min_score: int = 1,
) -> str:
    bonus_terms = bonus_terms or []
    candidates: list[tuple[int, str]] = []
    for asset in assets:
        if _class_name(asset) not in class_whitelist:
            continue
        score = _score_asset(asset, include_terms, exclude_terms, bonus_terms)
        if score >= min_score:
            candidates.append((score, _asset_path(asset)))

    if not candidates:
        return ""

    candidates.sort(key=lambda item: item[0], reverse=True)
    return candidates[0][1]


def _choose_top_maps(assets: list[unreal.AssetData], max_results: int = 4) -> list[str]:
    include = ["middle", "town", "city", "market", "village", "desert", "ruin", "ancient", "roman", "jerusalem", "galilee"]
    exclude = ["test", "example", "overview", "template"]
    weighted: list[tuple[int, str]] = []

    for asset in assets:
        if _class_name(asset) != "World":
            continue
        score = _score_asset(asset, include, exclude, bonus_terms=["map", "level", "pcg"])
        if score >= 1:
            weighted.append((score, _asset_path(asset)))

    weighted.sort(key=lambda item: item[0], reverse=True)
    return [path for _, path in weighted[:max_results]]


def _to_package_path(asset_path: str) -> str:
    if "." in asset_path:
        return asset_path.split(".", 1)[0]
    return asset_path


def _to_anim_class_path(asset_path: str) -> str:
    if not asset_path:
        return ""

    if "." not in asset_path:
        short_name = asset_path.rsplit("/", 1)[-1]
        return f"{asset_path}.{short_name}_C"

    package, object_name = asset_path.split(".", 1)
    if object_name.endswith("_C"):
        return asset_path
    return f"{package}.{object_name}_C"


def _write_overrides(overrides: dict[str, str]) -> None:
    lines: list[str] = [
        f"[{SECTION_NAME}]",
    ]
    for key, value in overrides.items():
        if value:
            lines.append(f"{key}={value}")

    os.makedirs(os.path.dirname(CONFIG_PATH), exist_ok=True)
    with open(CONFIG_PATH, "w", encoding="utf-8", newline="\n") as f:
        f.write("\n".join(lines).strip() + "\n")


def main() -> None:
    asset_registry = unreal.AssetRegistryHelpers.get_asset_registry()
    assets = asset_registry.get_assets_by_path("/Game", recursive=True)
    assets = list(assets)

    player_mesh = _choose_best(
        assets,
        {"SkeletalMesh"},
        include_terms=["jesus", "hero", "pilgrim", "apostle", "prophet", "oriental", "metahuman", "male"],
        exclude_terms=["enemy", "roman", "guard", "soldier", "demon", "monster", "zombie"],
        bonus_terms=["sk_", "body"],
    )

    enemy_mesh = _choose_best(
        assets,
        {"SkeletalMesh"},
        include_terms=["roman", "guard", "legion", "soldier", "warrior", "centurion", "bandit"],
        exclude_terms=["jesus", "hero", "metahuman", "female", "civilian"],
        bonus_terms=["sk_", "enemy"],
    )

    player_anim = _choose_best(
        assets,
        {"AnimBlueprint", "Blueprint"},
        include_terms=["abp", "anim", "locomotion", "thirdperson", "movement", "hero", "player"],
        exclude_terms=["enemy", "test", "demo"],
        bonus_terms=["character"],
    )

    enemy_anim = _choose_best(
        assets,
        {"AnimBlueprint", "Blueprint"},
        include_terms=["abp", "anim", "enemy", "roman", "legion", "guard"],
        exclude_terms=["test", "demo"],
        bonus_terms=["combat", "melee"],
    )
    if not enemy_anim:
        enemy_anim = player_anim

    env_block_mesh = _choose_best(
        assets,
        {"StaticMesh"},
        include_terms=["block", "wall", "stone", "brick", "modular"],
        exclude_terms=["weapon", "character", "skeletal", "vfx", "collision", "proxy"],
        bonus_terms=["sm_", "architecture", "building", "ruin"],
    )

    env_column_mesh = _choose_best(
        assets,
        {"StaticMesh"},
        include_terms=["column", "pillar", "roman", "greek", "temple"],
        exclude_terms=["weapon", "character", "skeletal", "vfx", "collision", "proxy"],
        bonus_terms=["sm_", "architecture", "ruin"],
    )

    env_tent_mesh = _choose_best(
        assets,
        {"StaticMesh"},
        include_terms=["tent", "awning", "canopy", "cloth", "stall"],
        exclude_terms=["weapon", "character", "skeletal", "vfx", "collision", "proxy"],
        bonus_terms=["sm_", "market"],
    )

    env_canopy_mesh = _choose_best(
        assets,
        {"StaticMesh"},
        include_terms=["olive", "tree", "palm", "foliage", "bush"],
        exclude_terms=["weapon", "character", "skeletal", "vfx", "collision", "proxy"],
        bonus_terms=["sm_", "environment"],
    )

    env_ground_mesh = _choose_best(
        assets,
        {"StaticMesh"},
        include_terms=["ground", "floor", "tile", "terrain", "road"],
        exclude_terms=["weapon", "character", "skeletal", "vfx", "collision", "proxy"],
        bonus_terms=["sm_", "environment", "modular"],
    )

    env_material_stone = _choose_best(
        assets,
        {"Material", "MaterialInstanceConstant", "MaterialInstance"},
        include_terms=["stone", "rock", "masonry", "wall", "ruin"],
        exclude_terms=["character", "skin", "face", "hair", "ui", "vfx"],
        bonus_terms=["mi_", "master", "tile"],
    )

    env_material_olive = _choose_best(
        assets,
        {"Material", "MaterialInstanceConstant", "MaterialInstance"},
        include_terms=["olive", "leaf", "foliage", "tree", "forest"],
        exclude_terms=["character", "skin", "face", "hair", "ui", "vfx"],
        bonus_terms=["mi_", "master", "nature"],
    )

    env_material_sand = _choose_best(
        assets,
        {"Material", "MaterialInstanceConstant", "MaterialInstance"},
        include_terms=["sand", "desert", "dirt", "soil", "dust"],
        exclude_terms=["character", "skin", "face", "hair", "ui", "vfx"],
        bonus_terms=["mi_", "master", "ground"],
    )

    map_candidates = _choose_top_maps(assets, max_results=4)
    primary_map = _to_package_path(map_candidates[0]) if map_candidates else ""
    if primary_map.startswith("/Game/Maps/Regions/"):
        primary_map = ""

    overrides = {
        "PlayerSkeletalMesh": player_mesh,
        "EnemySkeletalMesh": enemy_mesh,
        "PlayerAnimBlueprint": _to_anim_class_path(player_anim),
        "EnemyAnimBlueprint": _to_anim_class_path(enemy_anim),
        "EnvMeshBlock": env_block_mesh,
        "EnvMeshColumn": env_column_mesh,
        "EnvMeshTent": env_tent_mesh,
        "EnvMeshCanopy": env_canopy_mesh,
        "EnvMeshGround": env_ground_mesh,
        "EnvMaterialStone": env_material_stone,
        "EnvMaterialOlive": env_material_olive,
        "EnvMaterialSand": env_material_sand,
        "GalileeMap": primary_map,
        "DecapolisMap": primary_map,
        "WildernessMap": primary_map,
        "JerusalemMap": primary_map,
        "GethsemaneMap": primary_map,
        "ViaDolorosaMap": primary_map,
        "EmptyTombMap": primary_map,
    }

    _write_overrides(overrides)

    unreal.log(f"Wrote starter-pack override config: {CONFIG_PATH}")
    for key, value in overrides.items():
        unreal.log(f"  {key}: {value or '<not found>'}")

    if not any(v for v in overrides.values()):
        unreal.log_warning("No matching assets found under /Game. Import Fab packs first, then re-run this script.")


if __name__ == "__main__":
    try:
        main()
    except Exception as exc:
        unreal.log_error(f"configure_biblical_starter_pack.py failed: {exc}")
