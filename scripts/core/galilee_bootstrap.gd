extends Node3D


var _player: PlayerController
var _hud: HUDLayer


func _ready() -> void:
	_ensure_input_map()
	_build_world()
	_spawn_player()
	_spawn_prayer_site()
	_spawn_phase_two_enemies()
	_spawn_hud()


func _ensure_input_map() -> void:
	_register_key_action("move_forward", [Key.W, Key.UP])
	_register_key_action("move_backward", [Key.S, Key.DOWN])
	_register_key_action("move_left", [Key.A, Key.LEFT])
	_register_key_action("move_right", [Key.D, Key.RIGHT])
	_register_key_action("interact", [Key.E])
	_register_key_action("lock_on", [Key.Q])
	_register_key_action("parry", [Key.F])
	_register_key_action("miracle_heal", [Key.R])
	_register_key_action("block", [Key.SHIFT])
	_register_key_action("dodge", [Key.SPACE])

	_register_key_action("save_slot_1", [Key.F1])
	_register_key_action("save_slot_2", [Key.F2])
	_register_key_action("save_slot_3", [Key.F3])
	_register_key_action("load_slot_1", [Key.F5])
	_register_key_action("load_slot_2", [Key.F6])
	_register_key_action("load_slot_3", [Key.F7])

	var light_attack := InputEventMouseButton.new()
	light_attack.button_index = MouseButton.LEFT
	_register_action_event("light_attack", light_attack)

	var heavy_attack := InputEventMouseButton.new()
	heavy_attack.button_index = MouseButton.RIGHT
	_register_action_event("heavy_attack", heavy_attack)


func _register_key_action(action_name: String, keycodes: Array[int]) -> void:
	for keycode in keycodes:
		var event := InputEventKey.new()
		event.keycode = keycode
		event.physical_keycode = keycode
		_register_action_event(action_name, event)


func _register_action_event(action_name: String, event: InputEvent) -> void:
	if not InputMap.has_action(action_name):
		InputMap.add_action(action_name)

	if _action_has_event(action_name, event):
		return

	InputMap.action_add_event(action_name, event)


func _action_has_event(action_name: String, candidate: InputEvent) -> bool:
	for current in InputMap.action_get_events(action_name):
		if current.as_text() == candidate.as_text():
			return true
	return false


func _build_world() -> void:
	var sun := DirectionalLight3D.new()
	sun.name = "SunLight"
	sun.rotation_degrees = Vector3(-46.0, -38.0, 0.0)
	sun.light_energy = 1.35
	add_child(sun)

	var world_environment := WorldEnvironment.new()
	world_environment.environment = _create_environment_resource()
	add_child(world_environment)

	var ground := StaticBody3D.new()
	ground.name = "GalileeGround"
	add_child(ground)

	var ground_shape := CollisionShape3D.new()
	var ground_box := BoxShape3D.new()
	ground_box.size = Vector3(120.0, 1.0, 120.0)
	ground_shape.shape = ground_box
	ground_shape.position = Vector3(0.0, -0.5, 0.0)
	ground.add_child(ground_shape)

	var ground_mesh := MeshInstance3D.new()
	var ground_visual := BoxMesh.new()
	ground_visual.size = Vector3(120.0, 1.0, 120.0)
	ground_mesh.mesh = ground_visual
	ground_mesh.position = Vector3(0.0, -0.5, 0.0)
	ground_mesh.material_override = _make_material(Color(0.62, 0.55, 0.42))
	ground.add_child(ground_mesh)

	_place_rock(Vector3(14.0, 0.8, 8.0), Vector3(3.0, 1.6, 2.5))
	_place_rock(Vector3(-11.0, 1.0, -7.0), Vector3(2.8, 2.0, 2.2))
	_place_rock(Vector3(8.0, 0.6, -15.0), Vector3(4.2, 1.2, 2.0))
	_place_rock(Vector3(26.0, 0.6, -28.0), Vector3(4.4, 1.1, 4.2))
	_place_rock(Vector3(-28.0, 0.9, -30.0), Vector3(3.6, 1.6, 3.1))
	_place_tree(Vector3(-16.0, 0.0, 12.0))
	_place_tree(Vector3(19.0, 0.0, -8.0))
	_place_tree(Vector3(-24.0, 0.0, -18.0))
	_place_tree(Vector3(27.0, 0.0, 15.0))

	var sea := MeshInstance3D.new()
	sea.name = "SeaOfGalileeBackdrop"
	var sea_mesh := PlaneMesh.new()
	sea_mesh.size = Vector2(100.0, 28.0)
	sea.mesh = sea_mesh
	sea.position = Vector3(0.0, 0.1, -42.0)
	sea.rotation_degrees.x = -90.0
	sea.material_override = _make_material(Color(0.24, 0.39, 0.48))
	add_child(sea)

	_build_boss_arena()


func _build_boss_arena() -> void:
	var arena_center := Vector3(0.0, 0.02, -34.0)
	var arena_floor := MeshInstance3D.new()
	var cylinder := CylinderMesh.new()
	cylinder.top_radius = 9.0
	cylinder.bottom_radius = 9.0
	cylinder.height = 0.08
	arena_floor.mesh = cylinder
	arena_floor.position = arena_center
	arena_floor.material_override = _make_material(Color(0.33, 0.24, 0.21))
	add_child(arena_floor)

	for i in range(10):
		var pillar := MeshInstance3D.new()
		var pillar_mesh := CylinderMesh.new()
		pillar_mesh.top_radius = 0.28
		pillar_mesh.bottom_radius = 0.34
		pillar_mesh.height = 2.4
		pillar.mesh = pillar_mesh
		var angle := TAU * float(i) / 10.0
		pillar.position = arena_center + Vector3(cos(angle) * 9.5, 1.2, sin(angle) * 9.5)
		pillar.material_override = _make_material(Color(0.44, 0.29, 0.24))
		add_child(pillar)


func _create_environment_resource() -> Environment:
	var environment := Environment.new()
	environment.background_mode = Environment.BG_COLOR
	environment.background_color = Color(0.76, 0.78, 0.74)
	environment.ambient_light_color = Color(0.64, 0.58, 0.49)
	environment.ambient_light_energy = 1.1
	environment.fog_enabled = true
	environment.fog_light_color = Color(0.81, 0.73, 0.62)
	environment.fog_light_energy = 1.0
	environment.fog_density = 0.015
	return environment


func _place_rock(at: Vector3, size: Vector3) -> void:
	var rock := StaticBody3D.new()
	rock.name = "Rock"
	add_child(rock)
	rock.position = at

	var collision := CollisionShape3D.new()
	var shape := BoxShape3D.new()
	shape.size = size
	collision.shape = shape
	rock.add_child(collision)

	var mesh_instance := MeshInstance3D.new()
	var mesh := BoxMesh.new()
	mesh.size = size
	mesh_instance.mesh = mesh
	mesh_instance.material_override = _make_material(Color(0.44, 0.38, 0.32))
	rock.add_child(mesh_instance)


func _place_tree(at: Vector3) -> void:
	var tree := Node3D.new()
	tree.position = at
	add_child(tree)

	var trunk := MeshInstance3D.new()
	var trunk_mesh := CylinderMesh.new()
	trunk_mesh.height = 2.8
	trunk_mesh.top_radius = 0.24
	trunk_mesh.bottom_radius = 0.33
	trunk.mesh = trunk_mesh
	trunk.position = Vector3(0.0, 1.4, 0.0)
	trunk.material_override = _make_material(Color(0.42, 0.28, 0.16))
	tree.add_child(trunk)

	var leaves := MeshInstance3D.new()
	var leaves_mesh := SphereMesh.new()
	leaves_mesh.radius = 1.2
	leaves.mesh = leaves_mesh
	leaves.position = Vector3(0.0, 3.1, 0.0)
	leaves.material_override = _make_material(Color(0.37, 0.52, 0.26))
	tree.add_child(leaves)


func _make_material(color: Color) -> StandardMaterial3D:
	var material := StandardMaterial3D.new()
	material.albedo_color = color
	material.roughness = 0.9
	return material


func _spawn_player() -> void:
	_player = PlayerController.new()
	_player.name = "JesusPlayer"
	_player.global_position = Vector3(0.0, 1.2, 12.0)
	add_child(_player)


func _spawn_prayer_site() -> void:
	var prayer_site := PrayerSite.new()
	prayer_site.name = "PrayerSite_Galilee"
	prayer_site.site_id = "galilee_site_01"
	prayer_site.site_name = "Prayer Site: Galilee Shores"
	prayer_site.global_position = Vector3(0.0, 0.15, 2.0)
	add_child(prayer_site)


func _spawn_phase_two_enemies() -> void:
	var enemy_specs := [
		{
			"id": "galilee_shield_01",
			"name": "Roman Shieldbearer I",
			"type": EnemyAI.EnemyType.MELEE_SHIELD,
			"position": Vector3(9.0, 1.0, -3.0),
		},
		{
			"id": "galilee_shield_02",
			"name": "Roman Shieldbearer II",
			"type": EnemyAI.EnemyType.MELEE_SHIELD,
			"position": Vector3(-9.0, 1.0, -5.0),
		},
		{
			"id": "galilee_spear_01",
			"name": "Roman Spearman I",
			"type": EnemyAI.EnemyType.SPEAR,
			"position": Vector3(15.0, 1.0, 11.0),
		},
		{
			"id": "galilee_spear_02",
			"name": "Roman Spearman II",
			"type": EnemyAI.EnemyType.SPEAR,
			"position": Vector3(-14.0, 1.0, 12.0),
		},
		{
			"id": "galilee_ranged_01",
			"name": "Roman Slinger I",
			"type": EnemyAI.EnemyType.RANGED,
			"position": Vector3(18.0, 1.0, -14.0),
		},
		{
			"id": "galilee_ranged_02",
			"name": "Roman Slinger II",
			"type": EnemyAI.EnemyType.RANGED,
			"position": Vector3(-18.0, 1.0, -15.0),
		},
		{
			"id": "galilee_demon_01",
			"name": "Unclean Spirit I",
			"type": EnemyAI.EnemyType.DEMON,
			"position": Vector3(5.0, 1.0, -20.0),
		},
		{
			"id": "galilee_demon_02",
			"name": "Unclean Spirit II",
			"type": EnemyAI.EnemyType.DEMON,
			"position": Vector3(-6.0, 1.0, -22.0),
		},
		{
			"id": "galilee_named_boss_01",
			"name": "Legion Sovereign of Gerasa",
			"type": EnemyAI.EnemyType.BOSS,
			"position": Vector3(0.0, 1.0, -34.0),
		},
	]

	for spec in enemy_specs:
		var enemy := EnemyAI.new()
		enemy.spawn_id = str(spec["id"])
		enemy.enemy_name = str(spec["name"])
		enemy.enemy_type = int(spec["type"])
		enemy.global_position = spec["position"] as Vector3
		add_child(enemy)


func _spawn_hud() -> void:
	_hud = HUDLayer.new()
	add_child(_hud)
	_hud.bind_player(_player)
