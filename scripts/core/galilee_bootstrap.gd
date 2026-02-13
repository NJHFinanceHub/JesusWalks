extends Node3D

var _player: PlayerController
var _hud: HUDLayer
var _music_player: AudioStreamPlayer


func _ready() -> void:
	_ensure_input_map()
	_build_world()
	_spawn_music()
	_spawn_player()
	_spawn_prayer_site()
	_spawn_phase_two_enemies()
	_spawn_hud()


func _ensure_input_map() -> void:
	_register_key_action("move_forward", [Key.KEY_W, Key.KEY_UP])
	_register_key_action("move_backward", [Key.KEY_S, Key.KEY_DOWN])
	_register_key_action("move_left", [Key.KEY_A, Key.KEY_LEFT])
	_register_key_action("move_right", [Key.KEY_D, Key.KEY_RIGHT])
	_register_key_action("interact", [Key.KEY_E])
	_register_key_action("lock_on", [Key.KEY_Q])
	_register_key_action("parry", [Key.KEY_F])
	_register_key_action("miracle_heal", [Key.KEY_R])
	_register_key_action("miracle_blessing", [Key.KEY_1])
	_register_key_action("miracle_radiance", [Key.KEY_2])
	_register_key_action("block", [Key.KEY_SHIFT])
	_register_key_action("dodge", [Key.KEY_SPACE])
	_register_key_action("pause", [Key.KEY_ESCAPE])
	_register_key_action("toggle_mouse", [Key.KEY_TAB])

	_register_key_action("save_slot_1", [Key.KEY_F1])
	_register_key_action("save_slot_2", [Key.KEY_F2])
	_register_key_action("save_slot_3", [Key.KEY_F3])
	_register_key_action("load_slot_1", [Key.KEY_F5])
	_register_key_action("load_slot_2", [Key.KEY_F6])
	_register_key_action("load_slot_3", [Key.KEY_F7])

	var light_attack := InputEventMouseButton.new()
	light_attack.button_index = MouseButton.MOUSE_BUTTON_LEFT
	_register_action_event("light_attack", light_attack)

	var heavy_attack := InputEventMouseButton.new()
	heavy_attack.button_index = MouseButton.MOUSE_BUTTON_RIGHT
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


# ---------------------------------------------------------------------------
# WORLD BUILDING
# ---------------------------------------------------------------------------

func _build_world() -> void:
	_build_sky_and_lighting()
	_build_terrain()
	_build_sea_of_galilee()
	_build_shoreline()
	_scatter_rocks()
	_scatter_trees()
	_scatter_bushes()
	_build_stone_path()
	_build_ruins()
	_build_boss_arena()


func _build_sky_and_lighting() -> void:
	# Dramatic golden-hour sun with shadows
	var sun := DirectionalLight3D.new()
	sun.name = "SunLight"
	sun.rotation_degrees = Vector3(-32.0, -42.0, 0.0)
	sun.light_energy = 1.6
	sun.light_color = Color(1.0, 0.92, 0.78)
	sun.shadow_enabled = true
	sun.shadow_opacity = 0.72
	sun.directional_shadow_max_distance = 80.0
	sun.directional_shadow_mode = DirectionalLight3D.SHADOW_PARALLEL_4_SPLITS
	add_child(sun)

	# Soft fill light from the sea side
	var fill_light := DirectionalLight3D.new()
	fill_light.name = "FillLight"
	fill_light.rotation_degrees = Vector3(-16.0, 140.0, 0.0)
	fill_light.light_energy = 0.35
	fill_light.light_color = Color(0.72, 0.82, 0.95)
	fill_light.shadow_enabled = false
	add_child(fill_light)

	# Environment with procedural sky
	var world_environment := WorldEnvironment.new()
	world_environment.environment = _create_environment_resource()
	add_child(world_environment)


func _create_environment_resource() -> Environment:
	var env := Environment.new()

	# Procedural sky
	var sky_material := ProceduralSkyMaterial.new()
	sky_material.sky_top_color = Color(0.32, 0.48, 0.72)
	sky_material.sky_horizon_color = Color(0.78, 0.72, 0.58)
	sky_material.sky_curve = 0.12
	sky_material.sky_energy_multiplier = 1.0
	sky_material.ground_bottom_color = Color(0.42, 0.36, 0.28)
	sky_material.ground_horizon_color = Color(0.82, 0.74, 0.58)
	sky_material.ground_curve = 0.08
	sky_material.sun_angle_max = 38.0
	sky_material.sun_curve = 0.14

	var sky := Sky.new()
	sky.sky_material = sky_material
	env.sky = sky
	env.background_mode = Environment.BG_SKY

	# Ambient light from sky
	env.ambient_light_source = Environment.AMBIENT_SOURCE_SKY
	env.ambient_light_energy = 0.6

	# Tonemap for cinematic look
	env.tonemap_mode = Environment.TONE_MAP_ACES
	env.tonemap_white = 6.0

	# Fog for depth and atmosphere
	env.fog_enabled = true
	env.fog_light_color = Color(0.84, 0.76, 0.62)
	env.fog_light_energy = 0.85
	env.fog_density = 0.006
	env.fog_sky_affect = 0.45

	# Glow for bloom
	env.glow_enabled = true
	env.glow_intensity = 0.4
	env.glow_strength = 0.8
	env.glow_bloom = 0.15
	env.glow_blend_mode = Environment.GLOW_BLEND_MODE_SOFTLIGHT

	# SSAO for contact shadows
	env.ssao_enabled = true
	env.ssao_radius = 2.5
	env.ssao_intensity = 1.5

	# SSR for subtle reflections
	env.ssr_enabled = true
	env.ssr_max_steps = 32
	env.ssr_fade_in = 0.15

	return env


func _build_terrain() -> void:
	var ground := StaticBody3D.new()
	ground.name = "GalileeGround"
	add_child(ground)

	var ground_shape := CollisionShape3D.new()
	var ground_box := BoxShape3D.new()
	ground_box.size = Vector3(160.0, 1.0, 160.0)
	ground_shape.shape = ground_box
	ground_shape.position = Vector3(0.0, -0.5, 0.0)
	ground.add_child(ground_shape)

	# Main ground plane - sandy Galilee soil
	var ground_mesh := MeshInstance3D.new()
	var ground_visual := BoxMesh.new()
	ground_visual.size = Vector3(160.0, 1.0, 160.0)
	ground_mesh.mesh = ground_visual
	ground_mesh.position = Vector3(0.0, -0.5, 0.0)

	var ground_mat := StandardMaterial3D.new()
	ground_mat.albedo_color = Color(0.68, 0.58, 0.42)
	ground_mat.roughness = 0.95
	ground_mat.metallic = 0.0
	ground_mat.normal_enabled = false
	ground_mesh.material_override = ground_mat
	ground.add_child(ground_mesh)

	# Sandy overlay near shore (slightly different color)
	var sand_strip := MeshInstance3D.new()
	var sand_mesh := BoxMesh.new()
	sand_mesh.size = Vector3(120.0, 0.03, 18.0)
	sand_strip.mesh = sand_mesh
	sand_strip.position = Vector3(0.0, 0.01, -33.0)
	var sand_mat := StandardMaterial3D.new()
	sand_mat.albedo_color = Color(0.78, 0.72, 0.56)
	sand_mat.roughness = 0.92
	sand_strip.material_override = sand_mat
	add_child(sand_strip)

	# Darker soil patch near the main area
	var soil_patch := MeshInstance3D.new()
	var soil_mesh := BoxMesh.new()
	soil_mesh.size = Vector3(50.0, 0.02, 40.0)
	soil_patch.mesh = soil_mesh
	soil_patch.position = Vector3(0.0, 0.01, 0.0)
	var soil_mat := StandardMaterial3D.new()
	soil_mat.albedo_color = Color(0.56, 0.48, 0.35)
	soil_mat.roughness = 0.96
	soil_patch.material_override = soil_mat
	add_child(soil_patch)


func _build_sea_of_galilee() -> void:
	# Large sea body with translucent animated-look material
	var sea := MeshInstance3D.new()
	sea.name = "SeaOfGalilee"
	var sea_mesh := PlaneMesh.new()
	sea_mesh.size = Vector2(200.0, 100.0)
	sea.mesh = sea_mesh
	sea.position = Vector3(0.0, -0.18, -85.0)

	var water_mat := StandardMaterial3D.new()
	water_mat.albedo_color = Color(0.14, 0.32, 0.44, 0.82)
	water_mat.roughness = 0.12
	water_mat.metallic = 0.35
	water_mat.transparency = BaseMaterial3D.TRANSPARENCY_ALPHA
	water_mat.cull_mode = BaseMaterial3D.CULL_DISABLED
	water_mat.emission_enabled = true
	water_mat.emission = Color(0.08, 0.16, 0.22)
	water_mat.emission_energy_multiplier = 0.15
	sea.material_override = water_mat
	add_child(sea)

	# Underwater floor visible through water
	var sea_floor := MeshInstance3D.new()
	var floor_mesh := PlaneMesh.new()
	floor_mesh.size = Vector2(200.0, 100.0)
	sea_floor.mesh = floor_mesh
	sea_floor.position = Vector3(0.0, -2.5, -85.0)
	var floor_mat := StandardMaterial3D.new()
	floor_mat.albedo_color = Color(0.24, 0.28, 0.22)
	floor_mat.roughness = 0.98
	sea_floor.material_override = floor_mat
	add_child(sea_floor)


func _build_shoreline() -> void:
	# Pebbled shoreline strip
	for i in range(24):
		var pebble := MeshInstance3D.new()
		var pebble_mesh := SphereMesh.new()
		var pebble_size := randf_range(0.08, 0.22)
		pebble_mesh.radius = pebble_size
		pebble_mesh.height = pebble_size * 1.3
		pebble.mesh = pebble_mesh
		var x_pos := randf_range(-45.0, 45.0)
		var z_pos := randf_range(-36.0, -33.0)
		pebble.position = Vector3(x_pos, pebble_size * 0.4, z_pos)
		var shade := randf_range(0.4, 0.65)
		pebble.material_override = _make_material(Color(shade, shade * 0.92, shade * 0.82))
		add_child(pebble)


func _scatter_rocks() -> void:
	# Large landmark rocks with organic shapes
	_place_rock(Vector3(14.0, 0.0, 8.0), Vector3(3.5, 2.2, 3.0), 0.0)
	_place_rock(Vector3(-11.0, 0.0, -7.0), Vector3(3.0, 2.6, 2.5), 22.0)
	_place_rock(Vector3(8.0, 0.0, -15.0), Vector3(4.5, 1.6, 3.2), -15.0)
	_place_rock(Vector3(26.0, 0.0, -28.0), Vector3(4.8, 1.4, 4.5), 35.0)
	_place_rock(Vector3(-28.0, 0.0, -30.0), Vector3(3.8, 2.0, 3.4), -28.0)

	# Scattered smaller rocks for detail
	_place_small_rock(Vector3(6.0, 0.0, 4.0))
	_place_small_rock(Vector3(-4.0, 0.0, 9.0))
	_place_small_rock(Vector3(22.0, 0.0, -5.0))
	_place_small_rock(Vector3(-20.0, 0.0, 3.0))
	_place_small_rock(Vector3(12.0, 0.0, -22.0))
	_place_small_rock(Vector3(-15.0, 0.0, -25.0))
	_place_small_rock(Vector3(30.0, 0.0, 5.0))
	_place_small_rock(Vector3(-32.0, 0.0, -12.0))
	_place_small_rock(Vector3(3.0, 0.0, -28.0))
	_place_small_rock(Vector3(-8.0, 0.0, 16.0))


func _scatter_trees() -> void:
	# Olive trees around the region
	_place_tree(Vector3(-16.0, 0.0, 12.0), 1.0)
	_place_tree(Vector3(19.0, 0.0, -8.0), 0.85)
	_place_tree(Vector3(-24.0, 0.0, -18.0), 1.1)
	_place_tree(Vector3(27.0, 0.0, 15.0), 0.95)
	_place_tree(Vector3(-35.0, 0.0, 5.0), 1.15)
	_place_tree(Vector3(35.0, 0.0, -3.0), 0.9)
	_place_tree(Vector3(-8.0, 0.0, 20.0), 0.8)
	_place_tree(Vector3(10.0, 0.0, 22.0), 1.05)
	_place_tree(Vector3(-30.0, 0.0, -28.0), 0.75)
	_place_tree(Vector3(32.0, 0.0, -20.0), 0.92)


func _scatter_bushes() -> void:
	# Low scrub bushes across the terrain
	var bush_positions := [
		Vector3(-5.0, 0.0, 6.0), Vector3(8.0, 0.0, 14.0),
		Vector3(-18.0, 0.0, -3.0), Vector3(23.0, 0.0, 4.0),
		Vector3(-12.0, 0.0, -14.0), Vector3(4.0, 0.0, -10.0),
		Vector3(16.0, 0.0, -20.0), Vector3(-25.0, 0.0, 10.0),
		Vector3(28.0, 0.0, -12.0), Vector3(-3.0, 0.0, -18.0),
		Vector3(11.0, 0.0, 7.0), Vector3(-22.0, 0.0, 16.0),
		Vector3(38.0, 0.0, 8.0), Vector3(-38.0, 0.0, -8.0),
		Vector3(0.0, 0.0, -26.0), Vector3(-14.0, 0.0, 22.0),
	]
	for pos in bush_positions:
		_place_bush(pos)


func _build_stone_path() -> void:
	# Worn stone path leading toward the boss arena
	for i in range(20):
		var stone := MeshInstance3D.new()
		var stone_mesh := BoxMesh.new()
		var stone_radius := randf_range(0.35, 0.6)
		stone_mesh.size = Vector3(stone_radius * 1.9, 0.06, stone_radius * 1.7)
		stone.mesh = stone_mesh
		var t := float(i) / 19.0
		var z_pos := lerp(4.0, -30.0, t)
		var x_offset := sin(t * 3.0) * 1.2 + randf_range(-0.3, 0.3)
		stone.position = Vector3(x_offset, 0.02, z_pos)
		stone.rotation_degrees.y = randf_range(0.0, 360.0)
		var shade := randf_range(0.42, 0.52)
		stone.material_override = _make_material(Color(shade, shade * 0.95, shade * 0.88))
		add_child(stone)


func _build_ruins() -> void:
	# Broken Roman column near the path
	_place_broken_column(Vector3(5.0, 0.0, -8.0), 2.2)
	_place_broken_column(Vector3(-6.0, 0.0, -12.0), 1.6)
	_place_broken_column(Vector3(3.0, 0.0, -16.0), 0.8)

	# Fallen column section
	var fallen := MeshInstance3D.new()
	var fallen_mesh := CapsuleMesh.new()
	fallen_mesh.radius = 0.34
	fallen_mesh.mid_height = 2.1
	fallen.mesh = fallen_mesh
	fallen.position = Vector3(-4.0, 0.22, -10.0)
	fallen.rotation_degrees.z = 86.0
	fallen.rotation_degrees.y = 35.0
	var column_mat := StandardMaterial3D.new()
	column_mat.albedo_color = Color(0.72, 0.68, 0.6)
	column_mat.roughness = 0.82
	fallen.material_override = column_mat
	add_child(fallen)


func _build_boss_arena() -> void:
	var arena_center := Vector3(0.0, 0.0, -34.0)

	# Raised stone floor
	var arena_floor := MeshInstance3D.new()
	var floor_disc := SphereMesh.new()
	floor_disc.radius = 10.2
	floor_disc.height = 0.24
	arena_floor.mesh = floor_disc
	arena_floor.scale = Vector3(1.0, 0.1, 1.0)
	arena_floor.position = arena_center + Vector3(0.0, 0.1, 0.0)
	var arena_mat := StandardMaterial3D.new()
	arena_mat.albedo_color = Color(0.38, 0.28, 0.22)
	arena_mat.roughness = 0.88
	arena_mat.metallic = 0.05
	arena_floor.material_override = arena_mat
	add_child(arena_floor)

	# Inner ring pattern
	var inner_ring := MeshInstance3D.new()
	var ring_mesh := TorusMesh.new()
	ring_mesh.ring_radius = 6.0
	ring_mesh.pipe_radius = 0.16
	inner_ring.mesh = ring_mesh
	inner_ring.position = arena_center + Vector3(0.0, 0.23, 0.0)
	var ring_mat := StandardMaterial3D.new()
	ring_mat.albedo_color = Color(0.52, 0.34, 0.24)
	ring_mat.roughness = 0.78
	inner_ring.material_override = ring_mat
	add_child(inner_ring)

	# Pillars with capitals
	for i in range(12):
		var angle := TAU * float(i) / 12.0
		var pillar_pos := arena_center + Vector3(cos(angle) * 10.2, 0.0, sin(angle) * 10.2)

		# Column shaft
		var pillar := MeshInstance3D.new()
		var pillar_mesh := PrismMesh.new()
		pillar_mesh.left_to_right = 0.86
		pillar_mesh.size = Vector3(0.68, 3.2, 0.68)
		pillar.mesh = pillar_mesh
		pillar.position = pillar_pos + Vector3(0.0, 1.6, 0.0)
		var pillar_mat := StandardMaterial3D.new()
		pillar_mat.albedo_color = Color(0.68, 0.62, 0.54)
		pillar_mat.roughness = 0.76
		pillar_mat.metallic = 0.02
		pillar.material_override = pillar_mat
		add_child(pillar)

		# Column capital (top block)
		var capital := MeshInstance3D.new()
		var cap_mesh := BoxMesh.new()
		cap_mesh.size = Vector3(0.76, 0.22, 0.76)
		capital.mesh = cap_mesh
		capital.position = pillar_pos + Vector3(0.0, 3.32, 0.0)
		capital.material_override = pillar_mat
		add_child(capital)

		# Column base
		var col_base := MeshInstance3D.new()
		var base_mesh := BoxMesh.new()
		base_mesh.size = Vector3(1.0, 0.18, 1.0)
		col_base.mesh = base_mesh
		col_base.position = pillar_pos + Vector3(0.0, 0.09, 0.0)
		col_base.material_override = pillar_mat
		add_child(col_base)

	# Fire braziers at cardinal points
	for i in range(4):
		var angle := TAU * float(i) / 4.0 + PI * 0.25
		var brazier_pos := arena_center + Vector3(cos(angle) * 7.8, 0.0, sin(angle) * 7.8)
		_place_brazier(brazier_pos)


# ---------------------------------------------------------------------------
# PROP PLACEMENT HELPERS
# ---------------------------------------------------------------------------

func _place_rock(at: Vector3, base_size: Vector3, y_rotation: float) -> void:
	var rock := StaticBody3D.new()
	rock.name = "Rock"
	add_child(rock)
	rock.position = at + Vector3(0.0, base_size.y * 0.4, 0.0)
	rock.rotation_degrees.y = y_rotation

	var collision := CollisionShape3D.new()
	var shape := BoxShape3D.new()
	shape.size = base_size * 0.85
	collision.shape = shape
	rock.add_child(collision)

	# Main rock body using sphere for organic look
	var mesh_instance := MeshInstance3D.new()
	var mesh := SphereMesh.new()
	mesh.radius = base_size.x * 0.5
	mesh.height = base_size.y
	mesh_instance.mesh = mesh
	mesh_instance.scale = Vector3(1.0, base_size.y / base_size.x, base_size.z / base_size.x)

	var rock_mat := StandardMaterial3D.new()
	var base_shade := randf_range(0.38, 0.52)
	rock_mat.albedo_color = Color(base_shade + 0.06, base_shade, base_shade - 0.04)
	rock_mat.roughness = 0.92
	rock_mat.metallic = 0.0
	mesh_instance.material_override = rock_mat
	rock.add_child(mesh_instance)

	# Moss accent on top
	var moss := MeshInstance3D.new()
	var moss_mesh := SphereMesh.new()
	moss_mesh.radius = base_size.x * 0.35
	moss_mesh.height = base_size.y * 0.3
	moss.mesh = moss_mesh
	moss.position = Vector3(0.0, base_size.y * 0.25, 0.0)
	moss.scale = Vector3(1.2, 0.3, 1.1)
	var moss_mat := StandardMaterial3D.new()
	moss_mat.albedo_color = Color(0.36, 0.42, 0.28, 0.7)
	moss_mat.roughness = 0.98
	moss_mat.transparency = BaseMaterial3D.TRANSPARENCY_ALPHA
	moss.material_override = moss_mat
	rock.add_child(moss)


func _place_small_rock(at: Vector3) -> void:
	var rock := MeshInstance3D.new()
	var mesh := SphereMesh.new()
	var size := randf_range(0.2, 0.55)
	mesh.radius = size
	mesh.height = size * randf_range(0.8, 1.4)
	rock.mesh = mesh
	rock.position = at + Vector3(0.0, size * 0.35, 0.0)
	rock.rotation_degrees.y = randf_range(0.0, 360.0)
	rock.scale = Vector3(randf_range(0.8, 1.3), randf_range(0.6, 1.0), randf_range(0.8, 1.3))
	var shade := randf_range(0.4, 0.58)
	rock.material_override = _make_material(Color(shade + 0.04, shade, shade - 0.03))
	add_child(rock)


func _place_tree(at: Vector3, scale_factor: float) -> void:
	var tree := Node3D.new()
	tree.position = at
	add_child(tree)

	# Gnarled olive tree trunk
	var trunk := MeshInstance3D.new()
	var trunk_mesh := PrismMesh.new()
	trunk_mesh.left_to_right = 0.78
	trunk_mesh.size = Vector3(0.62 * scale_factor, 3.2 * scale_factor, 0.56 * scale_factor)
	trunk.mesh = trunk_mesh
	trunk.position = Vector3(0.0, trunk_mesh.height * 0.5, 0.0)

	var trunk_mat := StandardMaterial3D.new()
	trunk_mat.albedo_color = Color(0.38, 0.28, 0.16)
	trunk_mat.roughness = 0.94
	trunk.material_override = trunk_mat
	tree.add_child(trunk)

	# Main canopy
	var canopy := MeshInstance3D.new()
	var canopy_mesh := SphereMesh.new()
	canopy_mesh.radius = 1.6 * scale_factor
	canopy_mesh.height = 1.8 * scale_factor
	canopy.mesh = canopy_mesh
	canopy.position = Vector3(0.0, (3.2 + 0.6) * scale_factor, 0.0)
	canopy.scale = Vector3(1.3, 0.8, 1.1)

	var leaf_mat := StandardMaterial3D.new()
	leaf_mat.albedo_color = Color(0.32, 0.46, 0.22)
	leaf_mat.roughness = 0.88
	leaf_mat.metallic = 0.0
	canopy.material_override = leaf_mat
	tree.add_child(canopy)

	# Secondary canopy cluster offset
	var canopy2 := MeshInstance3D.new()
	var canopy2_mesh := SphereMesh.new()
	canopy2_mesh.radius = 1.1 * scale_factor
	canopy2_mesh.height = 1.3 * scale_factor
	canopy2.mesh = canopy2_mesh
	canopy2.position = Vector3(0.6 * scale_factor, (2.8 + 0.4) * scale_factor, 0.4 * scale_factor)
	canopy2.scale = Vector3(1.1, 0.75, 1.0)

	var leaf_mat2 := StandardMaterial3D.new()
	leaf_mat2.albedo_color = Color(0.28, 0.42, 0.2)
	leaf_mat2.roughness = 0.9
	canopy2.material_override = leaf_mat2
	tree.add_child(canopy2)

	# Third smaller cluster
	var canopy3 := MeshInstance3D.new()
	var canopy3_mesh := SphereMesh.new()
	canopy3_mesh.radius = 0.8 * scale_factor
	canopy3_mesh.height = 1.0 * scale_factor
	canopy3.mesh = canopy3_mesh
	canopy3.position = Vector3(-0.5 * scale_factor, (3.0 + 0.2) * scale_factor, -0.3 * scale_factor)
	canopy3.scale = Vector3(1.0, 0.7, 1.2)
	canopy3.material_override = leaf_mat
	tree.add_child(canopy3)

	# Shadow blob on ground
	var shadow := MeshInstance3D.new()
	var shadow_mesh := SphereMesh.new()
	shadow_mesh.radius = 1.9 * scale_factor
	shadow_mesh.height = 0.08
	shadow.mesh = shadow_mesh
	shadow.position = Vector3(0.0, 0.01, 0.0)
	var shadow_mat := StandardMaterial3D.new()
	shadow_mat.albedo_color = Color(0.32, 0.3, 0.24, 0.3)
	shadow_mat.roughness = 1.0
	shadow_mat.transparency = BaseMaterial3D.TRANSPARENCY_ALPHA
	shadow.material_override = shadow_mat
	tree.add_child(shadow)


func _place_bush(at: Vector3) -> void:
	var bush := MeshInstance3D.new()
	var mesh := SphereMesh.new()
	var bush_size := randf_range(0.3, 0.7)
	mesh.radius = bush_size
	mesh.height = bush_size * 1.2
	bush.mesh = mesh
	bush.position = at + Vector3(0.0, bush_size * 0.35, 0.0)
	bush.scale = Vector3(randf_range(1.0, 1.6), randf_range(0.5, 0.8), randf_range(1.0, 1.5))

	var bush_mat := StandardMaterial3D.new()
	var green_var := randf_range(0.0, 0.08)
	bush_mat.albedo_color = Color(0.34 + green_var, 0.46 + green_var, 0.24 + green_var)
	bush_mat.roughness = 0.92
	bush.material_override = bush_mat
	add_child(bush)


func _place_broken_column(at: Vector3, height: float) -> void:
	var column := MeshInstance3D.new()
	var mesh := PrismMesh.new()
	mesh.left_to_right = 0.84
	mesh.size = Vector3(0.62, height, 0.62)
	column.mesh = mesh
	column.position = at + Vector3(0.0, height * 0.5, 0.0)

	var col_mat := StandardMaterial3D.new()
	col_mat.albedo_color = Color(0.72, 0.68, 0.6)
	col_mat.roughness = 0.82
	col_mat.metallic = 0.02
	column.material_override = col_mat
	add_child(column)

	# Broken top cap
	var cap := MeshInstance3D.new()
	var cap_mesh := BoxMesh.new()
	cap_mesh.size = Vector3(0.58, 0.12, 0.58)
	cap.mesh = cap_mesh
	cap.position = at + Vector3(0.0, height + 0.06, 0.0)
	cap.rotation_degrees = Vector3(randf_range(-8.0, 8.0), 0.0, randf_range(-8.0, 8.0))
	cap.material_override = col_mat
	add_child(cap)


func _place_brazier(at: Vector3) -> void:
	# Stone bowl
	var bowl := MeshInstance3D.new()
	var bowl_mesh := SphereMesh.new()
	bowl_mesh.radius = 0.42
	bowl_mesh.height = 0.3
	bowl.mesh = bowl_mesh
	bowl.position = at + Vector3(0.0, 0.32, 0.0)
	var bowl_mat := StandardMaterial3D.new()
	bowl_mat.albedo_color = Color(0.42, 0.36, 0.3)
	bowl_mat.roughness = 0.85
	bowl.material_override = bowl_mat
	add_child(bowl)

	# Stand
	var stand := MeshInstance3D.new()
	var stand_mesh := PrismMesh.new()
	stand_mesh.left_to_right = 0.84
	stand_mesh.size = Vector3(0.32, 0.65, 0.32)
	stand.mesh = stand_mesh
	stand.position = at + Vector3(0.0, 0.0, 0.0)
	stand.material_override = bowl_mat
	add_child(stand)

	# Fire glow
	var fire_light := OmniLight3D.new()
	fire_light.position = at + Vector3(0.0, 1.0, 0.0)
	fire_light.light_color = Color(1.0, 0.72, 0.32)
	fire_light.light_energy = 2.2
	fire_light.omni_range = 6.5
	fire_light.shadow_enabled = true
	add_child(fire_light)

	# Flame mesh
	var flame := MeshInstance3D.new()
	var flame_mesh := SphereMesh.new()
	flame_mesh.radius = 0.18
	flame_mesh.height = 0.45
	flame.mesh = flame_mesh
	flame.position = at + Vector3(0.0, 0.75, 0.0)
	var flame_mat := StandardMaterial3D.new()
	flame_mat.albedo_color = Color(1.0, 0.7, 0.2)
	flame_mat.emission_enabled = true
	flame_mat.emission = Color(1.0, 0.55, 0.12)
	flame_mat.emission_energy_multiplier = 3.0
	flame_mat.roughness = 0.1
	flame_mat.transparency = BaseMaterial3D.TRANSPARENCY_ALPHA
	flame_mat.albedo_color.a = 0.85
	flame.material_override = flame_mat
	add_child(flame)


func _make_material(color: Color) -> StandardMaterial3D:
	var material := StandardMaterial3D.new()
	material.albedo_color = color
	material.roughness = 0.9
	return material


# ---------------------------------------------------------------------------
# ENTITY SPAWNING
# ---------------------------------------------------------------------------

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


func _spawn_music() -> void:
	_music_player = AudioStreamPlayer.new()
	_music_player.name = "EpicBiblicalTheme"
	var stream := load("res://assets/music/epic_biblical_theme.wav") as AudioStreamWAV
	if stream:
		stream.loop_mode = AudioStreamWAV.LOOP_FORWARD
		stream.loop_begin = 0
		stream.loop_end = int(stream.get_length() * stream.mix_rate)
		_music_player.stream = stream
	else:
		_music_player.stream = load("res://assets/music/epic_biblical_theme.wav")
	_music_player.autoplay = true
	_music_player.volume_db = -6.0
	add_child(_music_player)
