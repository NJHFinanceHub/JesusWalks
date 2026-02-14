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

def _choose_region_map(assets: list[unreal.AssetData], region_terms: list[str], fallback_package_path: str) -> str:
    weighted: list[tuple[int, str]] = []
    include = ["map", "level", "region", "ancient", "town", "city", "village", "desert", "ruin"] + region_terms
    exclude = ["test", "example", "overview", "template"]

    for asset in assets:
        if _class_name(asset) != "World":
            continue
        path = _asset_path(asset)
        score = _score_asset(asset, include, exclude, bonus_terms=region_terms)
        if score < 1:
            continue
        weighted.append((score, path))

    if not weighted:
        return fallback_package_path

    weighted.sort(key=lambda item: item[0], reverse=True)
    package = _to_package_path(weighted[0][1])
    if package.startswith("/Game/Maps/Regions/"):
        return fallback_package_path
    return package


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

    enemy_mesh_melee = _choose_best(
        assets,
        {"SkeletalMesh"},
        include_terms=["roman", "shield", "legion", "soldier", "infantry"],
        exclude_terms=["jesus", "hero", "demon", "monster", "female", "civilian"],
        bonus_terms=["sk_", "enemy"],
    ) or enemy_mesh

    enemy_mesh_spear = _choose_best(
        assets,
        {"SkeletalMesh"},
        include_terms=["roman", "spear", "lancer", "pike", "centurion"],
        exclude_terms=["jesus", "hero", "demon", "monster", "female", "civilian"],
        bonus_terms=["sk_", "enemy"],
    ) or enemy_mesh_melee

    enemy_mesh_ranged = _choose_best(
        assets,
        {"SkeletalMesh"},
        include_terms=["archer", "ranged", "slinger", "bow", "marksman", "roman"],
        exclude_terms=["jesus", "hero", "demon", "monster", "female", "civilian"],
        bonus_terms=["sk_", "enemy"],
    ) or enemy_mesh_melee

    enemy_mesh_demon = _choose_best(
        assets,
        {"SkeletalMesh"},
        include_terms=["demon", "spirit", "wraith", "undead", "ghost", "fiend"],
        exclude_terms=["jesus", "hero", "roman", "civilian"],
        bonus_terms=["sk_", "monster", "enemy"],
    ) or enemy_mesh_melee

    enemy_mesh_boss = _choose_best(
        assets,
        {"SkeletalMesh"},
        include_terms=["boss", "warlord", "commander", "centurion", "champion"],
        exclude_terms=["jesus", "hero", "civilian"],
        bonus_terms=["sk_", "enemy", "elite"],
    ) or enemy_mesh_melee

    enemy_anim_melee = _choose_best(
        assets,
        {"AnimBlueprint", "Blueprint"},
        include_terms=["abp", "anim", "melee", "sword", "shield", "roman", "legion"],
        exclude_terms=["test", "demo", "hero", "player"],
        bonus_terms=["combat", "enemy"],
    ) or enemy_anim

    enemy_anim_spear = _choose_best(
        assets,
        {"AnimBlueprint", "Blueprint"},
        include_terms=["abp", "anim", "spear", "polearm", "lancer"],
        exclude_terms=["test", "demo", "hero", "player"],
        bonus_terms=["combat", "enemy"],
    ) or enemy_anim_melee

    enemy_anim_ranged = _choose_best(
        assets,
        {"AnimBlueprint", "Blueprint"},
        include_terms=["abp", "anim", "ranged", "archer", "bow", "sling"],
        exclude_terms=["test", "demo", "hero", "player"],
        bonus_terms=["combat", "enemy"],
    ) or enemy_anim_melee

    enemy_anim_demon = _choose_best(
        assets,
        {"AnimBlueprint", "Blueprint"},
        include_terms=["abp", "anim", "demon", "monster", "wraith", "fiend"],
        exclude_terms=["test", "demo", "hero", "player"],
        bonus_terms=["combat", "enemy"],
    ) or enemy_anim_melee

    enemy_anim_boss = _choose_best(
        assets,
        {"AnimBlueprint", "Blueprint"},
        include_terms=["abp", "anim", "boss", "elite", "warlord", "champion"],
        exclude_terms=["test", "demo", "hero", "player"],
        bonus_terms=["combat", "enemy"],
    ) or enemy_anim_melee

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
        include_terms=["tent", "awning", "stall", "cloth", "canopy"],
        exclude_terms=["weapon", "character", "skeletal", "vfx", "collision", "proxy", "tree", "foliage", "leaf", "bush"],
        bonus_terms=["sm_", "market"],
    )

    env_canopy_mesh = _choose_best(
        assets,
        {"StaticMesh"},
        include_terms=["olive", "tree", "palm", "foliage", "bush", "canopy"],
        exclude_terms=["weapon", "character", "skeletal", "vfx", "collision", "proxy", "tent", "awning", "stall", "wall", "block"],
        bonus_terms=["sm_", "environment"],
    )

    if env_canopy_mesh and env_canopy_mesh == env_block_mesh:
        env_canopy_mesh = _choose_best(
            assets,
            {"StaticMesh"},
            include_terms=["tree", "olive", "palm", "foliage", "bush"],
            exclude_terms=["weapon", "character", "skeletal", "vfx", "collision", "proxy", "wall", "block", "tent", "awning", "stall"],
            bonus_terms=["sm_", "nature", "environment"],
        )

    if env_tent_mesh and env_canopy_mesh and env_tent_mesh == env_canopy_mesh:
        env_tent_mesh = _choose_best(
            assets,
            {"StaticMesh"},
            include_terms=["tent", "awning", "stall", "cloth"],
            exclude_terms=["weapon", "character", "skeletal", "vfx", "collision", "proxy", "tree", "foliage", "bush", "leaf"],
            bonus_terms=["sm_", "market"],
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

    default_maps = {
        "GalileeMap": "/Game/Maps/Regions/Galilee/L_GalileeShores",
        "DecapolisMap": "/Game/Maps/Regions/Decapolis/L_DecapolisRuins",
        "WildernessMap": "/Game/Maps/Regions/Wilderness/L_WildernessTemptation",
        "JerusalemMap": "/Game/Maps/Regions/Jerusalem/L_JerusalemApproach",
        "GethsemaneMap": "/Game/Maps/Regions/Gethsemane/L_GardenGethsemane",
        "ViaDolorosaMap": "/Game/Maps/Regions/ViaDolorosa/L_ViaDolorosa",
        "EmptyTombMap": "/Game/Maps/Regions/EmptyTomb/L_EmptyTomb",
    }

    map_candidates = _choose_top_maps(assets, max_results=4)
    primary_map = _to_package_path(map_candidates[0]) if map_candidates else default_maps["GalileeMap"]
    if primary_map.startswith("/Game/Maps/Regions/"):
        primary_map = default_maps["GalileeMap"]

    enemy_material_roman = _choose_best(
        assets,
        {"Material", "MaterialInstanceConstant", "MaterialInstance"},
        include_terms=["roman", "armor", "soldier", "legion", "metal"],
        exclude_terms=["demon", "monster", "ui", "vfx"],
        bonus_terms=["mi_", "character"],
    )
    enemy_material_demon = _choose_best(
        assets,
        {"Material", "MaterialInstanceConstant", "MaterialInstance"},
        include_terms=["demon", "monster", "evil", "corrupt", "undead"],
        exclude_terms=["roman", "soldier", "ui", "vfx"],
        bonus_terms=["mi_", "character"],
    )
    enemy_material_boss = _choose_best(
        assets,
        {"Material", "MaterialInstanceConstant", "MaterialInstance"},
        include_terms=["boss", "elite", "warlord", "commander", "armor"],
        exclude_terms=["ui", "vfx"],
        bonus_terms=["mi_", "character"],
    )

    galilee_map = _choose_region_map(assets, ["galilee", "shore", "coast", "lake", "fish"], default_maps["GalileeMap"])
    decapolis_map = _choose_region_map(assets, ["decapolis", "ruin", "roman", "plaza"], default_maps["DecapolisMap"])
    wilderness_map = _choose_region_map(assets, ["wilderness", "desert", "canyon", "dune"], default_maps["WildernessMap"])
    jerusalem_map = _choose_region_map(assets, ["jerusalem", "city", "temple", "wall"], default_maps["JerusalemMap"])
    gethsemane_map = _choose_region_map(assets, ["gethsemane", "garden", "olive", "grove"], default_maps["GethsemaneMap"])
    via_dolorosa_map = _choose_region_map(assets, ["via", "dolorosa", "street", "alley", "city"], default_maps["ViaDolorosaMap"])
    empty_tomb_map = _choose_region_map(assets, ["tomb", "empty", "cave", "dawn"], default_maps["EmptyTombMap"])

    overrides = {
        "PlayerSkeletalMesh": player_mesh,
        "EnemySkeletalMesh": enemy_mesh,
        "EnemySkeletalMesh_MeleeShield": enemy_mesh_melee,
        "EnemySkeletalMesh_Spear": enemy_mesh_spear,
        "EnemySkeletalMesh_Ranged": enemy_mesh_ranged,
        "EnemySkeletalMesh_Demon": enemy_mesh_demon,
        "EnemySkeletalMesh_Boss": enemy_mesh_boss,
        "PlayerAnimBlueprint": _to_anim_class_path(player_anim),
        "EnemyAnimBlueprint": _to_anim_class_path(enemy_anim),
        "EnemyAnimBlueprint_MeleeShield": _to_anim_class_path(enemy_anim_melee),
        "EnemyAnimBlueprint_Spear": _to_anim_class_path(enemy_anim_spear),
        "EnemyAnimBlueprint_Ranged": _to_anim_class_path(enemy_anim_ranged),
        "EnemyAnimBlueprint_Demon": _to_anim_class_path(enemy_anim_demon),
        "EnemyAnimBlueprint_Boss": _to_anim_class_path(enemy_anim_boss),
        "EnemyMaterialRoman": enemy_material_roman,
        "EnemyMaterialDemon": enemy_material_demon,
        "EnemyMaterialBoss": enemy_material_boss,
        "EnvMeshBlock": env_block_mesh,
        "EnvMeshColumn": env_column_mesh,
        "EnvMeshTent": env_tent_mesh,
        "EnvMeshCanopy": env_canopy_mesh,
        "EnvMeshGround": env_ground_mesh,
        "EnvMaterialStone": env_material_stone,
        "EnvMaterialOlive": env_material_olive,
        "EnvMaterialSand": env_material_sand,
        "GalileeMap": galilee_map,
        "DecapolisMap": decapolis_map,
        "WildernessMap": wilderness_map,
        "JerusalemMap": jerusalem_map,
        "GethsemaneMap": gethsemane_map,
        "ViaDolorosaMap": via_dolorosa_map,
        "EmptyTombMap": empty_tomb_map,
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
