extends Node
class_name CameraShake


## Screen shake system for impact feedback and dramatic moments
## Provides various shake patterns for different gameplay situations


signal shake_started(intensity: float)
signal shake_finished()


enum ShakeType {
	IMPACT,          ## Short, sharp shake for hits and impacts
	RUMBLE,          ## Sustained shake for ongoing effects
	EXPLOSION,       ## Large shake with decay
	SUBTLE_TREMOR,   ## Very light shake for ambient effects
}


@export_category("Shake Settings")
@export var max_shake_distance: float = 0.5
@export var trauma_decay_rate: float = 1.5
@export var shake_frequency: float = 20.0


var _target_camera: Camera3D
var _original_transform: Transform3D
var _trauma: float = 0.0
var _shake_time: float = 0.0
var _is_shaking: bool = false


func _ready() -> void:
	set_physics_process(false)


func initialize(camera: Camera3D) -> void:
	if camera == null:
		push_error("CameraShake: Cannot initialize with null camera")
		return

	_target_camera = camera
	_original_transform = camera.transform
	set_physics_process(true)


func _physics_process(delta: float) -> void:
	if _target_camera == null:
		return

	if _trauma > 0.0:
		_apply_shake(delta)
		_trauma = max(_trauma - trauma_decay_rate * delta, 0.0)

		if not _is_shaking:
			_is_shaking = true
			shake_started.emit(_trauma)
	else:
		if _is_shaking:
			_reset_camera()
			_is_shaking = false
			shake_finished.emit()


func add_trauma(amount: float) -> void:
	## Add trauma to trigger screen shake. Amount is clamped 0-1
	_trauma = min(_trauma + amount, 1.0)


func add_shake(shake_type: ShakeType, intensity: float = 1.0) -> void:
	## Trigger a specific type of shake with given intensity (0-1)
	match shake_type:
		ShakeType.IMPACT:
			add_trauma(0.3 * intensity)
		ShakeType.RUMBLE:
			add_trauma(0.6 * intensity)
		ShakeType.EXPLOSION:
			add_trauma(0.8 * intensity)
		ShakeType.SUBTLE_TREMOR:
			add_trauma(0.1 * intensity)


func shake_at_position(world_position: Vector3, intensity: float, max_distance: float = 20.0) -> void:
	## Apply shake based on distance from a world position
	## Useful for explosions, impacts, etc.
	if _target_camera == null:
		return

	var distance := _target_camera.global_position.distance_to(world_position)
	if distance > max_distance:
		return

	var distance_factor := 1.0 - (distance / max_distance)
	add_trauma(intensity * distance_factor)


func clear_shake() -> void:
	## Immediately stop all shaking
	_trauma = 0.0
	_reset_camera()


func _apply_shake(delta: float) -> void:
	_shake_time += delta * shake_frequency

	# Use trauma squared for more dramatic effect
	var shake_amount := _trauma * _trauma

	# Generate shake offsets using noise-like patterns
	var offset_x := _shake_pattern(_shake_time) * shake_amount * max_shake_distance
	var offset_y := _shake_pattern(_shake_time + 100.0) * shake_amount * max_shake_distance
	var offset_z := _shake_pattern(_shake_time + 200.0) * shake_amount * max_shake_distance * 0.3

	# Apply rotation shake
	var rotation_x := _shake_pattern(_shake_time + 300.0) * shake_amount * 0.08
	var rotation_y := _shake_pattern(_shake_time + 400.0) * shake_amount * 0.05
	var rotation_z := _shake_pattern(_shake_time + 500.0) * shake_amount * 0.1

	# Store base position/rotation from parent transform
	var base_transform := _target_camera.transform
	base_transform.origin += Vector3(offset_x, offset_y, offset_z)

	# Apply rotation shake
	var shake_basis := Basis()
	shake_basis = shake_basis.rotated(Vector3.RIGHT, rotation_x)
	shake_basis = shake_basis.rotated(Vector3.UP, rotation_y)
	shake_basis = shake_basis.rotated(Vector3.FORWARD, rotation_z)

	_target_camera.transform.origin = base_transform.origin
	_target_camera.transform.basis = base_transform.basis * shake_basis


func _shake_pattern(time: float) -> float:
	## Generate a pseudo-random shake pattern using sine waves
	return sin(time) * 0.5 + sin(time * 0.7) * 0.3 + sin(time * 1.3) * 0.2


func _reset_camera() -> void:
	if _target_camera == null:
		return
	# Reset any shake-applied offsets while preserving parent-driven position
	# The camera's position is managed by the player controller, so we don't override that
