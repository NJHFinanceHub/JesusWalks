extends Control
class_name MainMenu


const SaveSystemScript := preload("res://scripts/persistence/save_system.gd")


var _continue_button: Button
var _slot_labels: Array[Label] = []
var _status_label: Label
var _anim_time: float = 0.0
var _background: ColorRect
var _settings_panel: PanelContainer


func _ready() -> void:
	Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)
	_build_ui()
	_refresh_slots()
	_play_theme()


func _process(delta: float) -> void:
	_anim_time += delta
	if _background != null:
		var pulse := 0.08 + sin(_anim_time * 0.75) * 0.03
		_background.color = Color(0.05 + pulse, 0.06 + pulse * 0.8, 0.09 + pulse * 0.6, 1.0)


func _play_theme() -> void:
	var player := AudioStreamPlayer.new()
	var stream := load("res://assets/music/epic_biblical_theme.wav") as AudioStream
	if stream != null:
		player.stream = stream
		player.autoplay = true
		player.bus = "Music"
		add_child(player)


func _build_ui() -> void:
	set_anchors_preset(Control.PRESET_FULL_RECT)

	_background = ColorRect.new()
	_background.color = Color(0.08, 0.09, 0.12, 1.0)
	_background.set_anchors_preset(Control.PRESET_FULL_RECT)
	add_child(_background)

	var panel := PanelContainer.new()
	panel.offset_left = 200.0
	panel.offset_top = 100.0
	panel.offset_right = 1400.0
	panel.offset_bottom = 820.0
	add_child(panel)

	var style := StyleBoxFlat.new()
	style.bg_color = Color(0.09, 0.09, 0.08, 0.9)
	style.corner_radius_top_left = 18
	style.corner_radius_top_right = 18
	style.corner_radius_bottom_left = 18
	style.corner_radius_bottom_right = 18
	panel.add_theme_stylebox_override("panel", style)

	var layout := HBoxContainer.new()
	layout.add_theme_constant_override("separation", 48)
	panel.add_child(layout)

	var left := VBoxContainer.new()
	left.custom_minimum_size = Vector2(520.0, 640.0)
	left.add_theme_constant_override("separation", 14)
	layout.add_child(left)

	var title := Label.new()
	title.text = "Jesus Walks"
	title.add_theme_color_override("font_color", Color(0.98, 0.9, 0.75))
	title.add_theme_font_size_override("font_size", 44)
	left.add_child(title)

	var subtitle := Label.new()
	subtitle.text = "Action pilgrimage RPG"
	subtitle.add_theme_color_override("font_color", Color(0.82, 0.78, 0.66))
	left.add_child(subtitle)

	_continue_button = _make_button("Continue", _on_continue_pressed)
	left.add_child(_continue_button)
	left.add_child(_make_button("New Game", _on_new_game_pressed))
	left.add_child(_make_button("Settings", _on_settings_pressed))
	left.add_child(_make_button("Credits", _on_credits_pressed))
	left.add_child(_make_button("Quit", _on_quit_pressed))

	var class_label := Label.new()
	class_label.text = "Class: [1] Shepherd [2] Zealot [3] Prophet"
	left.add_child(class_label)

	var right := VBoxContainer.new()
	right.custom_minimum_size = Vector2(520.0, 640.0)
	right.add_theme_constant_override("separation", 10)
	layout.add_child(right)

	var slots_title := Label.new()
	slots_title.text = "Journey Records"
	right.add_child(slots_title)

	right.add_child(_make_button("Load Slot 1", func(): _on_load_slot_pressed(1)))
	right.add_child(_make_button("Load Slot 2", func(): _on_load_slot_pressed(2)))
	right.add_child(_make_button("Load Slot 3", func(): _on_load_slot_pressed(3)))

	for i in range(3):
		var label := Label.new()
		label.text = "Slot %d: Empty" % (i + 1)
		right.add_child(label)
		_slot_labels.append(label)

	_settings_panel = PanelContainer.new()
	_settings_panel.visible = false
	_settings_panel.offset_left = 370.0
	_settings_panel.offset_top = 250.0
	_settings_panel.offset_right = 860.0
	_settings_panel.offset_bottom = 480.0
	add_child(_settings_panel)
	var settings_text := Label.new()
	settings_text.text = "Settings\n- Master Volume: 80%\n- Music Volume: 70%\n- Fullscreen: Off"
	_settings_panel.add_child(settings_text)

	_status_label = Label.new()
	_status_label.text = ""
	_status_label.offset_left = 250.0
	_status_label.offset_right = 1300.0
	_status_label.offset_top = 760.0
	_status_label.offset_bottom = 805.0
	add_child(_status_label)

	_continue_button.grab_focus()


func _make_button(text: String, action: Callable) -> Button:
	var button := Button.new()
	button.text = text
	button.focus_mode = Control.FOCUS_ALL
	button.pressed.connect(action)
	return button


func _refresh_slots() -> void:
	for i in range(_slot_labels.size()):
		_slot_labels[i].text = SaveSystemScript.slot_summary(i + 1)
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
