extends Node
class_name GameSession


const SaveSystemScript := preload("res://scripts/persistence/save_system.gd")


enum CharacterClass {
	SHEPHERD,
	ZEALOT,
	PROPHET,
}


var region_index: int = 0
var unlocked_miracles: Array[String] = ["heal"]
var max_health_bonus: float = 0.0
var max_stamina_bonus: float = 0.0
var campaign_flags: Dictionary = {}
var pending_payload: Dictionary = {}

var selected_class: CharacterClass = CharacterClass.SHEPHERD
var total_xp: int = 0
var player_level: int = 1
var skill_points: int = 0
var unlocked_skills: Array[String] = []


func start_new_game() -> void:
	region_index = 0
	unlocked_miracles = ["heal"]
	max_health_bonus = 0.0
	max_stamina_bonus = 0.0
	campaign_flags = {}
	pending_payload = {}
	total_xp = 0
	player_level = 1
	skill_points = 0
	unlocked_skills = []


func queue_payload(payload: Dictionary) -> void:
	pending_payload = payload.duplicate(true)


func take_pending_payload() -> Dictionary:
	var payload := pending_payload
	pending_payload = {}
	return payload


func build_campaign_payload() -> Dictionary:
	return {
		"region_index": region_index,
		"unlocked_miracles": unlocked_miracles.duplicate(),
		"max_health_bonus": max_health_bonus,
		"max_stamina_bonus": max_stamina_bonus,
		"flags": campaign_flags.duplicate(true),
		"selected_class": int(selected_class),
		"total_xp": total_xp,
		"player_level": player_level,
		"skill_points": skill_points,
		"unlocked_skills": unlocked_skills.duplicate(),
	}


func apply_campaign_payload(data: Dictionary) -> void:
	region_index = clampi(int(data.get("region_index", 0)), 0, 32)
	unlocked_miracles = data.get("unlocked_miracles", ["heal"])
	max_health_bonus = float(data.get("max_health_bonus", 0.0))
	max_stamina_bonus = float(data.get("max_stamina_bonus", 0.0))
	campaign_flags = data.get("flags", {})
	selected_class = clampi(int(data.get("selected_class", CharacterClass.SHEPHERD)), 0, CharacterClass.size() - 1)
	total_xp = maxi(int(data.get("total_xp", 0)), 0)
	player_level = maxi(int(data.get("player_level", 1)), 1)
	skill_points = maxi(int(data.get("skill_points", 0)), 0)
	unlocked_skills = data.get("unlocked_skills", [])


func ensure_miracle_unlocked(miracle_id: String) -> bool:
	if miracle_id.is_empty():
		return false
	if unlocked_miracles.has(miracle_id):
		return false
	unlocked_miracles.append(miracle_id)
	return true


func mark_flag(flag_id: String) -> void:
	if flag_id.is_empty():
		return
	campaign_flags[flag_id] = true


func is_flag_set(flag_id: String) -> bool:
	if flag_id.is_empty():
		return false
	return bool(campaign_flags.get(flag_id, false))


func class_name_label() -> String:
	match selected_class:
		CharacterClass.ZEALOT:
			return "Zealot"
		CharacterClass.PROPHET:
			return "Prophet"
		_:
			return "Shepherd"
