extends CharacterBody3D
class_name PlayerController


const SaveSystemScript := preload("res://scripts/persistence/save_system.gd")

const MIRACLE_HEAL := "heal"
const MIRACLE_BLESSING := "blessing"
const MIRACLE_RADIANCE := "radiance"


signal health_changed(current: float, max_value: float)
signal stamina_changed(current: float, max_value: float)
signal faith_changed(current: float)
signal lock_target_changed(target_name: String)
signal context_hint_changed(message: String)
signal animation_state_changed(state_name: String)


enum AttackType {
	NONE,
	LIGHT,
	HEAVY,
}


enum AnimationState {
	IDLE,
	RUN,
	LIGHT_ATTACK,
	HEAVY_ATTACK,
	DODGE,
	BLOCK,
	PARRY,
	CAST,
	HURT,
	DEAD,
}


@export_category("Vitals")
@export var max_health: float = 120.0
@export var max_stamina: float = 100.0
@export var stamina_regeneration: float = 22.0
@export var starting_faith: float = 35.0

@export_category("Movement")
@export var walk_speed: float = 5.8
@export var acceleration: float = 18.0
@export var dodge_speed: float = 11.2

@export_category("Combat")
@export var light_attack_damage: float = 26.0
@export var heavy_attack_damage: float = 42.0
@export var light_attack_poise_damage: float = 28.0
@export var heavy_attack_poise_damage: float = 54.0
@export var light_attack_stamina_cost: float = 18.0
@export var heavy_attack_stamina_cost: float = 32.0
@export var dodge_stamina_cost: float = 26.0
@export var parry_stamina_cost: float = 20.0
@export var lock_on_range: float = 20.0
@export var light_attack_range: float = 2.9
@export var heavy_attack_range: float = 3.4

@export_category("Miracles")
@export var heal_amount: float = 45.0
@export var heal_faith_cost: float = 18.0
@export var heal_cooldown: float = 6.5
@export var blessing_faith_cost: float = 22.0
@export var blessing_duration: float = 5.0
@export var blessing_cooldown: float = 14.0
@export var blessing_damage_multiplier: float = 0.65
@export var blessing_stamina_multiplier: float = 0.7
@export var radiance_faith_cost: float = 30.0
@export var radiance_cooldown: float = 12.0
@export var radiance_damage: float = 32.0
@export var radiance_poise_damage: float = 38.0
@export var radiance_radius: float = 6.0


var current_health: float
var current_stamina: float
var current_faith: float

var _attack_cooldown: float = 0.0
var _dodge_timer: float = 0.0
var _invulnerability_timer: float = 0.0
var _parry_window: float = 0.0
var _parry_startup_timer: float = 0.0
var _heal_cooldown_timer: float = 0.0
var _blessing_timer: float = 0.0
var _blessing_cooldown_timer: float = 0.0
var _radiance_cooldown_timer: float = 0.0
var _perfect_block_timer: float = 0.0
var _hurt_timer: float = 0.0

var _attack_type: AttackType = AttackType.NONE
var _attack_windup_timer: float = 0.0
var _attack_active_timer: float = 0.0
var _attack_has_resolved: bool = false

var _look_yaw: float = 0.0
var _look_pitch: float = -0.24
var _dodge_direction: Vector3 = Vector3.ZERO
var _lock_target: EnemyAI
var _is_blocking: bool = false
var _gravity: float = float(ProjectSettings.get_setting("physics/3d/default_gravity"))
var _context_hint: String = ""
var _animation_state: AnimationState = AnimationState.IDLE
var _animation_time: float = 0.0

var _unlocked_miracles: Dictionary = { MIRACLE_HEAL: true }
var _campaign_manager: CampaignManager

var _active_prayer_site: PrayerSite
var _last_rest_site_id: String = ""

var _camera_rig: Node3D
var _camera_pitch: Node3D
var _spring_arm: SpringArm3D
var _camera: Camera3D


func _ready() -> void:
	add_to_group("player")
	_build_collider_and_mesh()
	_build_camera()

	current_health = max_health
	current_stamina = max_stamina
	current_faith = starting_faith

	Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)
	_emit_all_stats()
	emit_signal("lock_target_changed", "")
	emit_signal("context_hint_changed", "")
	_set_animation_state(AnimationState.IDLE)


func _input(event: InputEvent) -> void:
	if event is InputEventMouseMotion and Input.mouse_mode == Input.MOUSE_MODE_CAPTURED:
		var motion := event as InputEventMouseMotion
		_look_yaw -= motion.relative.x * 0.0034
		_look_pitch = clamp(_look_pitch - motion.relative.y * 0.0024, -0.75, 0.45)

	if event.is_action_pressed("toggle_mouse"):
		if Input.mouse_mode == Input.MOUSE_MODE_CAPTURED:
			Input.mouse_mode = Input.MOUSE_MODE_VISIBLE
		else:
			Input.mouse_mode = Input.MOUSE_MODE_CAPTURED


func _physics_process(delta: float) -> void:
	_update_timers(delta)
	_regen_stamina(delta)
	_validate_lock_target()
	_handle_combat_input()
	_handle_prayer_site_slot_input()
	_update_attack_timing(delta)
	_handle_movement(delta)
	_update_camera(delta)
	_update_visual_animation(delta)


func _build_collider_and_mesh() -> void:
	if get_node_or_null("BodyCollision") == null:
		var collision := CollisionShape3D.new()
		collision.name = "BodyCollision"
		var capsule := CapsuleShape3D.new()
		capsule.radius = 0.36
		capsule.height = 1.2
		collision.shape = capsule
		collision.position = Vector3(0.0, 1.0, 0.0)
		add_child(collision)

	if get_node_or_null("BodyMesh") == null:
		# Multi-part robed figure for Jesus (non-placeholder silhouette)
		var body_root := MeshInstance3D.new()
		body_root.name = "BodyMesh"
		body_root.position = Vector3(0.0, 0.0, 0.0)
		var body_core := CapsuleMesh.new()
		body_core.radius = 0.28
		body_core.mid_height = 0.74
		body_root.mesh = body_core
		add_child(body_root)

		var robe_mat := StandardMaterial3D.new()
		robe_mat.albedo_color = Color(0.88, 0.84, 0.74)
		robe_mat.roughness = 0.82
		robe_mat.metallic = 0.0

		body_root.material_override = robe_mat

		# Robe lower drape
		var robe := MeshInstance3D.new()
		robe.name = "Robe"
		var robe_mesh := PrismMesh.new()
		robe_mesh.left_to_right = 0.9
		robe_mesh.size = Vector3(0.86, 1.1, 0.72)
		robe.mesh = robe_mesh
		robe.position = Vector3(0.0, 0.55, 0.0)
		robe.rotation_degrees.y = 180.0
		robe.material_override = robe_mat
		body_root.add_child(robe)

		# Sash / belt
		var sash := MeshInstance3D.new()
		var sash_mesh := TorusMesh.new()
		sash_mesh.ring_radius = 0.3
		sash_mesh.pipe_radius = 0.034
		sash.mesh = sash_mesh
		sash.position = Vector3(0.0, 0.85, 0.0)
		var sash_mat := StandardMaterial3D.new()
		sash_mat.albedo_color = Color(0.52, 0.38, 0.22)
		sash_mat.roughness = 0.78
		sash.material_override = sash_mat
		body_root.add_child(sash)

		# Cloak layer
		var torso := MeshInstance3D.new()
		var torso_mesh := CapsuleMesh.new()
		torso_mesh.radius = 0.24
		torso_mesh.mid_height = 0.38
		torso.mesh = torso_mesh
		torso.position = Vector3(0.0, 1.45, 0.0)
		torso.material_override = robe_mat
		body_root.add_child(torso)

		# Head
		var head := MeshInstance3D.new()
		var head_mesh := SphereMesh.new()
		head_mesh.radius = 0.18
		head_mesh.height = 0.38
		head.mesh = head_mesh
		head.position = Vector3(0.0, 1.9, 0.0)
		var skin_mat := StandardMaterial3D.new()
		skin_mat.albedo_color = Color(0.78, 0.62, 0.48)
		skin_mat.roughness = 0.72
		head.material_override = skin_mat
		body_root.add_child(head)

		# Hair
		var hair := MeshInstance3D.new()
		var hair_mesh := SphereMesh.new()
		hair_mesh.radius = 0.2
		hair_mesh.height = 0.28
		hair.mesh = hair_mesh
		hair.position = Vector3(0.0, 1.96, -0.02)
		hair.scale = Vector3(1.0, 0.8, 1.1)
		var hair_mat := StandardMaterial3D.new()
		hair_mat.albedo_color = Color(0.22, 0.15, 0.1)
		hair_mat.roughness = 0.92
		hair.material_override = hair_mat
		body_root.add_child(hair)

		# Left arm
		var left_arm := MeshInstance3D.new()
		var arm_mesh := CapsuleMesh.new()
		arm_mesh.radius = 0.07
		arm_mesh.mid_height = 0.38
		left_arm.mesh = arm_mesh
		left_arm.position = Vector3(-0.32, 1.3, 0.0)
		left_arm.rotation_degrees.z = 12.0
		left_arm.material_override = robe_mat
		body_root.add_child(left_arm)

		# Right arm
		var right_arm := MeshInstance3D.new()
		right_arm.mesh = arm_mesh
		right_arm.position = Vector3(0.32, 1.3, 0.0)
		right_arm.rotation_degrees.z = -12.0
		right_arm.material_override = robe_mat
		body_root.add_child(right_arm)

		# Holy aura glow (subtle)
		var aura_light := OmniLight3D.new()
		aura_light.name = "PlayerAura"
		aura_light.position = Vector3(0.0, 1.4, 0.0)
		aura_light.light_color = Color(1.0, 0.95, 0.8)
		aura_light.light_energy = 0.35
		aura_light.omni_range = 3.5
		aura_light.shadow_enabled = false
		body_root.add_child(aura_light)


func _build_camera() -> void:
	_camera_rig = Node3D.new()
	_camera_rig.name = "CameraRig"
	_camera_rig.top_level = true
	add_child(_camera_rig)

	_camera_pitch = Node3D.new()
	_camera_pitch.name = "CameraPitch"
	_camera_rig.add_child(_camera_pitch)

	_spring_arm = SpringArm3D.new()
	_spring_arm.name = "SpringArm3D"
	_spring_arm.spring_length = 4.8
	_spring_arm.margin = 0.18
	_spring_arm.collision_mask = 1
	_camera_pitch.add_child(_spring_arm)

	_camera = Camera3D.new()
	_camera.name = "PlayerCamera"
	_camera.current = true
	_camera.fov = 68.0
	_spring_arm.add_child(_camera)


func _handle_combat_input() -> void:
	var block_just_pressed := Input.is_action_just_pressed("block")
	_is_blocking = Input.is_action_pressed("block") and _dodge_timer <= 0.0 and _attack_type == AttackType.NONE
	if block_just_pressed:
		_perfect_block_timer = 0.18
		if not _is_busy():
			_set_animation_state(AnimationState.BLOCK)

	if Input.is_action_just_pressed("lock_on"):
		_toggle_lock_on()

	if Input.is_action_just_pressed("light_attack"):
		_try_light_attack()

	if Input.is_action_just_pressed("heavy_attack"):
		_try_heavy_attack()

	if Input.is_action_just_pressed("dodge"):
		_try_dodge()

	if Input.is_action_just_pressed("parry"):
		_try_parry()

	if Input.is_action_just_pressed("miracle_heal"):
		_try_healing_miracle()

	if Input.is_action_just_pressed("miracle_blessing"):
		_try_blessing_miracle()

	if Input.is_action_just_pressed("miracle_radiance"):
		_try_radiance_miracle()


func _handle_prayer_site_slot_input() -> void:
	if _active_prayer_site == null:
		return

	if Input.is_action_just_pressed("save_slot_1"):
		save_to_slot(1)
	elif Input.is_action_just_pressed("save_slot_2"):
		save_to_slot(2)
	elif Input.is_action_just_pressed("save_slot_3"):
		save_to_slot(3)
	elif Input.is_action_just_pressed("load_slot_1"):
		load_from_slot(1)
	elif Input.is_action_just_pressed("load_slot_2"):
		load_from_slot(2)
	elif Input.is_action_just_pressed("load_slot_3"):
		load_from_slot(3)


func _handle_movement(delta: float) -> void:
	var move_input := Input.get_vector("move_left", "move_right", "move_backward", "move_forward")
	var desired_direction := Vector3.ZERO
	if move_input.length() > 0.01:
		var basis := _camera.global_transform.basis
		var cam_forward := -basis.z
		cam_forward.y = 0.0
		cam_forward = cam_forward.normalized()
		var cam_right := basis.x
		cam_right.y = 0.0
		cam_right = cam_right.normalized()
		desired_direction = (cam_right * move_input.x + cam_forward * move_input.y).normalized()

	var movement_multiplier := 1.0
	if _is_blocking:
		movement_multiplier = min(movement_multiplier, 0.45)
	if _attack_type != AttackType.NONE and _attack_windup_timer > 0.0:
		movement_multiplier = min(movement_multiplier, 0.28)
	elif _attack_type != AttackType.NONE:
		movement_multiplier = min(movement_multiplier, 0.62)
	if _hurt_timer > 0.0:
		movement_multiplier = min(movement_multiplier, 0.35)

	if _dodge_timer > 0.0:
		velocity.x = _dodge_direction.x * dodge_speed
		velocity.z = _dodge_direction.z * dodge_speed
		_set_animation_state(AnimationState.DODGE)
	else:
		var target_speed := walk_speed * movement_multiplier
		velocity.x = move_toward(velocity.x, desired_direction.x * target_speed, acceleration * delta)
		velocity.z = move_toward(velocity.z, desired_direction.z * target_speed, acceleration * delta)

		var lock_target := _lock_target
		if lock_target != null and is_instance_valid(lock_target) and not lock_target.is_redeemed():
			var lock_node := lock_target as Node3D
			if lock_node != null:
				var lock_direction: Vector3 = lock_node.global_position - global_position
				lock_direction.y = 0.0
				if lock_direction.length() > 0.01:
					rotation.y = lerp_angle(rotation.y, atan2(lock_direction.x, lock_direction.z), delta * 14.0)
		elif desired_direction.length() > 0.1:
			rotation.y = lerp_angle(rotation.y, atan2(desired_direction.x, desired_direction.z), delta * 10.0)

		_update_locomotion_animation(desired_direction)

	if not is_on_floor():
		velocity.y -= _gravity * delta
	else:
		velocity.y = -0.01

	move_and_slide()


func _update_locomotion_animation(desired_direction: Vector3) -> void:
	if _hurt_timer > 0.0:
		_set_animation_state(AnimationState.HURT)
		return
	if _attack_type != AttackType.NONE:
		return
	if _parry_startup_timer > 0.0 or _parry_window > 0.0:
		return
	if _is_blocking:
		_set_animation_state(AnimationState.BLOCK)
		return
	if desired_direction.length() > 0.08:
		_set_animation_state(AnimationState.RUN)
	else:
		_set_animation_state(AnimationState.IDLE)


func _update_camera(delta: float) -> void:
	_camera_rig.global_position = global_position + Vector3(0.0, 1.55, 0.0)

	var lock_target := _lock_target
	if lock_target != null and is_instance_valid(lock_target) and not lock_target.is_redeemed():
		var lock_node := lock_target as Node3D
		if lock_node != null:
			var to_target: Vector3 = (lock_node.global_position + Vector3(0.0, 1.1, 0.0)) - _camera_rig.global_position
			var desired_yaw: float = atan2(to_target.x, to_target.z)
			var flat_distance: float = maxf(Vector2(to_target.x, to_target.z).length(), 0.05)
			var desired_pitch: float = clampf(-atan2(to_target.y, flat_distance) - 0.08, -0.7, 0.35)
			_look_yaw = lerp_angle(_look_yaw, desired_yaw, delta * 6.0)
			_look_pitch = lerp(_look_pitch, desired_pitch, delta * 3.0)

	_camera_rig.rotation.y = _look_yaw
	_camera_pitch.rotation.x = _look_pitch


func _update_timers(delta: float) -> void:
	_attack_cooldown = max(_attack_cooldown - delta, 0.0)
	_dodge_timer = max(_dodge_timer - delta, 0.0)
	_invulnerability_timer = max(_invulnerability_timer - delta, 0.0)
	_heal_cooldown_timer = max(_heal_cooldown_timer - delta, 0.0)
	_blessing_timer = max(_blessing_timer - delta, 0.0)
	_blessing_cooldown_timer = max(_blessing_cooldown_timer - delta, 0.0)
	_radiance_cooldown_timer = max(_radiance_cooldown_timer - delta, 0.0)
	_perfect_block_timer = max(_perfect_block_timer - delta, 0.0)
	_hurt_timer = max(_hurt_timer - delta, 0.0)

	if _parry_startup_timer > 0.0:
		_parry_startup_timer = max(_parry_startup_timer - delta, 0.0)
		if _parry_startup_timer <= 0.0:
			_parry_window = 0.23
	else:
		_parry_window = max(_parry_window - delta, 0.0)


func _update_attack_timing(delta: float) -> void:
	if _attack_type == AttackType.NONE:
		return

	if _attack_windup_timer > 0.0:
		_attack_windup_timer = max(_attack_windup_timer - delta, 0.0)
		if _attack_windup_timer <= 0.0 and not _attack_has_resolved:
			_resolve_pending_attack()

	if _attack_windup_timer <= 0.0:
		if not _attack_has_resolved:
			_resolve_pending_attack()
		_attack_active_timer = max(_attack_active_timer - delta, 0.0)
		if _attack_active_timer <= 0.0:
			_attack_type = AttackType.NONE
			_attack_has_resolved = false


func _regen_stamina(delta: float) -> void:
	if _dodge_timer > 0.0:
		return
	if _is_blocking:
		return
	if _attack_cooldown > 0.0:
		return

	var regen_rate := stamina_regeneration
	if _blessing_timer > 0.0:
		regen_rate *= 1.35

	var previous := current_stamina
	current_stamina = min(current_stamina + regen_rate * delta, max_stamina)
	if not is_equal_approx(previous, current_stamina):
		emit_signal("stamina_changed", current_stamina, max_stamina)


func _is_busy() -> bool:
	return _attack_cooldown > 0.0 or _dodge_timer > 0.0 or _hurt_timer > 0.0


func _consume_stamina(cost: float) -> bool:
	if current_stamina < cost:
		return false

	current_stamina -= cost
	emit_signal("stamina_changed", current_stamina, max_stamina)
	return true


func _try_light_attack() -> void:
	if _is_busy() or _attack_type != AttackType.NONE:
		return
	if not _consume_stamina(light_attack_stamina_cost):
		return

	_attack_type = AttackType.LIGHT
	_attack_windup_timer = 0.15
	_attack_active_timer = 0.2
	_attack_has_resolved = false
	_attack_cooldown = 0.5
	_set_animation_state(AnimationState.LIGHT_ATTACK)


func _try_heavy_attack() -> void:
	if _is_busy() or _attack_type != AttackType.NONE:
		return
	if not _consume_stamina(heavy_attack_stamina_cost):
		return

	_attack_type = AttackType.HEAVY
	_attack_windup_timer = 0.3
	_attack_active_timer = 0.23
	_attack_has_resolved = false
	_attack_cooldown = 0.84
	_set_animation_state(AnimationState.HEAVY_ATTACK)


func _resolve_pending_attack() -> void:
	var enemy
	if _attack_type == AttackType.LIGHT:
		enemy = _find_attack_target(light_attack_range, 70.0)
	elif _attack_type == AttackType.HEAVY:
		enemy = _find_attack_target(heavy_attack_range, 80.0)
	else:
		return

	_attack_has_resolved = true
	if enemy == null:
		return

	if _attack_type == AttackType.LIGHT:
		if enemy.is_parried():
			enemy.receive_riposte(heavy_attack_damage * 1.08, self)
		else:
			enemy.receive_hit(light_attack_damage, light_attack_poise_damage, self)
			add_faith(1.0)
	else:
		if enemy.is_parried():
			enemy.receive_riposte(heavy_attack_damage * 1.5, self)
		else:
			enemy.receive_hit(heavy_attack_damage, heavy_attack_poise_damage, self)
			add_faith(1.5)


func _try_dodge() -> void:
	if _is_busy():
		return
	if not _consume_stamina(dodge_stamina_cost):
		return

	_attack_type = AttackType.NONE
	_attack_cooldown = 0.28
	_dodge_timer = 0.28
	_invulnerability_timer = 0.22
	var move_input := Input.get_vector("move_left", "move_right", "move_backward", "move_forward")
	if move_input.length() <= 0.05:
		_dodge_direction = -global_basis.z
	else:
		var basis := _camera.global_transform.basis
		var cam_forward := -basis.z
		cam_forward.y = 0.0
		cam_forward = cam_forward.normalized()
		var cam_right := basis.x
		cam_right.y = 0.0
		cam_right = cam_right.normalized()
		_dodge_direction = (cam_right * move_input.x + cam_forward * move_input.y).normalized()
	_set_animation_state(AnimationState.DODGE)


func _try_parry() -> void:
	if _is_busy():
		return
	if not _consume_stamina(parry_stamina_cost):
		return

	_attack_cooldown = 0.42
	_parry_startup_timer = 0.08
	_parry_window = 0.0
	_set_animation_state(AnimationState.PARRY)


func _try_healing_miracle() -> void:
	if _heal_cooldown_timer > 0.0:
		return
	if current_faith < heal_faith_cost:
		return
	if current_health >= max_health:
		return

	current_faith -= heal_faith_cost
	current_health = min(current_health + heal_amount, max_health)
	_heal_cooldown_timer = heal_cooldown
	_attack_cooldown = max(_attack_cooldown, 0.35)

	emit_signal("faith_changed", current_faith)
	emit_signal("health_changed", current_health, max_health)
	_set_animation_state(AnimationState.CAST)

	_apply_miracle_highlight(Color(0.92, 0.83, 0.55), 1.4)


func _try_blessing_miracle() -> void:
	if not is_miracle_unlocked(MIRACLE_BLESSING):
		set_context_hint("Blessing miracle not yet unlocked.")
		return
	if _blessing_cooldown_timer > 0.0:
		return
	if current_faith < blessing_faith_cost:
		return

	current_faith -= blessing_faith_cost
	_blessing_timer = blessing_duration
	_blessing_cooldown_timer = blessing_cooldown
	_attack_cooldown = max(_attack_cooldown, 0.35)

	emit_signal("faith_changed", current_faith)
	_set_animation_state(AnimationState.CAST)
	_apply_miracle_highlight(Color(0.88, 0.78, 0.52), 1.1)


func _try_radiance_miracle() -> void:
	if not is_miracle_unlocked(MIRACLE_RADIANCE):
		set_context_hint("Radiance miracle not yet unlocked.")
		return
	if _radiance_cooldown_timer > 0.0:
		return
	if current_faith < radiance_faith_cost:
		return

	current_faith -= radiance_faith_cost
	_radiance_cooldown_timer = radiance_cooldown
	_attack_cooldown = max(_attack_cooldown, 0.45)

	for node in get_tree().get_nodes_in_group("enemies"):
		var enemy := node as EnemyAI
		if enemy == null or enemy.is_redeemed():
			continue
		var to_enemy := enemy.global_position - global_position
		if to_enemy.length() <= radiance_radius:
			enemy.receive_hit(radiance_damage, radiance_poise_damage, self)
			if enemy.has_method("apply_knockback"):
				enemy.apply_knockback(to_enemy.normalized(), 4.5)

	emit_signal("faith_changed", current_faith)
	_set_animation_state(AnimationState.CAST)
	_apply_miracle_highlight(Color(0.95, 0.75, 0.38), 1.35)


func _apply_miracle_highlight(color: Color, energy: float) -> void:
	var mesh := get_node_or_null("BodyMesh") as MeshInstance3D
	if mesh != null and mesh.material_override is StandardMaterial3D:
		var highlight := (mesh.material_override as StandardMaterial3D).duplicate() as StandardMaterial3D
		highlight.emission_enabled = true
		highlight.emission = color
		highlight.emission_energy_multiplier = energy
		mesh.material_override = highlight


func receive_enemy_attack(attacker, damage: float, posture_damage: float) -> void:
	if _invulnerability_timer > 0.0:
		return

	if _parry_window > 0.0 and attacker != null and attacker.can_be_parried():
		attacker.on_parried(self)
		add_faith(2.0)
		_parry_window = 0.0
		return

	var damage_multiplier := 1.0
	var posture_multiplier := 1.0
	if _blessing_timer > 0.0:
		damage_multiplier = blessing_damage_multiplier
		posture_multiplier = blessing_stamina_multiplier

	var adjusted_damage := damage * damage_multiplier
	var adjusted_posture := posture_damage * posture_multiplier

	if _is_blocking and current_stamina > 0.0:
		var stamina_loss := adjusted_posture * 1.2
		if _perfect_block_timer > 0.0:
			stamina_loss *= 0.45
			add_faith(1.0)
		current_stamina = max(current_stamina - stamina_loss, 0.0)
		emit_signal("stamina_changed", current_stamina, max_stamina)
		if current_stamina <= 0.01 and _perfect_block_timer <= 0.0:
			_apply_health_damage(adjusted_damage * 0.35)
		return

	_apply_health_damage(adjusted_damage)


func _apply_health_damage(amount: float) -> void:
	current_health = max(current_health - amount, 0.0)
	emit_signal("health_changed", current_health, max_health)
	_hurt_timer = 0.22
	_set_animation_state(AnimationState.HURT)

	if current_health <= 0.01:
		_handle_defeat()


func _handle_defeat() -> void:
	_set_animation_state(AnimationState.DEAD)
	var prayer_sites := get_tree().get_nodes_in_group("prayer_sites")
	if prayer_sites.is_empty():
		current_health = max_health
		current_stamina = max_stamina
		global_position = Vector3(0.0, 1.2, 10.0)
		_emit_all_stats()
		return

	var closest_site := prayer_sites[0]
	var closest_site_node := closest_site as Node3D
	var closest_distance: float = INF
	if closest_site_node != null:
		closest_distance = global_position.distance_squared_to(closest_site_node.global_position)
	for node in prayer_sites:
		var site_node := node as Node3D
		if site_node == null:
			continue
		var test_distance: float = global_position.distance_squared_to(site_node.global_position)
		if test_distance < closest_distance:
			closest_distance = test_distance
			closest_site = node

	rest_at_prayer_site(closest_site)


func rest_at_prayer_site(site) -> void:
	if site == null:
		return

	global_position = site.get_respawn_position()
	velocity = Vector3.ZERO
	current_health = max_health
	current_stamina = max_stamina
	_heal_cooldown_timer = 0.0
	_last_rest_site_id = site.site_id
	set_context_hint(site.get_prompt_message())
	emit_signal("health_changed", current_health, max_health)
	emit_signal("stamina_changed", current_stamina, max_stamina)
	_set_animation_state(AnimationState.IDLE)

	get_tree().call_group("enemies", "reset_to_spawn")
	_clear_lock_on()


func set_active_prayer_site(site) -> void:
	_active_prayer_site = site
	set_context_hint(site.get_prompt_message())


func clear_active_prayer_site(site) -> void:
	if site != _active_prayer_site:
		return
	_active_prayer_site = null
	set_context_hint("")

func set_campaign_manager(manager: CampaignManager) -> void:
	_campaign_manager = manager


func set_unlocked_miracles(miracles: Array) -> void:
	_unlocked_miracles.clear()
	_unlocked_miracles[MIRACLE_HEAL] = true
	for miracle in miracles:
		_unlocked_miracles[str(miracle)] = true


func is_miracle_unlocked(miracle_id: String) -> bool:
	return _unlocked_miracles.has(miracle_id)


func get_blessing_cooldown_remaining() -> float:
	return _blessing_cooldown_timer


func get_radiance_cooldown_remaining() -> float:
	return _radiance_cooldown_timer


func build_save_payload() -> Dictionary:
	return _build_save_payload()


func save_to_slot(slot_id: int) -> bool:
	if _active_prayer_site == null:
		set_context_hint("You can only save while standing at a Prayer Site.")
		return false

	var ok := false
	if _campaign_manager != null:
		ok = _campaign_manager.save_slot(slot_id, self)
	else:
		var payload := _build_save_payload()
		ok = SaveSystemScript.save_slot(slot_id, payload)
	if ok:
		set_context_hint("Saved to slot %d (%s)." % [slot_id, Time.get_datetime_string_from_system(true)])
	else:
		set_context_hint("Save failed for slot %d." % slot_id)
	return ok


func load_from_slot(slot_id: int) -> bool:
	if _active_prayer_site == null:
		set_context_hint("You can only load while standing at a Prayer Site.")
		return false

	var ok := false
	if _campaign_manager != null:
		ok = _campaign_manager.load_slot(slot_id)
	else:
		var payload := SaveSystemScript.load_slot(slot_id)
		if payload.is_empty():
			set_context_hint("Slot %d is empty." % slot_id)
			return false
		ok = apply_save_payload(payload)
	if ok:
		set_context_hint("Loaded slot %d." % slot_id)
	else:
		set_context_hint("Load failed for slot %d." % slot_id)
	return ok


func _build_save_payload() -> Dictionary:
	var enemy_states := {}
	for node in get_tree().get_nodes_in_group("enemies"):
		var enemy := node
		if enemy == null or not enemy.has_method("serialize_state"):
			continue
		enemy_states[enemy.spawn_id] = enemy.serialize_state()

	return {
		"player": {
			"position": [global_position.x, global_position.y, global_position.z],
			"health": current_health,
			"stamina": current_stamina,
			"faith": current_faith,
			"last_rest_site": _last_rest_site_id,
		},
		"world": {
			"enemies": enemy_states,
		},
	}


func apply_save_payload(payload: Dictionary) -> bool:
	var player_dict: Dictionary = payload.get("player", {})

	global_position = _array_to_vector3(player_dict.get("position", [0.0, 1.2, 10.0]), global_position)
	velocity = Vector3.ZERO
	current_health = clamp(float(player_dict.get("health", max_health)), 0.0, max_health)
	current_stamina = clamp(float(player_dict.get("stamina", max_stamina)), 0.0, max_stamina)
	current_faith = max(float(player_dict.get("faith", current_faith)), 0.0)
	_last_rest_site_id = str(player_dict.get("last_rest_site", _last_rest_site_id))
	_heal_cooldown_timer = 0.0
	_blessing_timer = 0.0
	_blessing_cooldown_timer = 0.0
	_radiance_cooldown_timer = 0.0
	_attack_type = AttackType.NONE
	_attack_windup_timer = 0.0
	_attack_active_timer = 0.0
	_attack_has_resolved = false
	_hurt_timer = 0.0
	_clear_lock_on()
	_emit_all_stats()

	var world_dict: Dictionary = payload.get("world", {})
	var enemy_dict: Dictionary = world_dict.get("enemies", {})
	for node in get_tree().get_nodes_in_group("enemies"):
		var enemy := node
		if enemy == null or not enemy.has_method("apply_saved_state"):
			continue
		if enemy_dict.has(enemy.spawn_id) and enemy_dict[enemy.spawn_id] is Dictionary:
			enemy.apply_saved_state(enemy_dict[enemy.spawn_id] as Dictionary)
		else:
			enemy.reset_to_spawn()

	return true


func _array_to_vector3(value: Variant, fallback: Vector3) -> Vector3:
	if value is Array:
		var array := value as Array
		if array.size() >= 3:
			return Vector3(float(array[0]), float(array[1]), float(array[2]))
	return fallback


func add_faith(amount: float) -> void:
	current_faith += amount
	emit_signal("faith_changed", current_faith)


func _find_attack_target(max_distance: float, arc_degrees: float):
	var lock_target := _lock_target
	if lock_target != null and is_instance_valid(lock_target) and not lock_target.is_redeemed():
		var lock_node := lock_target as Node3D
		if lock_node != null and global_position.distance_to(lock_node.global_position) <= max_distance + 0.7:
			return lock_target

	var best_target = null
	var best_distance: float = INF
	var forward: Vector3 = -global_basis.z
	for node in get_tree().get_nodes_in_group("enemies"):
		var enemy := node
		if enemy == null or enemy.is_redeemed():
			continue
		var enemy_node := enemy as Node3D
		if enemy_node == null:
			continue
		var to_enemy: Vector3 = enemy_node.global_position - global_position
		to_enemy.y = 0.0
		var distance: float = to_enemy.length()
		if distance > max_distance or distance < 0.1:
			continue
		var dot_value: float = forward.normalized().dot(to_enemy.normalized())
		dot_value = clampf(dot_value, -1.0, 1.0)
		var angle: float = rad_to_deg(acos(dot_value))
		if angle > arc_degrees * 0.5:
			continue
		if distance < best_distance:
			best_distance = distance
			best_target = enemy
	return best_target


func _toggle_lock_on() -> void:
	if _lock_target != null and is_instance_valid(_lock_target):
		_clear_lock_on()
		return

	var nearest = null
	var nearest_distance: float = lock_on_range
	for node in get_tree().get_nodes_in_group("enemies"):
		var enemy := node
		if enemy == null or enemy.is_redeemed():
			continue
		var enemy_node := enemy as Node3D
		if enemy_node == null:
			continue
		var distance: float = global_position.distance_to(enemy_node.global_position)
		if distance < nearest_distance:
			nearest_distance = distance
			nearest = enemy

	_lock_target = nearest
	if _lock_target == null:
		emit_signal("lock_target_changed", "")
	else:
		emit_signal("lock_target_changed", _lock_target.enemy_name)


func _validate_lock_target() -> void:
	if _lock_target == null:
		return
	if not is_instance_valid(_lock_target):
		_clear_lock_on()
		return
	if _lock_target.is_redeemed():
		_clear_lock_on()
		return
	if global_position.distance_to(_lock_target.global_position) > lock_on_range * 1.2:
		_clear_lock_on()


func _clear_lock_on() -> void:
	_lock_target = null
	emit_signal("lock_target_changed", "")


func set_context_hint(message: String) -> void:
	if _context_hint == message:
		return
	_context_hint = message
	emit_signal("context_hint_changed", _context_hint)


func get_heal_cooldown_remaining() -> float:
	return _heal_cooldown_timer


func _emit_all_stats() -> void:
	emit_signal("health_changed", current_health, max_health)
	emit_signal("stamina_changed", current_stamina, max_stamina)
	emit_signal("faith_changed", current_faith)


func _set_animation_state(next_state: AnimationState) -> void:
	if _animation_state == next_state:
		return
	_animation_state = next_state
	emit_signal("animation_state_changed", _animation_state_name(next_state))


func _animation_state_name(state_name: AnimationState) -> String:
	match state_name:
		AnimationState.IDLE:
			return "Idle"
		AnimationState.RUN:
			return "Run"
		AnimationState.LIGHT_ATTACK:
			return "LightAttack"
		AnimationState.HEAVY_ATTACK:
			return "HeavyAttack"
		AnimationState.DODGE:
			return "Dodge"
		AnimationState.BLOCK:
			return "Block"
		AnimationState.PARRY:
			return "Parry"
		AnimationState.CAST:
			return "Cast"
		AnimationState.HURT:
			return "Hurt"
		AnimationState.DEAD:
			return "Dead"
	return "Unknown"


func _update_visual_animation(delta: float) -> void:
	var mesh := get_node_or_null("BodyMesh") as MeshInstance3D
	if mesh == null:
		return

	_animation_time += delta
	match _animation_state:
		AnimationState.IDLE:
			mesh.scale = Vector3.ONE * (1.0 + sin(_animation_time * 2.2) * 0.01)
		AnimationState.RUN:
			var bob := sin(_animation_time * 11.0)
			mesh.scale = Vector3(1.02 - bob * 0.014, 0.98 + bob * 0.032, 1.02 - bob * 0.014)
		AnimationState.LIGHT_ATTACK:
			mesh.scale = Vector3(1.05, 0.94, 1.08)
		AnimationState.HEAVY_ATTACK:
			mesh.scale = Vector3(1.1, 0.9, 1.12)
		AnimationState.DODGE:
			mesh.scale = Vector3(0.92, 0.88, 1.2)
		AnimationState.BLOCK:
			mesh.scale = Vector3(1.08, 0.97, 0.94)
		AnimationState.PARRY:
			mesh.scale = Vector3(0.96, 1.04, 0.96)
		AnimationState.CAST:
			mesh.scale = Vector3(0.97, 1.07, 0.97)
		AnimationState.HURT:
			mesh.scale = Vector3(0.9, 1.06, 0.9)
		AnimationState.DEAD:
			mesh.scale = mesh.scale.lerp(Vector3(1.15, 0.25, 1.15), 0.08)
