extends Control
class_name MainMenu

const SaveSystemScript := preload("res://scripts/persistence/save_system.gd")
const TombBackdropScript := preload("res://scripts/ui/tomb_backdrop.gd")

var _continue_button: Button
var _slot_labels: Array[Label] = []
var _status_label: Label
var _settings_panel: PanelContainer


func _ready() -> void:
	Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)
	_build_ui()
	_refresh_slots()
	_play_theme()


func _play_theme() -> void:
	var player := AudioStreamPlayer.new()
	var stream := load("res://assets/music/epic_biblical_theme.wav") as AudioStream
	if stream == null:
		return
	player.stream = stream
	player.autoplay = true
	player.bus = "Music"
	add_child(player)


func _build_ui() -> void:
	set_anchors_preset(Control.PRESET_FULL_RECT)

	var backdrop := TombBackdropScript.new()
	add_child(backdrop)

	var shell := MarginContainer.new()
	shell.set_anchors_preset(Control.PRESET_FULL_RECT)
	shell.add_theme_constant_override("margin_left", 120)
	shell.add_theme_constant_override("margin_top", 72)
	shell.add_theme_constant_override("margin_right", 120)
	shell.add_theme_constant_override("margin_bottom", 72)
	add_child(shell)

	var panel := PanelContainer.new()
	shell.add_child(panel)
	var panel_style := StyleBoxFlat.new()
	panel_style.bg_color = Color(0.04, 0.035, 0.035, 0.66)
	panel_style.border_color = Color(0.95, 0.79, 0.52, 0.42)
	panel_style.border_width_left = 2
	panel_style.border_width_top = 2
	panel_style.border_width_right = 2
	panel_style.border_width_bottom = 2
	panel_style.corner_radius_top_left = 28
	panel_style.corner_radius_top_right = 28
	panel_style.corner_radius_bottom_left = 28
	panel_style.corner_radius_bottom_right = 28
	panel.add_theme_stylebox_override("panel", panel_style)

	var body := HBoxContainer.new()
	body.add_theme_constant_override("separation", 60)
	panel.add_child(body)

	var left := VBoxContainer.new()
	left.add_theme_constant_override("separation", 12)
	left.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	body.add_child(left)

	var right := VBoxContainer.new()
	right.add_theme_constant_override("separation", 10)
	right.custom_minimum_size = Vector2(480.0, 0.0)
	body.add_child(right)

	_add_header(left)
	_add_action_buttons(left)
	_add_class_hint(left)
	_add_slot_ui(right)
	_add_overlays()

	if _continue_button != null:
		_continue_button.grab_focus()


func _add_header(left: VBoxContainer) -> void:
	var title := Label.new()
	title.text = "Jesus Walks"
	title.add_theme_font_size_override("font_size", 64)
	title.add_theme_color_override("font_color", Color(1.0, 0.94, 0.79))
	left.add_child(title)

	var subtitle := Label.new()
	subtitle.text = "The stone is rolled away"
	subtitle.add_theme_font_size_override("font_size", 27)
	subtitle.add_theme_color_override("font_color", Color(0.96, 0.84, 0.62, 0.94))
	left.add_child(subtitle)

	var caption := Label.new()
	caption.text = "A quiet dawn. A slow retreat from the open tomb."
	caption.add_theme_color_override("font_color", Color(0.83, 0.8, 0.75, 0.92))
	left.add_child(caption)

	var spacer := Control.new()
	spacer.custom_minimum_size = Vector2(0.0, 12.0)
	left.add_child(spacer)


func _add_action_buttons(left: VBoxContainer) -> void:
	_continue_button = _make_action_button("Continue", _on_continue_pressed)
	left.add_child(_continue_button)
	left.add_child(_make_action_button("New Game", _on_new_game_pressed))
	left.add_child(_make_action_button("Settings", _on_settings_pressed))
	left.add_child(_make_action_button("Credits", _on_credits_pressed))
	left.add_child(_make_action_button("Quit", _on_quit_pressed))


func _add_class_hint(left: VBoxContainer) -> void:
	var class_label := Label.new()
	class_label.text = "Class: [1] Shepherd   [2] Zealot   [3] Prophet"
	class_label.add_theme_color_override("font_color", Color(0.89, 0.84, 0.76, 0.98))
	left.add_child(class_label)


func _add_slot_ui(right: VBoxContainer) -> void:
	var slots_title := Label.new()
	slots_title.text = "Journey Records"
	slots_title.add_theme_font_size_override("font_size", 34)
	slots_title.add_theme_color_override("font_color", Color(1.0, 0.92, 0.78))
	right.add_child(slots_title)

	right.add_child(_make_action_button("Load Slot 1", func(): _on_load_slot_pressed(1), false))
	right.add_child(_make_action_button("Load Slot 2", func(): _on_load_slot_pressed(2), false))
	right.add_child(_make_action_button("Load Slot 3", func(): _on_load_slot_pressed(3), false))

	for i in range(3):
		var label := Label.new()
		label.text = "Slot %d: Empty" % (i + 1)
		label.add_theme_color_override("font_color", Color(0.87, 0.84, 0.79, 0.95))
		right.add_child(label)
		_slot_labels.append(label)


func _add_overlays() -> void:
	_settings_panel = PanelContainer.new()
	_settings_panel.visible = false
	_settings_panel.offset_left = 360.0
	_settings_panel.offset_top = 220.0
	_settings_panel.offset_right = 980.0
	_settings_panel.offset_bottom = 540.0
	var style := StyleBoxFlat.new()
	style.bg_color = Color(0.07, 0.06, 0.06, 0.95)
	style.border_color = Color(0.96, 0.8, 0.53, 0.7)
	style.border_width_left = 1
	style.border_width_top = 1
	style.border_width_right = 1
	style.border_width_bottom = 1
	style.corner_radius_top_left = 20
	style.corner_radius_top_right = 20
	style.corner_radius_bottom_left = 20
	style.corner_radius_bottom_right = 20
	_settings_panel.add_theme_stylebox_override("panel", style)
	add_child(_settings_panel)

	var settings_text := Label.new()
	settings_text.text = "Settings\n- Master Volume: 80%\n- Music Volume: 70%\n- Fullscreen: Off"
	settings_text.position = Vector2(24.0, 24.0)
	_settings_panel.add_child(settings_text)

	_status_label = Label.new()
	_status_label.offset_left = 180.0
	_status_label.offset_top = 820.0
	_status_label.offset_right = 1500.0
	_status_label.offset_bottom = 860.0
	_status_label.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	_status_label.add_theme_color_override("font_color", Color(1.0, 0.85, 0.62, 0.95))
	add_child(_status_label)


func _make_action_button(text: String, action: Callable, wide: bool = true) -> Button:
	var button := Button.new()
	button.text = text
	button.focus_mode = Control.FOCUS_ALL
	button.custom_minimum_size = Vector2((360.0 if wide else 300.0), 56.0)
	button.pressed.connect(action)
	button.add_theme_font_size_override("font_size", 24)
	button.add_theme_color_override("font_color", Color(1.0, 0.93, 0.8))
	button.add_theme_color_override("font_hover_color", Color(1.0, 0.97, 0.87))
	button.add_theme_color_override("font_pressed_color", Color(1.0, 0.88, 0.66))

	var normal := StyleBoxFlat.new()
	normal.bg_color = Color(0.16, 0.11, 0.08, 0.84)
	normal.border_color = Color(0.96, 0.76, 0.45, 0.4)
	normal.border_width_left = 1
	normal.border_width_top = 1
	normal.border_width_right = 1
	normal.border_width_bottom = 1
	normal.corner_radius_top_left = 14
	normal.corner_radius_top_right = 14
	normal.corner_radius_bottom_left = 14
	normal.corner_radius_bottom_right = 14
	button.add_theme_stylebox_override("normal", normal)

	var hover := normal.duplicate() as StyleBoxFlat
	hover.bg_color = Color(0.24, 0.17, 0.12, 0.9)
	hover.border_color = Color(1.0, 0.84, 0.54, 0.72)
	button.add_theme_stylebox_override("hover", hover)

	var pressed := normal.duplicate() as StyleBoxFlat
	pressed.bg_color = Color(0.28, 0.2, 0.14, 0.95)
	pressed.border_color = Color(1.0, 0.88, 0.62, 0.95)
	button.add_theme_stylebox_override("pressed", pressed)

	return button


func _refresh_slots() -> void:
	for i in range(_slot_labels.size()):
		_slot_labels[i].text = SaveSystemScript.slot_summary(i + 1)
	if _continue_button != null:
		_continue_button.disabled = not SaveSystemScript.checkpoint_exists()


func _get_session() -> GameSession:
	return get_node_or_null("/root/GameSession") as GameSession


func _capture_class_selection() -> void:
	var class_id := GameSession.CharacterClass.SHEPHERD
	if Input.is_key_pressed(KEY_2):
		class_id = GameSession.CharacterClass.ZEALOT
	elif Input.is_key_pressed(KEY_3):
		class_id = GameSession.CharacterClass.PROPHET
	var session := _get_session()
	if session != null:
		session.selected_class = class_id


func _on_new_game_pressed() -> void:
	_capture_class_selection()
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


func _on_settings_pressed() -> void:
	if _settings_panel != null:
		_settings_panel.visible = not _settings_panel.visible


func _on_credits_pressed() -> void:
	get_tree().change_scene_to_file("res://scenes/Credits.tscn")


func _on_quit_pressed() -> void:
	get_tree().quit()
