extends Control
class_name EnemyHealthBar


## 3D world-space health bar that follows an enemy
## Shows enemy name, health, and poise status


signal bar_hidden()
signal bar_shown()


@export_category("Display Settings")
@export var bar_width: float = 180.0
@export var bar_height: float = 18.0
@export var poise_bar_height: float = 6.0
@export var world_offset: Vector3 = Vector3(0.0, 2.2, 0.0)
@export var fade_delay: float = 3.0
@export var fade_duration: float = 0.5


var _enemy: EnemyAI
var _camera: Camera3D
var _name_label: Label
var _health_bar: ProgressBar
var _poise_bar: ProgressBar
var _background: PanelContainer
var _fade_timer: float = 0.0
var _is_locked_on: bool = false
var _is_visible: bool = false


func _ready() -> void:
	_build_ui()
	modulate.a = 0.0
	set_process(false)


func _process(delta: float) -> void:
	if _enemy == null or _camera == null:
		return

	# Update position to follow enemy
	_update_screen_position()

	# Update health/poise bars in real-time
	if _is_visible:
		_update_health_display()
		_update_poise_display()

	# Handle auto-fade when not locked on
	if not _is_locked_on and _is_visible:
		_fade_timer += delta
		if _fade_timer >= fade_delay:
			_fade_out()


func initialize(enemy: EnemyAI, camera: Camera3D) -> void:
	_enemy = enemy
	_camera = camera

	if _enemy == null or _camera == null:
		return

	# Connect to enemy signals
	_enemy.tree_exiting.connect(_on_enemy_removed)

	# Set initial values
	_update_health_display()
	_update_poise_display()

	set_process(true)


func set_locked_on(locked: bool) -> void:
	_is_locked_on = locked
	if locked:
		_show_bar()
	else:
		_fade_timer = 0.0  # Start fade timer when lock is released


func show_temporarily(duration: float = 2.0) -> void:
	## Show the health bar temporarily (e.g., when enemy is hit)
	if not _is_locked_on:
		_fade_timer = -duration  # Negative delay keeps it visible
		_show_bar()


func _build_ui() -> void:
	# Container with background
	_background = PanelContainer.new()
	_background.custom_minimum_size = Vector2(bar_width, bar_height + poise_bar_height + 30.0)
	add_child(_background)

	var style := StyleBoxFlat.new()
	style.bg_color = Color(0.08, 0.06, 0.05, 0.85)
	style.corner_radius_top_left = 4
	style.corner_radius_top_right = 4
	style.corner_radius_bottom_left = 4
	style.corner_radius_bottom_right = 4
	style.border_color = Color(0.4, 0.35, 0.3, 0.9)
	style.border_width_left = 1
	style.border_width_right = 1
	style.border_width_top = 1
	style.border_width_bottom = 1
	_background.add_theme_stylebox_override("panel", style)

	var layout := VBoxContainer.new()
	layout.add_theme_constant_override("separation", 3)
	_background.add_child(layout)

	# Enemy name
	_name_label = Label.new()
	_name_label.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	_name_label.add_theme_font_size_override("font_size", 13)
	_name_label.add_theme_color_override("font_color", Color(0.95, 0.92, 0.85))
	layout.add_child(_name_label)

	# Health bar
	_health_bar = ProgressBar.new()
	_health_bar.custom_minimum_size = Vector2(bar_width - 10.0, bar_height)
	_health_bar.show_percentage = false
	layout.add_child(_health_bar)

	var health_fill := StyleBoxFlat.new()
	health_fill.bg_color = Color(0.82, 0.24, 0.22)
	health_fill.corner_radius_top_left = 2
	health_fill.corner_radius_top_right = 2
	health_fill.corner_radius_bottom_left = 2
	health_fill.corner_radius_bottom_right = 2
	_health_bar.add_theme_stylebox_override("fill", health_fill)

	var health_bg := StyleBoxFlat.new()
	health_bg.bg_color = Color(0.22, 0.18, 0.16)
	_health_bar.add_theme_stylebox_override("background", health_bg)

	# Poise bar
	_poise_bar = ProgressBar.new()
	_poise_bar.custom_minimum_size = Vector2(bar_width - 10.0, poise_bar_height)
	_poise_bar.show_percentage = false
	layout.add_child(_poise_bar)

	var poise_fill := StyleBoxFlat.new()
	poise_fill.bg_color = Color(0.85, 0.72, 0.38)
	poise_fill.corner_radius_top_left = 2
	poise_fill.corner_radius_top_right = 2
	poise_fill.corner_radius_bottom_left = 2
	poise_fill.corner_radius_bottom_right = 2
	_poise_bar.add_theme_stylebox_override("fill", poise_fill)

	var poise_bg := StyleBoxFlat.new()
	poise_bg.bg_color = Color(0.22, 0.18, 0.16)
	_poise_bar.add_theme_stylebox_override("background", poise_bg)


func _update_screen_position() -> void:
	if _enemy == null or _camera == null:
		return

	# Calculate world position above enemy
	var world_pos := _enemy.global_position + world_offset

	# Check if enemy is in front of camera
	var cam_to_enemy := world_pos - _camera.global_position
	if cam_to_enemy.dot(_camera.global_transform.basis.z) > 0:
		# Enemy is behind camera, hide bar
		visible = false
		return

	visible = true

	# Project to screen space
	var screen_pos := _camera.unproject_position(world_pos)

	# Center the bar
	position = screen_pos - size / 2.0


func _update_health_display() -> void:
	if _enemy == null:
		return

	_name_label.text = _enemy.enemy_name
	_health_bar.max_value = _enemy.max_health
	_health_bar.value = _enemy.current_health


func _update_poise_display() -> void:
	if _enemy == null:
		return

	_poise_bar.max_value = _enemy.max_poise
	_poise_bar.value = _enemy.current_poise


func _show_bar() -> void:
	if _is_visible:
		return

	_update_health_display()
	_update_poise_display()

	_is_visible = true
	var tween := create_tween()
	tween.tween_property(self, "modulate:a", 1.0, 0.2)
	bar_shown.emit()


func _fade_out() -> void:
	if not _is_visible:
		return

	_is_visible = false
	var tween := create_tween()
	tween.tween_property(self, "modulate:a", 0.0, fade_duration)
	tween.tween_callback(func(): bar_hidden.emit())


func _on_enemy_removed() -> void:
	queue_free()
