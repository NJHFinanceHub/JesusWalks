extends CharacterBody3D
class_name EnemyAI


signal redeemed(enemy_name: String, faith_reward: float)
signal phase_changed(enemy_name: String, phase: int)
signal animation_state_changed(enemy_name: String, state_name: String)


enum EnemyType {
	MELEE_SHIELD,
	SPEAR,
	RANGED,
	DEMON,
	BOSS,
}


enum State {
	IDLE,
	CHASE,
	RETREAT,
	STRAFE,
	WINDUP,
	CASTING,
	BLOCKING,
	RECOVER,
	STAGGERED,
	PARRIED,
	REDEEMED,
}


enum AnimationState {
	IDLE,
	RUN,
	ATTACK,
	BLOCK,
	CAST,
	HIT,
	STAGGER,
	PARRIED,
	DEATH,
}


@export_category("Identity")
@export var enemy_name: String = "Roman Patrol"
@export var spawn_id: String = ""
@export var enemy_type: EnemyType = EnemyType.MELEE_SHIELD
@export var faith_reward: float = 12.0

@export_category("Vitals")
@export var max_health: float = 90.0
@export var max_poise: float = 80.0
@export var poise_regeneration: float = 10.0

@export_category("Combat")
@export var move_speed: float = 3.8
@export var attack_damage: float = 15.0
@export var posture_damage: float = 20.0
@export var detection_range: float = 13.0
@export var attack_range: float = 2.0
@export var minimum_range: float = 5.2
@export var attack_windup: float = 0.4
@export var attack_recovery: float = 1.1
@export var strike_timing_ratio: float = 0.72
@export var parry_window_start_ratio: float = 0.35
@export var parry_window_end_ratio: float = 0.73
@export var stagger_duration: float = 1.15
@export var parry_vulnerability_duration: float = 1.75
@export var projectile_speed: float = 18.0
@export var can_parry_attacks: bool = true
@export var shield_block_chance: float = 0.45


var current_health: float
var current_poise: float
var state: State = State.IDLE
var state_timer: float = 0.0
var spawn_transform: Transform3D
var target: PlayerController
var _gravity: float = float(ProjectSettings.get_setting("physics/3d/default_gravity"))
var _base_collision_layer: int
var _base_collision_mask: int

var _phase: int = 1
var _windup_duration: float = 0.0
var _windup_elapsed: float = 0.0
var _attack_has_resolved: bool = false
var _shot_cooldown: float = 0.0
var _dash_cooldown: float = 0.0
var _strafe_timer: float = 0.0
var _strafe_direction: int = 1
var _animation_state: AnimationState = AnimationState.IDLE
var _animation_time: float = 0.0
var _mesh_instance: MeshInstance3D
var _base_color: Color = Color(0.49, 0.32, 0.29)


func _ready() -> void:
	add_to_group("enemies")
	if spawn_id.is_empty():
		spawn_id = "%s_%s" % [enemy_name.replace(" ", "_").to_lower(), str(get_instance_id())]

	_build_collider_and_mesh()
	_configure_archetype()
	spawn_transform = global_transform
	_base_collision_layer = collision_layer
	_base_collision_mask = collision_mask
	current_health = max_health
	current_poise = max_poise
	_set_animation_state(AnimationState.IDLE)


func _physics_process(delta: float) -> void:
	if state == State.REDEEMED:
		return

	if target == null or not is_instance_valid(target):
		target = get_tree().get_first_node_in_group("player") as PlayerController
	if target == null:
		return

	_update_phase_if_boss()
	_shot_cooldown = max(_shot_cooldown - delta, 0.0)
	_dash_cooldown = max(_dash_cooldown - delta, 0.0)
	current_poise = min(current_poise + poise_regeneration * delta, max_poise)

	var distance_to_player := global_position.distance_to(target.global_position)
	match state:
		State.IDLE:
			_slow_to_stop(delta)
			_set_animation_state(AnimationState.IDLE)
			if distance_to_player <= detection_range:
				state = State.CHASE

		State.CHASE:
			_face_target(delta)
			if distance_to_player > detection_range * 1.35:
				state = State.IDLE
			else:
				_process_chase_behavior(distance_to_player, delta)

		State.WINDUP:
			_process_windup(delta, false)

		State.CASTING:
			_process_windup(delta, true)

		State.BLOCKING, State.RECOVER, State.STAGGERED, State.PARRIED:
			state_timer = max(state_timer - delta, 0.0)
			_slow_to_stop(delta)
			if state_timer <= 0.0:
				if state == State.PARRIED:
					state = State.STAGGERED
					state_timer = stagger_duration * 0.55
					_set_animation_state(AnimationState.STAGGER)
				else:
					state = State.CHASE

		State.RETREAT:
			_move_away_from_target(delta)
			if distance_to_player >= minimum_range + 0.7:
				state = State.CHASE

		State.STRAFE:
			_strafe_around_target(delta)
			state_timer = max(state_timer - delta, 0.0)
			if state_timer <= 0.0:
				state = State.CHASE

		State.REDEEMED:
			pass

	if not is_on_floor():
		velocity.y -= _gravity * delta
	else:
		velocity.y = -0.01

	move_and_slide()
	_update_visual_animation(delta)


func _build_collider_and_mesh() -> void:
	var collision := get_node_or_null("EnemyCollision") as CollisionShape3D
	if collision == null:
		collision = CollisionShape3D.new()
		collision.name = "EnemyCollision"
		var capsule := CapsuleShape3D.new()
		capsule.radius = 0.35
		capsule.height = 1.15
		collision.shape = capsule
		collision.position = Vector3(0.0, 1.0, 0.0)
		add_child(collision)

	_mesh_instance = get_node_or_null("EnemyMesh") as MeshInstance3D
	if _mesh_instance == null:
		_mesh_instance = MeshInstance3D.new()
		_mesh_instance.name = "EnemyMesh"
		var torso_mesh := CapsuleMesh.new()
		torso_mesh.radius = 0.24
		torso_mesh.mid_height = 0.62
		_mesh_instance.mesh = torso_mesh
		_mesh_instance.position = Vector3(0.0, 1.12, 0.0)
		add_child(_mesh_instance)

	var helmet := _mesh_instance.get_node_or_null("Helmet") as MeshInstance3D
	if helmet == null:
		helmet = MeshInstance3D.new()
		helmet.name = "Helmet"
		var helmet_mesh := SphereMesh.new()
		helmet_mesh.radius = 0.2
		helmet_mesh.height = 0.3
		helmet.mesh = helmet_mesh
		helmet.position = Vector3(0.0, 0.62, 0.0)
		_mesh_instance.add_child(helmet)

	var shoulders := _mesh_instance.get_node_or_null("Shoulders") as MeshInstance3D
	if shoulders == null:
		shoulders = MeshInstance3D.new()
		shoulders.name = "Shoulders"
		var shoulder_mesh := BoxMesh.new()
		shoulder_mesh.size = Vector3(0.72, 0.16, 0.3)
		shoulders.mesh = shoulder_mesh
		shoulders.position = Vector3(0.0, 0.4, 0.02)
		_mesh_instance.add_child(shoulders)

	var weapon := _mesh_instance.get_node_or_null("Weapon") as MeshInstance3D
	if weapon == null:
		weapon = MeshInstance3D.new()
		weapon.name = "Weapon"
		var weapon_mesh := BoxMesh.new()
		weapon_mesh.size = Vector3(0.12, 0.78, 0.12)
		weapon.mesh = weapon_mesh
		weapon.position = Vector3(0.38, 0.12, 0.0)
		weapon.rotation_degrees = Vector3(0.0, 0.0, -18.0)
		_mesh_instance.add_child(weapon)

	if _mesh_instance.material_override == null:
		_mesh_instance.material_override = StandardMaterial3D.new()
	var armor := _mesh_instance.material_override as StandardMaterial3D
	armor.roughness = 0.64
	armor.metallic = 0.18
	armor.albedo_color = _base_color

	if helmet.material_override == null:
		helmet.material_override = StandardMaterial3D.new()
	var helm_mat := helmet.material_override as StandardMaterial3D
	helm_mat.albedo_color = _base_color.lightened(0.12)
	helm_mat.metallic = 0.28
	helm_mat.roughness = 0.42

	if shoulders.material_override == null:
		shoulders.material_override = StandardMaterial3D.new()
	var shoulder_mat := shoulders.material_override as StandardMaterial3D
	shoulder_mat.albedo_color = _base_color.darkened(0.08)
	shoulder_mat.metallic = 0.2
	shoulder_mat.roughness = 0.5

	if weapon.material_override == null:
		weapon.material_override = StandardMaterial3D.new()
	var weapon_mat := weapon.material_override as StandardMaterial3D
	weapon_mat.albedo_color = Color(0.35, 0.3, 0.24)
	weapon_mat.metallic = 0.06
	weapon_mat.roughness = 0.72


func _configure_archetype() -> void:
	match enemy_type:
		EnemyType.MELEE_SHIELD:
			enemy_name = enemy_name if enemy_name != "Roman Patrol" else "Roman Shieldbearer"
			max_health = 110.0
			max_poise = 105.0
			move_speed = 3.1
			attack_damage = 16.0
			posture_damage = 25.0
			attack_range = 2.0
			attack_windup = 0.46
			attack_recovery = 1.15
			can_parry_attacks = true
			shield_block_chance = 0.62
			_base_color = Color(0.52, 0.35, 0.27)

		EnemyType.SPEAR:
			enemy_name = enemy_name if enemy_name != "Roman Patrol" else "Roman Spearman"
			max_health = 86.0
			max_poise = 72.0
			move_speed = 4.0
			attack_damage = 20.0
			posture_damage = 24.0
			attack_range = 3.4
			attack_windup = 0.52
			attack_recovery = 1.05
			can_parry_attacks = true
			shield_block_chance = 0.0
			_base_color = Color(0.42, 0.31, 0.24)

		EnemyType.RANGED:
			enemy_name = enemy_name if enemy_name != "Roman Patrol" else "Roman Slinger"
			max_health = 70.0
			max_poise = 55.0
			move_speed = 3.6
			attack_damage = 15.0
			posture_damage = 18.0
			attack_range = 13.0
			minimum_range = 4.8
			attack_windup = 0.62
			attack_recovery = 0.75
			can_parry_attacks = false
			shield_block_chance = 0.0
			projectile_speed = 20.0
			_base_color = Color(0.29, 0.34, 0.4)

		EnemyType.DEMON:
			enemy_name = enemy_name if enemy_name != "Roman Patrol" else "Unclean Spirit"
			max_health = 94.0
			max_poise = 65.0
			move_speed = 4.8
			attack_damage = 22.0
			posture_damage = 24.0
			attack_range = 2.4
			attack_windup = 0.34
			attack_recovery = 0.82
			can_parry_attacks = true
			shield_block_chance = 0.0
			_base_color = Color(0.32, 0.16, 0.19)

		EnemyType.BOSS:
			enemy_name = enemy_name if enemy_name != "Roman Patrol" else "Legion Sovereign"
			max_health = 420.0
			max_poise = 210.0
			move_speed = 3.9
			attack_damage = 29.0
			posture_damage = 36.0
			detection_range = 28.0
			attack_range = 3.6
			minimum_range = 7.0
			attack_windup = 0.58
			attack_recovery = 0.95
			parry_window_start_ratio = 0.42
			parry_window_end_ratio = 0.66
			parry_vulnerability_duration = 1.15
			stagger_duration = 0.92
			projectile_speed = 22.0
			can_parry_attacks = true
			shield_block_chance = 0.0
			_base_color = Color(0.47, 0.14, 0.14)

	_apply_mesh_tint(_base_color)


func _process_chase_behavior(distance_to_player: float, delta: float) -> void:
	match enemy_type:
		EnemyType.RANGED:
			if distance_to_player < minimum_range:
				state = State.RETREAT
			elif distance_to_player <= attack_range and _shot_cooldown <= 0.0:
				_begin_cast_attack()
			else:
				if distance_to_player > attack_range * 0.92:
					_move_toward_target(delta, _effective_move_speed())
				else:
					state = State.STRAFE
					state_timer = randf_range(0.65, 1.2)
					_strafe_direction = 1 if randf() > 0.5 else -1

		EnemyType.DEMON:
			if distance_to_player <= attack_range * 1.25:
				if _dash_cooldown <= 0.0 and randf() < 0.32:
					_lunge_at_target()
					_dash_cooldown = 2.0
				_begin_melee_attack()
			else:
				_move_toward_target(delta, _effective_move_speed() * 1.08)

		EnemyType.BOSS:
			if _phase >= 2 and distance_to_player > attack_range + 2.5 and _shot_cooldown <= 0.0 and randf() < 0.45:
				_begin_cast_attack()
			elif distance_to_player <= attack_range + 0.4:
				_begin_melee_attack()
			else:
				_move_toward_target(delta, _effective_move_speed())

		_:
			if distance_to_player <= attack_range:
				_begin_melee_attack()
			else:
				_move_toward_target(delta, _effective_move_speed())


func _begin_melee_attack() -> void:
	if state != State.CHASE:
		return

	state = State.WINDUP
	_windup_duration = _effective_windup()
	_windup_elapsed = 0.0
	_attack_has_resolved = false
	state_timer = _windup_duration
	_set_animation_state(AnimationState.ATTACK)


func _begin_cast_attack() -> void:
	if state != State.CHASE:
		return

	state = State.CASTING
	_windup_duration = max(0.22, _effective_windup() + 0.08)
	_windup_elapsed = 0.0
	_attack_has_resolved = false
	state_timer = _windup_duration
	_set_animation_state(AnimationState.CAST)


func _process_windup(delta: float, casting: bool) -> void:
	_face_target(delta)
	state_timer = max(state_timer - delta, 0.0)
	_windup_elapsed += delta
	_slow_to_stop(delta)

	if not _attack_has_resolved and _windup_elapsed >= _windup_duration * strike_timing_ratio:
		_attack_has_resolved = true
		if casting:
			_resolve_cast_attack()
		else:
			_resolve_melee_attack()

	if state_timer <= 0.0:
		state = State.RECOVER
		state_timer = _effective_recovery()
		_set_animation_state(AnimationState.IDLE)


func _resolve_melee_attack() -> void:
	if target == null or not is_instance_valid(target):
		return

	var bonus_range := 0.8
	if enemy_type == EnemyType.SPEAR:
		bonus_range = 1.1
	if enemy_type == EnemyType.BOSS and _phase >= 2:
		bonus_range = 1.2

	var distance_to_target := global_position.distance_to(target.global_position)
	if distance_to_target <= attack_range + bonus_range:
		target.receive_enemy_attack(self, _effective_attack_damage(), _effective_posture_damage())


func _resolve_cast_attack() -> void:
	if target == null or not is_instance_valid(target):
		return

	var projectile := EnemyProjectile.new()
	projectile.owner_enemy = self
	projectile.damage = _effective_attack_damage() * 0.88
	projectile.posture_damage = _effective_posture_damage() * 0.74
	projectile.speed = projectile_speed * _phase_speed_scale()

	var start := global_position + Vector3(0.0, 1.15, 0.0)
	var toward := (target.global_position + Vector3(0.0, 1.0, 0.0)) - start
	if toward.length() <= 0.001:
		toward = -global_basis.z
	projectile.direction = toward.normalized()
	projectile.global_position = start + projectile.direction * 0.8
	var parent := get_tree().current_scene
	if parent == null:
		parent = get_tree().root
	parent.add_child(projectile)

	_shot_cooldown = 1.65 / _phase_speed_scale()


func _move_toward_target(delta: float, speed: float) -> void:
	var target_node := target as Node3D
	if target_node == null:
		return
	var direction: Vector3 = target_node.global_position - global_position
	direction.y = 0.0
	if direction.length() <= 0.01:
		return
	direction = direction.normalized()

	velocity.x = move_toward(velocity.x, direction.x * speed, speed * delta * 5.0)
	velocity.z = move_toward(velocity.z, direction.z * speed, speed * delta * 5.0)
	_set_animation_state(AnimationState.RUN)


func _move_away_from_target(delta: float) -> void:
	var target_node := target as Node3D
	if target_node == null:
		return
	var direction: Vector3 = global_position - target_node.global_position
	direction.y = 0.0
	if direction.length() <= 0.01:
		return
	direction = direction.normalized()

	var speed := _effective_move_speed()
	velocity.x = move_toward(velocity.x, direction.x * speed, speed * delta * 5.0)
	velocity.z = move_toward(velocity.z, direction.z * speed, speed * delta * 5.0)
	_face_target(delta)
	_set_animation_state(AnimationState.RUN)


func _strafe_around_target(delta: float) -> void:
	var target_node := target as Node3D
	if target_node == null:
		return
	var forward: Vector3 = target_node.global_position - global_position
	forward.y = 0.0
	if forward.length() <= 0.01:
		return
	forward = forward.normalized()
	var right := Vector3(-forward.z, 0.0, forward.x) * float(_strafe_direction)
	var strafe: Vector3 = (right + forward * 0.18).normalized()
	var speed := _effective_move_speed() * 0.9

	velocity.x = move_toward(velocity.x, strafe.x * speed, speed * delta * 4.8)
	velocity.z = move_toward(velocity.z, strafe.z * speed, speed * delta * 4.8)
	_face_target(delta)
	_set_animation_state(AnimationState.RUN)


func _face_target(delta: float) -> void:
	var target_node := target as Node3D
	if target_node == null:
		return
	var direction: Vector3 = target_node.global_position - global_position
	direction.y = 0.0
	if direction.length() <= 0.01:
		return
	rotation.y = lerp_angle(rotation.y, atan2(direction.x, direction.z), delta * 11.0)


func _lunge_at_target() -> void:
	var target_node := target as Node3D
	if target_node == null:
		return
	var direction: Vector3 = target_node.global_position - global_position
	direction.y = 0.0
	if direction.length() <= 0.01:
		return
	direction = direction.normalized()
	velocity.x = direction.x * _effective_move_speed() * 1.9
	velocity.z = direction.z * _effective_move_speed() * 1.9


func _slow_to_stop(delta: float) -> void:
	var damping := _effective_move_speed() * delta * 6.0
	velocity.x = move_toward(velocity.x, 0.0, damping)
	velocity.z = move_toward(velocity.z, 0.0, damping)


func _update_phase_if_boss() -> void:
	if enemy_type != EnemyType.BOSS:
		return

	var health_ratio: float = current_health / maxf(max_health, 1.0)
	var next_phase := 1
	if health_ratio <= 0.33:
		next_phase = 3
	elif health_ratio <= 0.66:
		next_phase = 2

	if next_phase != _phase:
		_phase = next_phase
		phase_changed.emit(enemy_name, _phase)
		if _phase == 2:
			_apply_mesh_tint(Color(0.58, 0.16, 0.12), Color(0.62, 0.18, 0.12), 0.55)
		elif _phase == 3:
			_apply_mesh_tint(Color(0.68, 0.18, 0.1), Color(0.82, 0.24, 0.12), 0.9)


func _effective_move_speed() -> float:
	var value := move_speed
	if enemy_type == EnemyType.BOSS:
		value *= _phase_speed_scale()
	return value


func _effective_attack_damage() -> float:
	var value := attack_damage
	if enemy_type == EnemyType.BOSS:
		value *= (1.0 + float(_phase - 1) * 0.2)
	if enemy_type == EnemyType.DEMON:
		value *= 1.08
	return value


func _effective_posture_damage() -> float:
	var value := posture_damage
	if enemy_type == EnemyType.BOSS:
		value *= (1.0 + float(_phase - 1) * 0.16)
	return value


func _effective_windup() -> float:
	var value := attack_windup
	if enemy_type == EnemyType.BOSS:
		value *= (1.0 - float(_phase - 1) * 0.08)
	return max(value, 0.16)


func _effective_recovery() -> float:
	var value := attack_recovery
	if enemy_type == EnemyType.BOSS:
		value *= (1.0 - float(_phase - 1) * 0.06)
	return max(value, 0.24)


func _effective_parry_start_ratio() -> float:
	var value := parry_window_start_ratio
	if enemy_type == EnemyType.BOSS:
		value += float(_phase - 1) * 0.05
	return clamp(value, 0.1, 0.84)


func _effective_parry_end_ratio() -> float:
	var value := parry_window_end_ratio
	if enemy_type == EnemyType.BOSS:
		value -= float(_phase - 1) * 0.05
	return clamp(value, _effective_parry_start_ratio() + 0.06, 0.95)


func _phase_speed_scale() -> float:
	if enemy_type != EnemyType.BOSS:
		return 1.0
	return 1.0 + float(_phase - 1) * 0.12


func can_be_parried() -> bool:
	if not can_parry_attacks:
		return false
	if state != State.WINDUP:
		return false
	if _windup_duration <= 0.0:
		return false

	var ratio := _windup_elapsed / _windup_duration
	return ratio >= _effective_parry_start_ratio() and ratio <= _effective_parry_end_ratio()


func on_parried(by_player: PlayerController) -> void:
	if state == State.REDEEMED:
		return

	state = State.PARRIED
	state_timer = parry_vulnerability_duration
	if enemy_type == EnemyType.BOSS:
		state_timer *= 0.72
	current_poise = max_poise * 0.5
	_set_animation_state(AnimationState.PARRIED)
	if by_player != null:
		by_player.add_faith(3.0)


func is_parried() -> bool:
	return state == State.PARRIED


func is_redeemed() -> bool:
	return state == State.REDEEMED


func receive_hit(damage: float, poise_damage: float, source: PlayerController) -> void:
	if state == State.REDEEMED:
		return
	if _try_shield_block(source, poise_damage):
		return

	current_health -= damage
	current_poise -= poise_damage
	_set_animation_state(AnimationState.HIT)

	if current_poise <= 0.0:
		state = State.STAGGERED
		state_timer = stagger_duration
		if enemy_type == EnemyType.BOSS:
			state_timer *= 0.68
		current_poise = max_poise
		_set_animation_state(AnimationState.STAGGER)

	if current_health <= 0.0:
		_become_redeemed(source)


func receive_riposte(damage: float, source: PlayerController) -> void:
	if state != State.PARRIED and state != State.STAGGERED:
		return

	current_health -= damage
	if current_health <= 0.0:
		_become_redeemed(source)
		return

	state = State.STAGGERED
	state_timer = stagger_duration
	_set_animation_state(AnimationState.STAGGER)


func apply_knockback(direction: Vector3, force: float) -> void:
	if state == State.REDEEMED:
		return
	if direction.length() <= 0.01:
		return
	var push := direction.normalized() * force
	velocity.x += push.x
	velocity.z += push.z


func _try_shield_block(source: PlayerController, poise_damage: float) -> bool:
	if enemy_type != EnemyType.MELEE_SHIELD:
		return false
	if source == null:
		return false
	if state != State.CHASE and state != State.IDLE and state != State.RECOVER:
		return false
	if randf() > shield_block_chance:
		return false

	var attacker_direction: Vector3 = source.global_position - global_position
	attacker_direction.y = 0.0
	if attacker_direction.length() <= 0.01:
		return false
	attacker_direction = attacker_direction.normalized()
	var facing := (-global_basis.z).normalized()
	var frontal := facing.dot(attacker_direction) > 0.3
	if not frontal:
		return false

	current_poise = maxf(current_poise - poise_damage * 0.35, 0.0)
	state = State.BLOCKING
	state_timer = 0.26
	_set_animation_state(AnimationState.BLOCK)
	return true


func _become_redeemed(source: PlayerController) -> void:
	state = State.REDEEMED
	current_health = 0.0
	velocity = Vector3.ZERO
	collision_layer = 0
	collision_mask = 0
	visible = false
	set_physics_process(false)
	_set_animation_state(AnimationState.DEATH)
	emit_signal("redeemed", enemy_name, faith_reward)
	if source != null:
		source.add_faith(faith_reward)


func reset_to_spawn() -> void:
	global_transform = spawn_transform
	velocity = Vector3.ZERO
	current_health = max_health
	current_poise = max_poise
	state = State.IDLE
	state_timer = 0.0
	_phase = 1
	_windup_elapsed = 0.0
	_windup_duration = 0.0
	_attack_has_resolved = false
	_shot_cooldown = 0.0
	_dash_cooldown = 0.0
	visible = true
	collision_layer = _base_collision_layer
	collision_mask = _base_collision_mask
	set_physics_process(true)
	_set_animation_state(AnimationState.IDLE)
	_apply_mesh_tint(_base_color)


func serialize_state() -> Dictionary:
	return {
		"id": spawn_id,
		"name": enemy_name,
		"type": int(enemy_type),
		"position": [global_position.x, global_position.y, global_position.z],
		"health": current_health,
		"poise": current_poise,
		"redeemed": state == State.REDEEMED,
		"phase": _phase,
	}


func apply_saved_state(data: Dictionary) -> void:
	if data.is_empty():
		reset_to_spawn()
		return

	var saved_redeemed := bool(data.get("redeemed", false))
	if saved_redeemed:
		_become_redeemed(null)
		return

	visible = true
	collision_layer = _base_collision_layer
	collision_mask = _base_collision_mask
	set_physics_process(true)
	state = State.IDLE
	state_timer = 0.0
	_windup_elapsed = 0.0
	_windup_duration = 0.0
	_attack_has_resolved = false
	_shot_cooldown = 0.0
	_dash_cooldown = 0.0

	global_position = _array_to_vector3(data.get("position", [spawn_transform.origin.x, spawn_transform.origin.y, spawn_transform.origin.z]))
	current_health = clamp(float(data.get("health", max_health)), 0.0, max_health)
	current_poise = clamp(float(data.get("poise", max_poise)), 0.0, max_poise)
	_phase = clampi(int(data.get("phase", 1)), 1, 3)
	_set_animation_state(AnimationState.IDLE)

	if enemy_type == EnemyType.BOSS:
		if _phase == 1:
			_apply_mesh_tint(_base_color)
		elif _phase == 2:
			_apply_mesh_tint(Color(0.58, 0.16, 0.12), Color(0.62, 0.18, 0.12), 0.55)
		else:
			_apply_mesh_tint(Color(0.68, 0.18, 0.1), Color(0.82, 0.24, 0.12), 0.9)


func _array_to_vector3(value: Variant) -> Vector3:
	if value is Array:
		var array := value as Array
		if array.size() >= 3:
			return Vector3(float(array[0]), float(array[1]), float(array[2]))
	return spawn_transform.origin


func _set_animation_state(next_state: AnimationState) -> void:
	if _animation_state == next_state:
		return
	_animation_state = next_state
	animation_state_changed.emit(enemy_name, _animation_state_name(_animation_state))


func _animation_state_name(current: AnimationState) -> String:
	match current:
		AnimationState.IDLE:
			return "Idle"
		AnimationState.RUN:
			return "Run"
		AnimationState.ATTACK:
			return "Attack"
		AnimationState.BLOCK:
			return "Block"
		AnimationState.CAST:
			return "Cast"
		AnimationState.HIT:
			return "Hit"
		AnimationState.STAGGER:
			return "Stagger"
		AnimationState.PARRIED:
			return "Parried"
		AnimationState.DEATH:
			return "Death"
	return "Unknown"


func _update_visual_animation(delta: float) -> void:
	if _mesh_instance == null:
		return

	_animation_time += delta
	match _animation_state:
		AnimationState.IDLE:
			_mesh_instance.scale = Vector3.ONE * (1.0 + sin(_animation_time * 2.8) * 0.012)
		AnimationState.RUN:
			var bob := sin(_animation_time * 12.0)
			_mesh_instance.scale = Vector3(1.03 - bob * 0.015, 0.98 + bob * 0.03, 1.03 - bob * 0.015)
		AnimationState.ATTACK:
			_mesh_instance.scale = Vector3(1.08, 0.92, 1.08)
		AnimationState.BLOCK:
			_mesh_instance.scale = Vector3(1.13, 0.95, 0.95)
		AnimationState.CAST:
			_mesh_instance.scale = Vector3(0.94, 1.1, 0.94)
		AnimationState.HIT:
			_mesh_instance.scale = Vector3(0.9, 1.07, 0.9)
		AnimationState.STAGGER:
			_mesh_instance.scale = Vector3(1.06, 0.84, 1.06)
		AnimationState.PARRIED:
			_mesh_instance.scale = Vector3(0.87, 1.04, 0.87)
		AnimationState.DEATH:
			_mesh_instance.scale = _mesh_instance.scale.lerp(Vector3(1.2, 0.3, 1.2), 0.08)


func _apply_mesh_tint(base_color: Color, emission: Color = Color(0.0, 0.0, 0.0), emission_energy: float = 0.0) -> void:
	if _mesh_instance == null:
		return
	var material := _mesh_instance.material_override as StandardMaterial3D
	if material == null:
		return
	material.albedo_color = base_color
	material.roughness = 0.86
	if emission_energy > 0.0:
		material.emission_enabled = true
		material.emission = emission
		material.emission_energy_multiplier = emission_energy
	else:
		material.emission_enabled = false
