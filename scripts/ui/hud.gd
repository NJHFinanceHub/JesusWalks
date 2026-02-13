extends CanvasLayer
class_name HUDLayer


var _player: PlayerController
var _health_bar: ProgressBar
var _stamina_bar: ProgressBar
var _faith_label: Label
var _lock_on_label: Label
var _context_label: Label
var _miracle_label: Label
var _blessing_label: Label
var _radiance_label: Label
var _animation_label: Label
var _region_label: Label
var _objective_label: Label
var _message_label: Label
var _message_timer: float = 0.0
var _ui_built: bool = false
var _xp_label: Label
var _class_label: Label
var _skill_tree_label: Label
var _popup_label: Label
var _popup_timer: float = 0.0


func _ready() -> void:
	_build_ui()


func _process(delta: float) -> void:
	if _player == null:
		return

	_update_miracle_label(_miracle_label, "Healing Miracle", "R", _player.get_heal_cooldown_remaining(), true)
	_update_miracle_label(_blessing_label, "Blessing Miracle", "1", _player.get_blessing_cooldown_remaining(), _player.is_miracle_unlocked("blessing"))
	_update_miracle_label(_radiance_label, "Radiance Miracle", "2", _player.get_radiance_cooldown_remaining(), _player.is_miracle_unlocked("radiance"))

	if _message_timer > 0.0:
		_message_timer = max(_message_timer - delta, 0.0)
		if _message_timer <= 0.0 and _message_label != null:
			_message_label.text = ""
	if _popup_timer > 0.0:
		_popup_timer = max(_popup_timer - delta, 0.0)
		if _popup_timer <= 0.0 and _popup_label != null:
			_popup_label.text = ""


func bind_player(player: PlayerController) -> void:
	_build_ui()
	_player = player
	if _player == null:
		return

	_player.health_changed.connect(_on_health_changed)
	_player.stamina_changed.connect(_on_stamina_changed)
	_player.faith_changed.connect(_on_faith_changed)
	_player.lock_target_changed.connect(_on_lock_target_changed)
	_player.context_hint_changed.connect(_on_context_hint_changed)
	_player.animation_state_changed.connect(_on_animation_state_changed)
	_player.experience_changed.connect(_on_experience_changed)
	_player.class_changed.connect(_on_class_changed)
	_player.level_up.connect(_on_level_up)
	_player.skill_unlocked.connect(_on_skill_unlocked)

	_on_health_changed(_player.current_health, _player.max_health)
	_on_stamina_changed(_player.current_stamina, _player.max_stamina)
	_on_faith_changed(_player.current_faith)
	_on_lock_target_changed("")
	_on_context_hint_changed("")
	_on_animation_state_changed("Idle")
	_on_class_changed(_player.get_class_name())
	_on_experience_changed(_player.total_xp, _player.xp_to_next_level(), _player.player_level, _player.unspent_skill_points)


func _build_ui() -> void:
	if _ui_built:
		return
	_ui_built = true

	var root := Control.new()
	root.name = "HUDRoot"
	root.set_anchors_preset(Control.PRESET_FULL_RECT)
	add_child(root)

	var panel := PanelContainer.new()
	panel.offset_left = 20.0
	panel.offset_top = 20.0
	panel.offset_right = 420.0
	panel.offset_bottom = 360.0
	root.add_child(panel)

	var style := StyleBoxFlat.new()
	style.bg_color = Color(0.06, 0.06, 0.04, 0.74)
	style.corner_radius_top_left = 8
	style.corner_radius_top_right = 8
	style.corner_radius_bottom_left = 8
	style.corner_radius_bottom_right = 8
	panel.add_theme_stylebox_override("panel", style)

	var layout := VBoxContainer.new()
	layout.add_theme_constant_override("separation", 7)
	panel.add_child(layout)

	var title := Label.new()
	title.text = "Jesus Walks - Pilgrimage"
	title.add_theme_color_override("font_color", Color(0.95, 0.89, 0.78))
	layout.add_child(title)

	var health_label := Label.new()
	health_label.text = "Health"
	layout.add_child(health_label)

	_health_bar = ProgressBar.new()
	_health_bar.min_value = 0.0
	_health_bar.max_value = 120.0
	_health_bar.custom_minimum_size = Vector2(360.0, 20.0)
	layout.add_child(_health_bar)

	var stamina_label := Label.new()
	stamina_label.text = "Stamina"
	layout.add_child(stamina_label)

	_stamina_bar = ProgressBar.new()
	_stamina_bar.min_value = 0.0
	_stamina_bar.max_value = 100.0
	_stamina_bar.custom_minimum_size = Vector2(360.0, 18.0)
	layout.add_child(_stamina_bar)

	_faith_label = Label.new()
	_faith_label.text = "Faith: 0"
	layout.add_child(_faith_label)

	_xp_label = Label.new()
	_xp_label.text = "Level 1 | XP 0 | SP 0"
	layout.add_child(_xp_label)

	_class_label = Label.new()
	_class_label.text = "Class: Shepherd"
	layout.add_child(_class_label)

	_lock_on_label = Label.new()
	_lock_on_label.text = "Lock-On: none"
	layout.add_child(_lock_on_label)

	_miracle_label = Label.new()
	_miracle_label.text = "Healing Miracle: Ready (R)"
	layout.add_child(_miracle_label)

	_blessing_label = Label.new()
	_blessing_label.text = "Blessing Miracle: Locked"
	layout.add_child(_blessing_label)

	_radiance_label = Label.new()
	_radiance_label.text = "Radiance Miracle: Locked"
	layout.add_child(_radiance_label)

	_animation_label = Label.new()
	_animation_label.text = "Anim: Idle"
	layout.add_child(_animation_label)

	_context_label = Label.new()
	_context_label.text = ""
	_context_label.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
	_context_label.custom_minimum_size = Vector2(360.0, 46.0)
	layout.add_child(_context_label)

	var mission_panel := PanelContainer.new()
	mission_panel.offset_left = 1180.0
	mission_panel.offset_top = 20.0
	mission_panel.offset_right = 1570.0
	mission_panel.offset_bottom = 220.0
	root.add_child(mission_panel)

	var mission_style := StyleBoxFlat.new()
	mission_style.bg_color = Color(0.06, 0.06, 0.05, 0.78)
	mission_style.corner_radius_top_left = 8
	mission_style.corner_radius_top_right = 8
	mission_style.corner_radius_bottom_left = 8
	mission_style.corner_radius_bottom_right = 8
	mission_panel.add_theme_stylebox_override("panel", mission_style)

	var mission_layout := VBoxContainer.new()
	mission_layout.add_theme_constant_override("separation", 6)
	mission_layout.offset_left = 10.0
	mission_layout.offset_right = 360.0
	mission_layout.offset_top = 10.0
	mission_layout.offset_bottom = 180.0
	mission_panel.add_child(mission_layout)

	_region_label = Label.new()
	_region_label.text = "Chapter 1: Galilee Shores"
	_region_label.add_theme_color_override("font_color", Color(0.94, 0.88, 0.76))
	mission_layout.add_child(_region_label)

	_objective_label = Label.new()
	_objective_label.text = "Objective"
	_objective_label.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
	_objective_label.add_theme_color_override("font_color", Color(0.85, 0.82, 0.7))
	_objective_label.custom_minimum_size = Vector2(340.0, 72.0)
	mission_layout.add_child(_objective_label)


	var skill_panel := PanelContainer.new()
	skill_panel.offset_left = 1180.0
	skill_panel.offset_top = 240.0
	skill_panel.offset_right = 1570.0
	skill_panel.offset_bottom = 500.0
	root.add_child(skill_panel)
	var skill_layout := VBoxContainer.new()
	skill_panel.add_child(skill_layout)
	var skill_title := Label.new()
	skill_title.text = "Skill Tree (PgDn unlock next)"
	skill_layout.add_child(skill_title)
	_skill_tree_label = Label.new()
	_skill_tree_label.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
	_skill_tree_label.custom_minimum_size = Vector2(350.0, 190.0)
	_skill_tree_label.text = "No skills unlocked."
	skill_layout.add_child(_skill_tree_label)

	_message_label = Label.new()
	_message_label.text = ""
	_message_label.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	_message_label.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
	_message_label.offset_left = 280.0
	_message_label.offset_right = 1320.0
	_message_label.offset_top = 14.0
	_message_label.offset_bottom = 70.0
	_message_label.add_theme_color_override("font_color", Color(0.96, 0.9, 0.78))
	root.add_child(_message_label)

	_popup_label = Label.new()
	_popup_label.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	_popup_label.offset_left = 520.0
	_popup_label.offset_right = 1080.0
	_popup_label.offset_top = 760.0
	_popup_label.offset_bottom = 840.0
	_popup_label.add_theme_color_override("font_color", Color(0.98, 0.84, 0.44))
	root.add_child(_popup_label)

	var controls := Label.new()
	controls.text = "WASD Move | LMB Light | RMB Heavy | Space Dodge | Shift Block | F Parry | Q Lock | R Heal | 1 Blessing | 2 Radiance | E Pray | PgDn Auto Unlock Skill | Tab Mouse | Esc Pause | Save F1-F3 | Load F5-F7"
	controls.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	controls.add_theme_color_override("font_color", Color(0.9, 0.85, 0.7))
	controls.offset_left = 24.0
	controls.offset_right = 1550.0
	controls.offset_top = 860.0
	controls.offset_bottom = 900.0
	root.add_child(controls)


func _on_health_changed(current: float, max_value: float) -> void:
	_health_bar.max_value = max_value
	_health_bar.value = current


func _on_stamina_changed(current: float, max_value: float) -> void:
	_stamina_bar.max_value = max_value
	_stamina_bar.value = current


func _on_faith_changed(current: float) -> void:
	_faith_label.text = "Faith: %d" % int(round(current))


func _on_lock_target_changed(target_name: String) -> void:
	if target_name.is_empty():
		_lock_on_label.text = "Lock-On: none"
	else:
		_lock_on_label.text = "Lock-On: %s" % target_name


func _on_context_hint_changed(message: String) -> void:
	_context_label.text = message


func _on_animation_state_changed(state_name: String) -> void:
	_animation_label.text = "Anim: %s" % state_name


func set_region_name(region_name: String) -> void:
	if _region_label == null:
		return
	_region_label.text = region_name


func set_objective(message: String) -> void:
	if _objective_label == null:
		return
	_objective_label.text = message


func show_message(message: String, duration: float = 3.5) -> void:
	if _message_label == null:
		return
	_message_label.text = message
	_message_timer = duration


func _update_miracle_label(label: Label, name: String, hotkey: String, remaining: float, unlocked: bool) -> void:
	if label == null:
		return
	if not unlocked:
		label.text = "%s: Locked" % name
		return
	if remaining <= 0.0:
		label.text = "%s: Ready (%s)" % [name, hotkey]
	else:
		label.text = "%s Cooldown: %.1fs" % [name, remaining]


func _on_experience_changed(current_xp: int, xp_to_next: int, level: int, skill_points: int) -> void:
	if _xp_label != null:
		_xp_label.text = "Level %d | XP %d | Next %d | SP %d" % [level, current_xp, xp_to_next, skill_points]
	if _player != null and _skill_tree_label != null:
		if _player.unlocked_skills.is_empty():
			_skill_tree_label.text = "No skills unlocked yet. Earn XP and spend skill points."
		else:
			_skill_tree_label.text = "Unlocked:\n- %s" % String("\n- ").join(_player.unlocked_skills)


func _on_class_changed(class_name: String) -> void:
	if _class_label != null:
		_class_label.text = "Class: %s" % class_name


func _on_level_up(level: int) -> void:
	show_popup("Level Up", "You reached level %d" % level)


func _on_skill_unlocked(skill_id: String) -> void:
	show_popup("Skill Unlocked", skill_id)


func show_popup(title: String, message: String, duration: float = 2.8) -> void:
	if _popup_label == null:
		return
	_popup_label.text = "%s: %s" % [title, message]
	_popup_timer = duration
