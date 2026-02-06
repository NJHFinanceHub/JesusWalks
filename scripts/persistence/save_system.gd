extends RefCounted
class_name SaveSystem


const SAVE_DIRECTORY := "user://saves"
const SAVE_VERSION := 1
const CHECKPOINT_PATH := "%s/campaign.save" % SAVE_DIRECTORY


static func save_slot(slot_id: int, payload: Dictionary) -> bool:
	if slot_id < 1:
		return false

	var directory_error := DirAccess.make_dir_recursive_absolute(SAVE_DIRECTORY)
	if directory_error != OK:
		return false

	var safe_payload := payload.duplicate(true)
	safe_payload["meta"] = {
		"save_version": SAVE_VERSION,
		"slot_id": slot_id,
		"timestamp": Time.get_datetime_string_from_system(true),
	}

	var file := FileAccess.open(_slot_path(slot_id), FileAccess.WRITE)
	if file == null:
		return false

	file.store_string(JSON.stringify(safe_payload, "\t"))
	file.flush()
	return true


static func load_slot(slot_id: int) -> Dictionary:
	if slot_id < 1:
		return {}

	var path := _slot_path(slot_id)
	if not FileAccess.file_exists(path):
		return {}

	var file := FileAccess.open(path, FileAccess.READ)
	if file == null:
		return {}

	var parsed: Variant = JSON.parse_string(file.get_as_text())
	if parsed is Dictionary:
		return parsed
	return {}


static func slot_exists(slot_id: int) -> bool:
	if slot_id < 1:
		return false
	return FileAccess.file_exists(_slot_path(slot_id))


static func slot_summary(slot_id: int) -> String:
	var data := load_slot(slot_id)
	if data.is_empty():
		return "Slot %d: Empty" % slot_id

	var meta: Variant = data.get("meta", {})
	if not (meta is Dictionary):
		return "Slot %d: Save data present" % slot_id

	var timestamp := str(meta.get("timestamp", "Unknown time"))
	return "Slot %d: %s" % [slot_id, timestamp]


static func _slot_path(slot_id: int) -> String:
	return "%s/slot_%d.save" % [SAVE_DIRECTORY, slot_id]


static func save_checkpoint(payload: Dictionary) -> bool:
	var directory_error := DirAccess.make_dir_recursive_absolute(SAVE_DIRECTORY)
	if directory_error != OK:
		return false

	var safe_payload := payload.duplicate(true)
	safe_payload["meta"] = {
		"save_version": SAVE_VERSION,
		"checkpoint": true,
		"timestamp": Time.get_datetime_string_from_system(true),
	}

	var file := FileAccess.open(CHECKPOINT_PATH, FileAccess.WRITE)
	if file == null:
		return false

	file.store_string(JSON.stringify(safe_payload, "\t"))
	file.flush()
	return true


static func load_checkpoint() -> Dictionary:
	if not FileAccess.file_exists(CHECKPOINT_PATH):
		return {}

	var file := FileAccess.open(CHECKPOINT_PATH, FileAccess.READ)
	if file == null:
		return {}

	var parsed := JSON.parse_string(file.get_as_text())
	if parsed is Dictionary:
		return parsed
	return {}


static func checkpoint_exists() -> bool:
	return FileAccess.file_exists(CHECKPOINT_PATH)


static func clear_checkpoint() -> void:
	if FileAccess.file_exists(CHECKPOINT_PATH):
		DirAccess.remove_absolute(CHECKPOINT_PATH)

