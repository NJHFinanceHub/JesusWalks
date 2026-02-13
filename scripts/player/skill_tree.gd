extends RefCounted
class_name SkillTree


const BRANCH_COMBAT := "combat"
const BRANCH_MOVEMENT := "movement"
const BRANCH_MIRACLES := "miracles"
const BRANCH_DEFENSE := "defense"

const SKILL_DEFINITIONS := {
	"combat_smite": {
		"name": "Smite Training",
		"branch": BRANCH_COMBAT,
		"icon": "⚔",
		"description": "Increase light/heavy attack damage by 10%.",
		"xp_requirement": 60,
		"cost": 1,
		"requires": [],
	},
	"combat_crusader": {
		"name": "Crusader Momentum",
		"branch": BRANCH_COMBAT,
		"icon": "🗡",
		"description": "Increase heavy poise damage and melee range.",
		"xp_requirement": 140,
		"cost": 1,
		"requires": ["combat_smite"],
	},
	"movement_pilgrim_stride": {
		"name": "Pilgrim Stride",
		"branch": BRANCH_MOVEMENT,
		"icon": "👣",
		"description": "Increase move speed by 12%.",
		"xp_requirement": 70,
		"cost": 1,
		"requires": [],
	},
	"movement_swift_vow": {
		"name": "Swift Vow",
		"branch": BRANCH_MOVEMENT,
		"icon": "💨",
		"description": "Reduce dodge stamina cost and cooldown.",
		"xp_requirement": 165,
		"cost": 1,
		"requires": ["movement_pilgrim_stride"],
	},
	"miracles_abundance": {
		"name": "Abundance of Grace",
		"branch": BRANCH_MIRACLES,
		"icon": "✚",
		"description": "Healing miracle restores more health.",
		"xp_requirement": 90,
		"cost": 1,
		"requires": [],
	},
	"miracles_radiance_lance": {
		"name": "Radiance Lance",
		"branch": BRANCH_MIRACLES,
		"icon": "☀",
		"description": "Radiance miracle gains damage/radius.",
		"xp_requirement": 190,
		"cost": 1,
		"requires": ["miracles_abundance"],
	},
	"defense_shepherd_guard": {
		"name": "Shepherd Guard",
		"branch": BRANCH_DEFENSE,
		"icon": "🛡",
		"description": "Increase max health and block efficiency.",
		"xp_requirement": 80,
		"cost": 1,
		"requires": [],
	},
	"defense_steadfast": {
		"name": "Steadfast Heart",
		"branch": BRANCH_DEFENSE,
		"icon": "⛨",
		"description": "Increase max stamina and regen.",
		"xp_requirement": 170,
		"cost": 1,
		"requires": ["defense_shepherd_guard"],
	},
}


static func get_definitions() -> Dictionary:
	return SKILL_DEFINITIONS


static func get_skill(skill_id: String) -> Dictionary:
	return SKILL_DEFINITIONS.get(skill_id, {})


static func can_unlock(skill_id: String, unlocked: Array[String], total_xp: int, skill_points: int) -> bool:
	var definition := get_skill(skill_id)
	if definition.is_empty():
		return false
	if unlocked.has(skill_id):
		return false
	if skill_points < int(definition.get("cost", 1)):
		return false
	if total_xp < int(definition.get("xp_requirement", 0)):
		return false
	for requirement in definition.get("requires", []):
		if not unlocked.has(str(requirement)):
			return false
	return true


static func branch_skills(branch_id: String) -> Array[String]:
	var result: Array[String] = []
	for skill_id in SKILL_DEFINITIONS.keys():
		var definition := SKILL_DEFINITIONS[skill_id]
		if str(definition.get("branch", "")) == branch_id:
			result.append(skill_id)
	return result
