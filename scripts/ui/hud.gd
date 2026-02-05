extends CanvasLayer
class_name HUDLayer


var _player: PlayerController
var _health_bar: ProgressBar
var _stamina_bar: ProgressBar
var _faith_label: Label
var _lock_on_label: Label
var _context_label: Label
var _miracle_label: Label
var _animation_label: Label
var _ui_built: bool = false


func _ready() -> void:
	_build_ui()


func _process(_delta: float) -> void:
	if _player == null:
		return

	var remaining := _player.get_heal_cooldown_remaining()
	if remaining <= 0.0:
		_miracle_label.text = "Healing Miracle: Ready (R)"
	else:
		_miracle_label.text = "Healing Miracle Cooldown: %.1fs" % remaining


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

	_on_health_changed(_player.current_health, _player.max_health)
	_on_stamina_changed(_player.current_stamina, _player.max_stamina)
	_on_faith_changed(_player.current_faith)
	_on_lock_target_changed("")
	_on_context_hint_changed("")
	_on_animation_state_changed("Idle")


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
	panel.offset_bottom = 300.0
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
	title.text = "Calvary Souls - Galilee Prototype"
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

	_lock_on_label = Label.new()
	_lock_on_label.text = "Lock-On: none"
	layout.add_child(_lock_on_label)

	_miracle_label = Label.new()
	_miracle_label.text = "Healing Miracle: Ready (R)"
	layout.add_child(_miracle_label)

	_animation_label = Label.new()
	_animation_label.text = "Anim: Idle"
	layout.add_child(_animation_label)

	_context_label = Label.new()
	_context_label.text = ""
	_context_label.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
	_context_label.custom_minimum_size = Vector2(360.0, 46.0)
	layout.add_child(_context_label)

	var controls := Label.new()
	controls.text = "WASD Move | LMB Light | RMB Heavy | Space Dodge | Shift Block | F Parry | Q Lock | R Heal | E Pray | Save F1-F3 | Load F5-F7"
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
