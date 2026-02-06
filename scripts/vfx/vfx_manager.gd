extends CanvasLayer
class_name VFXManager


## Central manager for all visual effects in the game
## Handles damage numbers, screen shake, and other visual feedback


const DamageNumberScript := preload("res://scripts/vfx/damage_number.gd")
const CameraShakeScript := preload("res://scripts/vfx/camera_shake.gd")


var _camera: Camera3D
var _camera_shake: CameraShake
var _damage_number_container: Control


func _ready() -> void:
	_setup_damage_number_container()
	_setup_camera_shake()


func _setup_damage_number_container() -> void:
	_damage_number_container = Control.new()
	_damage_number_container.name = "DamageNumberContainer"
	_damage_number_container.set_anchors_preset(Control.PRESET_FULL_RECT)
	_damage_number_container.mouse_filter = Control.MOUSE_FILTER_IGNORE
	add_child(_damage_number_container)


func _setup_camera_shake() -> void:
	_camera_shake = CameraShakeScript.new()
	_camera_shake.name = "CameraShake"
	add_child(_camera_shake)


func initialize(camera: Camera3D) -> void:
	_camera = camera
	if _camera_shake != null:
		_camera_shake.initialize(camera)


func spawn_damage_number(
	damage_value: float,
	world_position: Vector3,
	damage_type: DamageNumberScript.DamageType = DamageNumberScript.DamageType.NORMAL
) -> void:
	## Spawn a floating damage number at a world position
	if _camera == null:
		push_warning("VFXManager: Cannot spawn damage number without camera")
		return

	var damage_number := DamageNumberScript.new()
	_damage_number_container.add_child(damage_number)
	damage_number.initialize(damage_value, world_position, _camera, damage_type)


func add_screen_shake(shake_type: CameraShakeScript.ShakeType, intensity: float = 1.0) -> void:
	## Trigger screen shake effect
	if _camera_shake != null:
		_camera_shake.add_shake(shake_type, intensity)


func add_shake_at_position(world_position: Vector3, intensity: float, max_distance: float = 20.0) -> void:
	## Trigger screen shake based on distance from a world position
	if _camera_shake != null:
		_camera_shake.shake_at_position(world_position, intensity, max_distance)


func clear_shake() -> void:
	## Immediately stop all screen shaking
	if _camera_shake != null:
		_camera_shake.clear_shake()
