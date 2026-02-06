extends Control
class_name MainMenu


const SaveSystemScript := preload("res://scripts/persistence/save_system.gd")


var _continue_button: Button
var _slot_labels: Array[Label] = []
var _status_label: Label


func _ready() -> void:
	Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)
	_build_ui()
	_refresh_slots()


func _build_ui() -> void:
	set_anchors_preset(Control.PRESET_FULL_RECT)

	var backdrop := ColorRect.new()
	backdrop.color = Color(0.06, 0.07, 0.08, 1.0)
	backdrop.set_anchors_preset(Control.PRESET_FULL_RECT)
	add_child(backdrop)

	var panel := PanelContainer.new()
	panel.offset_left = 220.0
	panel.offset_top = 120.0
	panel.offset_right = 1380.0
	panel.offset_bottom = 820.0
	add_child(panel)

	var style := StyleBoxFlat.new()
	style.bg_color = Color(0.09, 0.09, 0.08, 0.94)
	style.corner_radius_top_left = 18
	style.corner_radius_top_right = 18
	style.corner_radius_bottom_left = 18
	style.corner_radius_bottom_right = 18
	panel.add_theme_stylebox_override("panel", style)

	var layout := HBoxContainer.new()
	layout.add_theme_constant_override("separation", 40)
	layout.offset_left = 24.0
	layout.offset_right = 1080.0
	layout.offset_top = 24.0
	layout.offset_bottom = 600.0
	panel.add_child(layout)

	var left := VBoxContainer.new()
	left.add_theme_constant_override("separation", 14)
	layout.add_child(left)

	var title := Label.new()
	title.text = "The Nazarene"
	title.add_theme_color_override("font_color", Color(0.96, 0.9, 0.78))
	title.add_theme_font_size_override("font_size", 32)
	left.add_child(title)

	var subtitle := Label.new()
	subtitle.text = "A pilgrimage across Galilee to Jerusalem."
	subtitle.add_theme_color_override("font_color", Color(0.82, 0.78, 0.66))
	left.add_child(subtitle)

	_continue_button = _make_button("Continue Journey", _on_continue_pressed)
	left.add_child(_continue_button)
	left.add_child(_make_button("New Pilgrimage", _on_new_game_pressed))

	left.add_child(_make_button("Credits", _on_credits_pressed))
	left.add_child(_make_button("Quit", _on_quit_pressed))

	var right := VBoxContainer.new()
	right.add_theme_constant_override("separation", 10)
	layout.add_child(right)

	var slots_title := Label.new()
	slots_title.text = "Pilgrimage Records"
	slots_title.add_theme_color_override("font_color", Color(0.88, 0.84, 0.72))
	right.add_child(slots_title)

	right.add_child(_make_button("Load Slot 1", func(): _on_load_slot_pressed(1)))
	right.add_child(_make_button("Load Slot 2", func(): _on_load_slot_pressed(2)))
	right.add_child(_make_button("Load Slot 3", func(): _on_load_slot_pressed(3)))

	var slot_panel := PanelContainer.new()
	var slot_style := StyleBoxFlat.new()
	slot_style.bg_color = Color(0.05, 0.05, 0.04, 0.7)
	slot_style.corner_radius_top_left = 10
	slot_style.corner_radius_top_right = 10
	slot_style.corner_radius_bottom_left = 10
	slot_style.corner_radius_bottom_right = 10
	slot_panel.add_theme_stylebox_override("panel", slot_style)
	right.add_child(slot_panel)

	var slot_layout := VBoxContainer.new()
	slot_layout.add_theme_constant_override("separation", 6)
	slot_layout.offset_left = 12.0
	slot_layout.offset_right = 420.0
	slot_layout.offset_top = 10.0
	slot_layout.offset_bottom = 180.0
	slot_panel.add_child(slot_layout)

	for i in range(3):
		var label := Label.new()
		label.text = "Slot %d: Empty" % (i + 1)
		label.add_theme_color_override("font_color", Color(0.83, 0.8, 0.68))
		slot_layout.add_child(label)
		_slot_labels.append(label)

	_status_label = Label.new()
	_status_label.text = ""
	_status_label.add_theme_color_override("font_color", Color(0.9, 0.76, 0.6))
	_status_label.offset_left = 32.0
	_status_label.offset_right = 760.0
	_status_label.offset_top = 740.0
	_status_label.offset_bottom = 780.0
	panel.add_child(_status_label)


func _make_button(text: String, action: Callable) -> Button:
	var button := Button.new()
	button.text = text
	button.pressed.connect(action)
	return button


func _refresh_slots() -> void:
	for i in range(_slot_labels.size()):
		_slot_labels[i].text = SaveSystemScript.slot_summary(i + 1)
	_continue_button.disabled = not SaveSystemScript.checkpoint_exists()


func _get_session() -> GameSession:
	return get_node_or_null("/root/GameSession") as GameSession


func _on_new_game_pressed() -> void:
	var session := _get_session()
	if session != null:
		session.start_new_game()
	SaveSystemScript.clear_checkpoint()
	get_tree().change_scene_to_file("res://scenes/Campaign.tscn")


func _on_continue_pressed() -> void:
	var payload := SaveSystemScript.load_checkpoint()
	if payload.is_empty():
		_status_label.text = "No checkpoint found. Start a new pilgrimage."
		_refresh_slots()
		return

	var session := _get_session()
	if session != null:
		session.queue_payload(payload)
	get_tree().change_scene_to_file("res://scenes/Campaign.tscn")


func _on_load_slot_pressed(slot_id: int) -> void:
	var payload := SaveSystemScript.load_slot(slot_id)
	if payload.is_empty():
		_status_label.text = "Slot %d is empty." % slot_id
		return

	var session := _get_session()
	if session != null:
		session.queue_payload(payload)
	get_tree().change_scene_to_file("res://scenes/Campaign.tscn")


func _on_credits_pressed() -> void:
	get_tree().change_scene_to_file("res://scenes/Credits.tscn")


func _on_quit_pressed() -> void:
	get_tree().quit()
