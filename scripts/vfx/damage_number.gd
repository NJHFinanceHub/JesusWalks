extends Control
class_name DamageNumber


## Floating damage number that appears in 3D world space
## Animates upward and fades out over time


enum DamageType {
	NORMAL,
	CRITICAL,
	HEAL,
	POISE_BREAK,
	BLOCKED,
}


@export var lifetime: float = 1.2
@export var rise_speed: float = 80.0
@export var horizontal_spread: float = 30.0


var _label: Label
var _camera: Camera3D
var _world_position: Vector3
var _velocity: Vector2
var _elapsed: float = 0.0
var _damage_type: DamageType = DamageType.NORMAL


func _ready() -> void:
	_build_label()
	set_process(false)


func initialize(
	damage_value: float,
	world_pos: Vector3,
	camera: Camera3D,
	damage_type: DamageType = DamageType.NORMAL
) -> void:
	if camera == null:
		push_warning("DamageNumber: Cannot initialize without camera")
		queue_free()
		return

	_camera = camera
	_world_position = world_pos
	_damage_type = damage_type

	# Set random horizontal velocity for spread
	var angle := randf() * TAU
	_velocity = Vector2(
		cos(angle) * horizontal_spread,
		-rise_speed
	)

	# Configure label based on damage type
	_configure_for_type(damage_value)

	set_process(true)


func _process(delta: float) -> void:
	_elapsed += delta

	if _elapsed >= lifetime:
		queue_free()
		return

	# Update position
	_update_screen_position(delta)

	# Update opacity (fade out in last third of lifetime)
	var fade_start := lifetime * 0.66
	if _elapsed > fade_start:
		var fade_progress := (_elapsed - fade_start) / (lifetime - fade_start)
		modulate.a = 1.0 - fade_progress


func _build_label() -> void:
	_label = Label.new()
	_label.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	_label.vertical_alignment = VERTICAL_ALIGNMENT_CENTER

	# Add outline effect
	_label.add_theme_color_override("font_outline_color", Color(0.0, 0.0, 0.0, 0.8))
	_label.add_theme_constant_override("outline_size", 3)

	add_child(_label)


func _configure_for_type(value: float) -> void:
	match _damage_type:
		DamageType.NORMAL:
			_label.text = "%.0f" % value
			_label.add_theme_font_size_override("font_size", 22)
			_label.add_theme_color_override("font_color", Color(0.95, 0.92, 0.85))

		DamageType.CRITICAL:
			_label.text = "%.0f!" % value
			_label.add_theme_font_size_override("font_size", 32)
			_label.add_theme_color_override("font_color", Color(1.0, 0.85, 0.3))
			# Add slight glow
			_label.add_theme_color_override("font_outline_color", Color(0.8, 0.4, 0.0, 0.6))
			_label.add_theme_constant_override("outline_size", 4)
			# Increase movement for emphasis
			_velocity.y *= 1.3

		DamageType.HEAL:
			_label.text = "+%.0f" % value
			_label.add_theme_font_size_override("font_size", 24)
			_label.add_theme_color_override("font_color", Color(0.4, 0.95, 0.5))
			_label.add_theme_color_override("font_outline_color", Color(0.1, 0.3, 0.1, 0.8))

		DamageType.POISE_BREAK:
			_label.text = "STAGGER!"
			_label.add_theme_font_size_override("font_size", 26)
			_label.add_theme_color_override("font_color", Color(0.95, 0.7, 0.25))
			_label.add_theme_color_override("font_outline_color", Color(0.3, 0.2, 0.0, 0.8))
			_velocity.y *= 1.2

		DamageType.BLOCKED:
			_label.text = "BLOCKED"
			_label.add_theme_font_size_override("font_size", 20)
			_label.add_theme_color_override("font_color", Color(0.6, 0.6, 0.7))
			_velocity.y *= 0.7


func _update_screen_position(delta: float) -> void:
	if _camera == null:
		visible = false
		return

	# Move world position upward over time
	_world_position.y += abs(_velocity.y) * delta * 0.01

	# Check if position is behind camera (basic frustum culling)
	var cam_to_pos := _world_position - _camera.global_position
	if cam_to_pos.dot(-_camera.global_transform.basis.z) < 0:
		visible = false
		return

	visible = true

	# Project to screen space
	var screen_pos := _camera.unproject_position(_world_position)

	# Update position
	position = screen_pos - size / 2.0
