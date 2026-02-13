extends Node3D
class_name CampaignManager


const SaveSystemScript := preload("res://scripts/persistence/save_system.gd")


const REGIONS := [
	{
		"id": "galilee",
		"name": "Galilee Shores",
		"chapter": 1,
		"objective": "Redeem the Legion Sovereign of Gerasa.",
		"player_spawn": Vector3(0.0, 1.2, 12.0),
		"prayer_site": {
			"id": "galilee_site_01",
			"name": "Prayer Site: Galilee Shores",
			"pos": Vector3(0.0, 0.15, 2.0),
		},
		"travel_gate": {
			"pos": Vector3(0.0, 0.2, 32.0),
			"label": "Press E to travel to Decapolis Ruins",
		},
		"environment": {
			"ground_size": Vector3(120.0, 1.0, 120.0),
			"ground_color": Color(0.62, 0.55, 0.42),
			"sky_color": Color(0.76, 0.78, 0.74),
			"ambient_color": Color(0.64, 0.58, 0.49),
			"fog_color": Color(0.81, 0.73, 0.62),
			"fog_density": 0.015,
			"sun_rotation": Vector3(-46.0, -38.0, 0.0),
			"sun_energy": 1.35,
		},
		"sea": {
			"pos": Vector3(0.0, 0.1, -42.0),
			"size": Vector2(100.0, 28.0),
			"color": Color(0.24, 0.39, 0.48),
		},
		"props": {
			"rocks": [
				{"pos": Vector3(14.0, 0.8, 8.0), "size": Vector3(3.0, 1.6, 2.5)},
				{"pos": Vector3(-11.0, 1.0, -7.0), "size": Vector3(2.8, 2.0, 2.2)},
				{"pos": Vector3(8.0, 0.6, -15.0), "size": Vector3(4.2, 1.2, 2.0)},
				{"pos": Vector3(26.0, 0.6, -28.0), "size": Vector3(4.4, 1.1, 4.2)},
				{"pos": Vector3(-28.0, 0.9, -30.0), "size": Vector3(3.6, 1.6, 3.1)},
			],
			"trees": [
				Vector3(-16.0, 0.0, 12.0),
				Vector3(19.0, 0.0, -8.0),
				Vector3(-24.0, 0.0, -18.0),
				Vector3(27.0, 0.0, 15.0),
			],
		},
		"arena": {
			"center": Vector3(0.0, 0.02, -34.0),
			"radius": 9.0,
			"color": Color(0.33, 0.24, 0.21),
		},
		"enemies": [
			{"id": "galilee_shield_01", "name": "Roman Shieldbearer I", "type": EnemyAI.EnemyType.MELEE_SHIELD, "pos": Vector3(9.0, 1.0, -3.0)},
			{"id": "galilee_shield_02", "name": "Roman Shieldbearer II", "type": EnemyAI.EnemyType.MELEE_SHIELD, "pos": Vector3(-9.0, 1.0, -5.0)},
			{"id": "galilee_spear_01", "name": "Roman Spearman I", "type": EnemyAI.EnemyType.SPEAR, "pos": Vector3(15.0, 1.0, 11.0)},
			{"id": "galilee_spear_02", "name": "Roman Spearman II", "type": EnemyAI.EnemyType.SPEAR, "pos": Vector3(-14.0, 1.0, 12.0)},
			{"id": "galilee_ranged_01", "name": "Roman Slinger I", "type": EnemyAI.EnemyType.RANGED, "pos": Vector3(18.0, 1.0, -14.0)},
			{"id": "galilee_ranged_02", "name": "Roman Slinger II", "type": EnemyAI.EnemyType.RANGED, "pos": Vector3(-18.0, 1.0, -15.0)},
			{"id": "galilee_demon_01", "name": "Unclean Spirit I", "type": EnemyAI.EnemyType.DEMON, "pos": Vector3(5.0, 1.0, -20.0)},
			{"id": "galilee_demon_02", "name": "Unclean Spirit II", "type": EnemyAI.EnemyType.DEMON, "pos": Vector3(-6.0, 1.0, -22.0)},
			{"id": "galilee_named_boss_01", "name": "Legion Sovereign of Gerasa", "type": EnemyAI.EnemyType.BOSS, "pos": Vector3(0.0, 1.0, -34.0)},
		],
		"boss_id": "galilee_named_boss_01",
		"reward": {"miracle": "blessing", "health": 10.0, "stamina": 6.0},
	},
	{
		"id": "decapolis",
		"name": "Decapolis Ruins",
		"chapter": 2,
		"objective": "Redeem the Gadarene Warlord among the ruins.",
		"player_spawn": Vector3(0.0, 1.2, 13.0),
		"prayer_site": {
			"id": "decapolis_site_01",
			"name": "Prayer Site: Decapolis Ruins",
			"pos": Vector3(0.0, 0.15, 3.0),
		},
		"travel_gate": {
			"pos": Vector3(0.0, 0.2, 34.0),
			"label": "Press E to travel to Wilderness of Temptation",
		},
		"environment": {
			"ground_size": Vector3(130.0, 1.0, 130.0),
			"ground_color": Color(0.52, 0.49, 0.46),
			"sky_color": Color(0.7, 0.72, 0.75),
			"ambient_color": Color(0.55, 0.52, 0.52),
			"fog_color": Color(0.75, 0.73, 0.7),
			"fog_density": 0.02,
			"sun_rotation": Vector3(-40.0, -20.0, 0.0),
			"sun_energy": 1.25,
		},
		"props": {
			"rocks": [
				{"pos": Vector3(20.0, 0.7, 14.0), "size": Vector3(3.4, 1.4, 3.1)},
				{"pos": Vector3(-20.0, 0.7, 12.0), "size": Vector3(3.1, 1.6, 2.8)},
			],
			"pillars": [
				{"pos": Vector3(10.0, 0.0, -6.0), "height": 2.6},
				{"pos": Vector3(-10.0, 0.0, -6.0), "height": 2.6},
				{"pos": Vector3(14.0, 0.0, -16.0), "height": 3.0},
				{"pos": Vector3(-14.0, 0.0, -16.0), "height": 3.0},
				{"pos": Vector3(0.0, 0.0, -22.0), "height": 3.4},
			],
			"ruins": [
				{"pos": Vector3(6.0, 0.5, -10.0), "size": Vector3(4.8, 1.0, 1.2)},
				{"pos": Vector3(-6.0, 0.5, -12.0), "size": Vector3(4.2, 1.0, 1.2)},
				{"pos": Vector3(0.0, 0.4, -18.0), "size": Vector3(7.2, 0.9, 1.5)},
			],
		},
		"arena": {
			"center": Vector3(0.0, 0.02, -30.0),
			"radius": 9.5,
			"color": Color(0.32, 0.3, 0.27),
		},
		"enemies": [
			{"id": "decapolis_shield_01", "name": "Roman Shieldbearer", "type": EnemyAI.EnemyType.MELEE_SHIELD, "pos": Vector3(8.0, 1.0, -4.0)},
			{"id": "decapolis_shield_02", "name": "Roman Shieldbearer", "type": EnemyAI.EnemyType.MELEE_SHIELD, "pos": Vector3(-9.0, 1.0, -6.0)},
			{"id": "decapolis_spear_01", "name": "Roman Spearman", "type": EnemyAI.EnemyType.SPEAR, "pos": Vector3(12.0, 1.0, 9.0)},
			{"id": "decapolis_spear_02", "name": "Roman Spearman", "type": EnemyAI.EnemyType.SPEAR, "pos": Vector3(-12.0, 1.0, 10.0)},
			{"id": "decapolis_ranged_01", "name": "Roman Slinger", "type": EnemyAI.EnemyType.RANGED, "pos": Vector3(16.0, 1.0, -10.0)},
			{"id": "decapolis_ranged_02", "name": "Roman Slinger", "type": EnemyAI.EnemyType.RANGED, "pos": Vector3(-16.0, 1.0, -12.0)},
			{"id": "decapolis_demon_01", "name": "Unclean Spirit", "type": EnemyAI.EnemyType.DEMON, "pos": Vector3(6.0, 1.0, -18.0)},
			{"id": "decapolis_demon_02", "name": "Unclean Spirit", "type": EnemyAI.EnemyType.DEMON, "pos": Vector3(-5.0, 1.0, -20.0)},
			{"id": "decapolis_demon_03", "name": "Unclean Spirit", "type": EnemyAI.EnemyType.DEMON, "pos": Vector3(3.0, 1.0, -24.0)},
			{"id": "decapolis_named_boss_01", "name": "Gadarene Warlord", "type": EnemyAI.EnemyType.BOSS, "pos": Vector3(0.0, 1.0, -30.0)},
		],
		"boss_id": "decapolis_named_boss_01",
		"reward": {"miracle": "radiance", "health": 10.0, "stamina": 6.0},
	},
	{
		"id": "wilderness",
		"name": "Wilderness of Temptation",
		"chapter": 3,
		"objective": "Resist the Adversary of the Desert.",
		"player_spawn": Vector3(0.0, 1.2, 14.0),
		"prayer_site": {
			"id": "wilderness_site_01",
			"name": "Prayer Site: Wilderness Ridge",
			"pos": Vector3(0.0, 0.15, 4.0),
		},
		"travel_gate": {
			"pos": Vector3(0.0, 0.2, 36.0),
			"label": "Press E to travel to Jerusalem Approach",
		},
		"environment": {
			"ground_size": Vector3(150.0, 1.0, 150.0),
			"ground_color": Color(0.72, 0.64, 0.45),
			"sky_color": Color(0.8, 0.77, 0.67),
			"ambient_color": Color(0.68, 0.62, 0.52),
			"fog_color": Color(0.86, 0.8, 0.7),
			"fog_density": 0.012,
			"sun_rotation": Vector3(-55.0, -10.0, 0.0),
			"sun_energy": 1.4,
		},
		"props": {
			"rocks": [
				{"pos": Vector3(22.0, 0.8, 16.0), "size": Vector3(4.8, 1.6, 3.6)},
				{"pos": Vector3(-24.0, 0.8, 14.0), "size": Vector3(4.2, 1.4, 3.4)},
			],
			"spires": [
				{"pos": Vector3(16.0, 0.0, -6.0), "height": 6.2, "radius": 0.9},
				{"pos": Vector3(-18.0, 0.0, -8.0), "height": 5.6, "radius": 0.85},
				{"pos": Vector3(8.0, 0.0, -22.0), "height": 7.1, "radius": 1.0},
				{"pos": Vector3(-10.0, 0.0, -24.0), "height": 6.8, "radius": 0.95},
			],
		},
		"arena": {
			"center": Vector3(0.0, 0.02, -32.0),
			"radius": 10.5,
			"color": Color(0.41, 0.33, 0.24),
		},
		"enemies": [
			{"id": "wilderness_ranged_01", "name": "Desert Slinger", "type": EnemyAI.EnemyType.RANGED, "pos": Vector3(18.0, 1.0, -10.0)},
			{"id": "wilderness_ranged_02", "name": "Desert Slinger", "type": EnemyAI.EnemyType.RANGED, "pos": Vector3(-18.0, 1.0, -12.0)},
			{"id": "wilderness_spear_01", "name": "Desert Spearman", "type": EnemyAI.EnemyType.SPEAR, "pos": Vector3(12.0, 1.0, 10.0)},
			{"id": "wilderness_spear_02", "name": "Desert Spearman", "type": EnemyAI.EnemyType.SPEAR, "pos": Vector3(-12.0, 1.0, 12.0)},
			{"id": "wilderness_demon_01", "name": "Tempter Spirit", "type": EnemyAI.EnemyType.DEMON, "pos": Vector3(6.0, 1.0, -18.0)},
			{"id": "wilderness_demon_02", "name": "Tempter Spirit", "type": EnemyAI.EnemyType.DEMON, "pos": Vector3(-6.0, 1.0, -19.0)},
			{"id": "wilderness_demon_03", "name": "Tempter Spirit", "type": EnemyAI.EnemyType.DEMON, "pos": Vector3(4.0, 1.0, -26.0)},
			{"id": "wilderness_demon_04", "name": "Tempter Spirit", "type": EnemyAI.EnemyType.DEMON, "pos": Vector3(-4.0, 1.0, -27.0)},
			{"id": "wilderness_named_boss_01", "name": "Adversary of the Desert", "type": EnemyAI.EnemyType.BOSS, "pos": Vector3(0.0, 1.0, -32.0)},
		],
		"boss_id": "wilderness_named_boss_01",
		"reward": {"health": 12.0, "stamina": 8.0},
	},
	{
		"id": "jerusalem",
		"name": "Jerusalem Approach",
		"chapter": 4,
		"objective": "Face the Temple Warden at the city's gate.",
		"player_spawn": Vector3(0.0, 1.2, 12.0),
		"prayer_site": {
			"id": "jerusalem_site_01",
			"name": "Prayer Site: Jerusalem Approach",
			"pos": Vector3(0.0, 0.15, 2.5),
		},
		"travel_gate": {
			"pos": Vector3(0.0, 0.2, 32.0),
			"label": "Press E to conclude the pilgrimage",
		},
		"environment": {
			"ground_size": Vector3(140.0, 1.0, 140.0),
			"ground_color": Color(0.63, 0.6, 0.55),
			"sky_color": Color(0.74, 0.75, 0.78),
			"ambient_color": Color(0.6, 0.58, 0.56),
			"fog_color": Color(0.8, 0.78, 0.76),
			"fog_density": 0.018,
			"sun_rotation": Vector3(-38.0, -30.0, 0.0),
			"sun_energy": 1.3,
		},
		"props": {
			"pillars": [
				{"pos": Vector3(8.0, 0.0, -4.0), "height": 3.6},
				{"pos": Vector3(-8.0, 0.0, -4.0), "height": 3.6},
				{"pos": Vector3(12.0, 0.0, -12.0), "height": 3.6},
				{"pos": Vector3(-12.0, 0.0, -12.0), "height": 3.6},
				{"pos": Vector3(16.0, 0.0, -20.0), "height": 3.6},
				{"pos": Vector3(-16.0, 0.0, -20.0), "height": 3.6},
			],
			"ruins": [
				{"pos": Vector3(0.0, 0.4, -8.0), "size": Vector3(8.0, 0.9, 2.0)},
				{"pos": Vector3(0.0, 0.4, -16.0), "size": Vector3(8.5, 0.9, 2.0)},
			],
		},
		"arena": {
			"center": Vector3(0.0, 0.02, -28.0),
			"radius": 10.0,
			"color": Color(0.41, 0.38, 0.34),
		},
		"enemies": [
			{"id": "jerusalem_shield_01", "name": "Temple Shieldbearer", "type": EnemyAI.EnemyType.MELEE_SHIELD, "pos": Vector3(8.0, 1.0, -4.0)},
			{"id": "jerusalem_shield_02", "name": "Temple Shieldbearer", "type": EnemyAI.EnemyType.MELEE_SHIELD, "pos": Vector3(-8.0, 1.0, -6.0)},
			{"id": "jerusalem_spear_01", "name": "Temple Spearman", "type": EnemyAI.EnemyType.SPEAR, "pos": Vector3(12.0, 1.0, 8.0)},
			{"id": "jerusalem_spear_02", "name": "Temple Spearman", "type": EnemyAI.EnemyType.SPEAR, "pos": Vector3(-12.0, 1.0, 10.0)},
			{"id": "jerusalem_ranged_01", "name": "Temple Slinger", "type": EnemyAI.EnemyType.RANGED, "pos": Vector3(16.0, 1.0, -12.0)},
			{"id": "jerusalem_ranged_02", "name": "Temple Slinger", "type": EnemyAI.EnemyType.RANGED, "pos": Vector3(-16.0, 1.0, -14.0)},
			{"id": "jerusalem_demon_01", "name": "Unclean Spirit", "type": EnemyAI.EnemyType.DEMON, "pos": Vector3(5.0, 1.0, -20.0)},
			{"id": "jerusalem_demon_02", "name": "Unclean Spirit", "type": EnemyAI.EnemyType.DEMON, "pos": Vector3(-5.0, 1.0, -21.0)},
			{"id": "jerusalem_named_boss_01", "name": "Temple Warden", "type": EnemyAI.EnemyType.BOSS, "pos": Vector3(0.0, 1.0, -28.0)},
		],
		"boss_id": "jerusalem_named_boss_01",
		"reward": {},
	},
]


var _player: PlayerController
var _hud: HUDLayer
var _pause_menu: PauseMenu
var _travel_gate: TravelGate
var _region_root: Node3D
var _boss_instance: EnemyAI
var _region_index: int = 0
var _region_completed: bool = false
var _pending_payload: Dictionary = {}
var _session: GameSession
var _suppress_redeem_events: bool = false


func _ready() -> void:
	pause_mode = Node.PAUSE_MODE_PROCESS
	add_to_group("campaign")
	_ensure_input_map()
	_session = _get_session()

	if _session != null:
		var pending := _session.take_pending_payload()
		if not pending.is_empty():
			var campaign_data := pending.get("campaign", {})
			if campaign_data is Dictionary:
				_session.apply_campaign_payload(campaign_data)
				_region_index = clampi(int(campaign_data.get("region_index", _session.region_index)), 0, REGIONS.size() - 1)
			_pending_payload = pending
		else:
			_region_index = clampi(_session.region_index, 0, REGIONS.size() - 1)

	_load_region(_region_index)


func _unhandled_input(event: InputEvent) -> void:
	if event.is_action_pressed("pause"):
		_toggle_pause()


func _toggle_pause() -> void:
	if get_tree().paused:
		_resume_game()
	else:
		_pause_game()


func _pause_game() -> void:
	get_tree().paused = true
	if _pause_menu != null:
		_pause_menu.show_menu()
	Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)


func _resume_game() -> void:
	if _pause_menu != null:
		_pause_menu.hide_menu()
	get_tree().paused = false
	Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)


func _return_to_menu() -> void:
	get_tree().paused = false
	get_tree().change_scene_to_file("res://scenes/MainMenu.tscn")


func _load_region(index: int) -> void:
	_clear_region()
	_region_index = clampi(index, 0, REGIONS.size() - 1)
	if _session != null:
		_session.region_index = _region_index

	var region := REGIONS[_region_index]
	_region_root = Node3D.new()
	_region_root.name = "RegionRoot"
	add_child(_region_root)

	_build_environment(region)
	_spawn_player(region)
	_spawn_prayer_site(region)
	_spawn_enemies(region)
	_spawn_travel_gate(region)
	_spawn_hud(region)
	_spawn_pause_menu()

	_region_completed = false
	_update_region_ui(region)
	Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)

	if not _pending_payload.is_empty():
		_apply_save_payload(_pending_payload)
		_pending_payload = {}

	_save_checkpoint()


func _clear_region() -> void:
	if _region_root != null:
		_region_root.queue_free()
		_region_root = null
		_boss_instance = null
		_travel_gate = null
		_player = null


func _build_environment(region: Dictionary) -> void:
	var environment_data := region.get("environment", {})
	var sun := DirectionalLight3D.new()
	sun.name = "SunLight"
	sun.rotation_degrees = environment_data.get("sun_rotation", Vector3(-46.0, -38.0, 0.0))
	sun.light_energy = float(environment_data.get("sun_energy", 1.3))
	_region_root.add_child(sun)

	var world_environment := WorldEnvironment.new()
	world_environment.environment = _create_environment_resource(environment_data)
	_region_root.add_child(world_environment)

	var ground := StaticBody3D.new()
	ground.name = "RegionGround"
	_region_root.add_child(ground)

	var ground_shape := CollisionShape3D.new()
	var ground_box := BoxShape3D.new()
	var ground_size := environment_data.get("ground_size", Vector3(120.0, 1.0, 120.0))
	ground_box.size = ground_size
	ground_shape.shape = ground_box
	ground_shape.position = Vector3(0.0, -0.5, 0.0)
	ground.add_child(ground_shape)

	var ground_mesh := MeshInstance3D.new()
	var ground_visual := BoxMesh.new()
	ground_visual.size = ground_size
	ground_mesh.mesh = ground_visual
	ground_mesh.position = Vector3(0.0, -0.5, 0.0)
	ground_mesh.material_override = _make_material(environment_data.get("ground_color", Color(0.62, 0.55, 0.42)))
	ground.add_child(ground_mesh)

	var sea_data := region.get("sea", {})
	if sea_data is Dictionary and not sea_data.is_empty():
		var sea := MeshInstance3D.new()
		sea.name = "SeaBackdrop"
		var sea_mesh := PlaneMesh.new()
		sea_mesh.size = sea_data.get("size", Vector2(100.0, 28.0))
		sea.mesh = sea_mesh
		sea.position = sea_data.get("pos", Vector3(0.0, 0.1, -42.0))
		sea.rotation_degrees.x = -90.0
		sea.material_override = _make_material(sea_data.get("color", Color(0.24, 0.39, 0.48)))
		_region_root.add_child(sea)

	var props := region.get("props", {})
	if props is Dictionary:
		for rock in props.get("rocks", []):
			if rock is Dictionary:
				_place_rock(rock.get("pos", Vector3.ZERO), rock.get("size", Vector3.ONE))
		for tree_pos in props.get("trees", []):
			if tree_pos is Vector3:
				_place_tree(tree_pos)
		for pillar in props.get("pillars", []):
			if pillar is Dictionary:
				_place_pillar(pillar.get("pos", Vector3.ZERO), float(pillar.get("height", 2.6)))
		for ruin in props.get("ruins", []):
			if ruin is Dictionary:
				_place_ruin_block(ruin.get("pos", Vector3.ZERO), ruin.get("size", Vector3(4.0, 1.0, 1.5)))
		for spire in props.get("spires", []):
			if spire is Dictionary:
				_place_spire(spire.get("pos", Vector3.ZERO), float(spire.get("height", 6.0)), float(spire.get("radius", 0.9)))

	var arena_data := region.get("arena", {})
	if arena_data is Dictionary and not arena_data.is_empty():
		_build_boss_arena(arena_data.get("center", Vector3.ZERO), float(arena_data.get("radius", 8.0)), arena_data.get("color", Color(0.33, 0.24, 0.21)))


func _create_environment_resource(environment_data: Dictionary) -> Environment:
	var environment := Environment.new()
	environment.background_mode = Environment.BG_COLOR
	environment.background_color = environment_data.get("sky_color", Color(0.76, 0.78, 0.74))
	environment.ambient_light_color = environment_data.get("ambient_color", Color(0.64, 0.58, 0.49))
	environment.ambient_light_energy = 1.1
	environment.fog_enabled = true
	environment.fog_light_color = environment_data.get("fog_color", Color(0.81, 0.73, 0.62))
	environment.fog_light_energy = 1.0
	environment.fog_density = float(environment_data.get("fog_density", 0.015))
	return environment


func _spawn_player(region: Dictionary) -> void:
	_player = PlayerController.new()
	_player.name = "JesusPlayer"
	_player.global_position = region.get("player_spawn", Vector3(0.0, 1.2, 12.0))
	_region_root.add_child(_player)
	_player.set_campaign_manager(self)

	if _session != null:
		_player.max_health += _session.max_health_bonus
		_player.max_stamina += _session.max_stamina_bonus
		_player.current_health = _player.max_health
		_player.current_stamina = _player.max_stamina
		_player.set_unlocked_miracles(_session.unlocked_miracles)


func _spawn_prayer_site(region: Dictionary) -> void:
	var site_data := region.get("prayer_site", {})
	var prayer_site := PrayerSite.new()
	prayer_site.name = "PrayerSite_%s" % region.get("id", "region")
	prayer_site.site_id = site_data.get("id", "")
	prayer_site.site_name = site_data.get("name", "Prayer Site")
	prayer_site.prompt_text = "Press E to pray and rest"
	prayer_site.global_position = site_data.get("pos", Vector3(0.0, 0.15, 2.0))
	_region_root.add_child(prayer_site)


func _spawn_enemies(region: Dictionary) -> void:
	_boss_instance = null
	for spec in region.get("enemies", []):
		if not (spec is Dictionary):
			continue
		var enemy := EnemyAI.new()
		enemy.spawn_id = str(spec.get("id", ""))
		enemy.enemy_name = str(spec.get("name", "Enemy"))
		enemy.enemy_type = int(spec.get("type", EnemyAI.EnemyType.MELEE_SHIELD))
		enemy.global_position = spec.get("pos", Vector3.ZERO)
		_region_root.add_child(enemy)
		enemy.redeemed.connect(_on_enemy_redeemed.bind(enemy))
		if enemy.spawn_id == str(region.get("boss_id", "")):
			_boss_instance = enemy


func _spawn_travel_gate(region: Dictionary) -> void:
	var gate_data := region.get("travel_gate", {})
	if not (gate_data is Dictionary):
		return
	_travel_gate = TravelGate.new()
	_travel_gate.name = "TravelGate"
	_travel_gate.global_position = gate_data.get("pos", Vector3(0.0, 0.2, 32.0))
	_travel_gate.prompt_text = gate_data.get("label", "Press E to travel onward")
	_travel_gate.target_region_index = _region_index + 1
	_travel_gate.set_gate_enabled(false)
	_travel_gate.travel_requested.connect(_on_travel_requested)
	_region_root.add_child(_travel_gate)


func _spawn_hud(region: Dictionary) -> void:
	if _hud == null:
		_hud = HUDLayer.new()
		add_child(_hud)
	_hud.bind_player(_player)
	_hud.set_region_name("Chapter %d: %s" % [region.get("chapter", 1), region.get("name", "Region")])
	_hud.set_objective(region.get("objective", ""))


func _spawn_pause_menu() -> void:
	if _pause_menu == null:
		_pause_menu = PauseMenu.new()
		add_child(_pause_menu)
		_pause_menu.resume_requested.connect(_resume_game)
		_pause_menu.return_to_menu_requested.connect(_return_to_menu)
	_pause_menu.bind(_player, self)


func save_slot(slot_id: int, player: PlayerController) -> bool:
	if player == null:
		return false
	var payload := player.build_save_payload()
	payload["campaign"] = _build_campaign_payload()
	return SaveSystemScript.save_slot(slot_id, payload)


func load_slot(slot_id: int) -> bool:
	var payload := SaveSystemScript.load_slot(slot_id)
	if payload.is_empty():
		if _player != null:
			_player.set_context_hint("Slot %d is empty." % slot_id)
		return false
	var ok := _apply_loaded_payload(payload)
	if ok and _player != null:
		_player.set_context_hint("Loaded slot %d." % slot_id)
	return ok


func _apply_loaded_payload(payload: Dictionary) -> bool:
	var campaign_data := payload.get("campaign", {})
	var target_index := _region_index
	if campaign_data is Dictionary:
		_apply_campaign_payload(campaign_data)
		target_index = int(campaign_data.get("region_index", _region_index))
	if target_index != _region_index:
		_pending_payload = payload
		_load_region(target_index)
		return true

	_apply_save_payload(payload)
	return true


func _apply_campaign_payload(campaign_data: Dictionary) -> void:
	if _session != null:
		_session.apply_campaign_payload(campaign_data)


func _build_campaign_payload() -> Dictionary:
	if _session != null:
		return _session.build_campaign_payload()
	return {
		"region_index": _region_index,
	}


func _apply_save_payload(payload: Dictionary) -> void:
	if _player == null:
		return
	_suppress_redeem_events = true
	_player.apply_save_payload(payload)
	_suppress_redeem_events = false
	_sync_completion_state()


func _sync_completion_state() -> void:
	if _boss_instance == null:
		return
	if _boss_instance.is_redeemed():
		_region_completed = true
		_enable_travel_gate()
		_update_objective_for_completion(REGIONS[_region_index])
	else:
		_region_completed = false


func _on_enemy_redeemed(_enemy_name: String, _faith_reward: float, enemy: EnemyAI) -> void:
	if _suppress_redeem_events:
		return
	if enemy != _boss_instance:
		return
	_on_boss_redeemed()


func _on_boss_redeemed() -> void:
	if _region_completed:
		return
	_region_completed = true

	var region := REGIONS[_region_index]
	var flag_id := "boss_%s" % region.get("id", "")
	var reward_applied := false
	if _session == null or not _session.is_flag_set(flag_id):
		reward_applied = _apply_region_reward(region)
		if _session != null:
			_session.mark_flag(flag_id)

	_enable_travel_gate()
	_update_objective_for_completion(region)
	if _hud != null:
		if reward_applied:
			_hud.show_message("Blessings strengthened. The way forward is open.")
		else:
			_hud.show_message("The way forward is open.")

	_save_checkpoint()


func _apply_region_reward(region: Dictionary) -> bool:
	var reward := region.get("reward", {})
	if not (reward is Dictionary):
		return false
	var unlocked := false
	var health_bonus := float(reward.get("health", 0.0))
	var stamina_bonus := float(reward.get("stamina", 0.0))

	var miracle_id := str(reward.get("miracle", ""))
	if not miracle_id.is_empty() and _session != null:
		if _session.ensure_miracle_unlocked(miracle_id):
			unlocked = true

	if _session != null:
		_session.max_health_bonus += health_bonus
		_session.max_stamina_bonus += stamina_bonus

	if _player != null:
		_player.max_health += health_bonus
		_player.max_stamina += stamina_bonus
		_player.current_health = _player.max_health
		_player.current_stamina = _player.max_stamina
		_player.emit_signal("health_changed", _player.current_health, _player.max_health)
		_player.emit_signal("stamina_changed", _player.current_stamina, _player.max_stamina)
		if _session != null:
			_player.set_unlocked_miracles(_session.unlocked_miracles)

	if _hud != null and unlocked:
		_hud.show_message("New miracle unlocked: %s" % miracle_id.capitalize())

	return unlocked or health_bonus > 0.0 or stamina_bonus > 0.0


func _enable_travel_gate() -> void:
	if _travel_gate == null:
		return
	_travel_gate.set_gate_enabled(true)


func _update_objective_for_completion(region: Dictionary) -> void:
	if _hud == null:
		return
	if _region_index >= REGIONS.size() - 1:
		_hud.set_objective("Step into the light to conclude the pilgrimage.")
	else:
		var next_region := REGIONS[_region_index + 1]
		_hud.set_objective("Journey marker open: travel to %s." % next_region.get("name", "the next region"))


func _update_region_ui(region: Dictionary) -> void:
	if _hud == null:
		return
	_hud.set_region_name("Chapter %d: %s" % [region.get("chapter", 1), region.get("name", "Region")])
	_hud.set_objective(region.get("objective", ""))


func _on_travel_requested(target_region_index: int) -> void:
	if not _region_completed:
		if _player != null:
			_player.set_context_hint("Redeem the region's guardian to proceed.")
		return

	if target_region_index >= REGIONS.size():
		_save_checkpoint()
		get_tree().change_scene_to_file("res://scenes/Credits.tscn")
		return

	_region_index = target_region_index
	if _session != null:
		_session.region_index = _region_index
	_load_region(_region_index)
	_save_checkpoint()


func _save_checkpoint() -> void:
	if _player == null:
		return
	var payload := _player.build_save_payload()
	payload["campaign"] = _build_campaign_payload()
	SaveSystemScript.save_checkpoint(payload)


func _place_rock(at: Vector3, size: Vector3) -> void:
	var rock := StaticBody3D.new()
	rock.name = "Rock"
	_region_root.add_child(rock)
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
	_region_root.add_child(tree)

	var trunk := MeshInstance3D.new()
	var trunk_mesh := PrismMesh.new()
	trunk_mesh.left_to_right = 0.78
	trunk_mesh.size = Vector3(0.52, 2.8, 0.44)
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


func _place_pillar(at: Vector3, height: float) -> void:
	var pillar := StaticBody3D.new()
	pillar.name = "Pillar"
	pillar.position = at
	_region_root.add_child(pillar)

	var collision := CollisionShape3D.new()
	var shape := CylinderShape3D.new()
	shape.height = height
	shape.radius = 0.35
	collision.shape = shape
	collision.position = Vector3(0.0, height * 0.5, 0.0)
	pillar.add_child(collision)

	var mesh_instance := MeshInstance3D.new()
	var mesh := PrismMesh.new()
	mesh.left_to_right = 0.86
	mesh.size = Vector3(0.66, height, 0.66)
	mesh_instance.mesh = mesh
	mesh_instance.position = Vector3(0.0, height * 0.5, 0.0)
	mesh_instance.material_override = _make_material(Color(0.44, 0.29, 0.24))
	pillar.add_child(mesh_instance)


func _place_ruin_block(at: Vector3, size: Vector3) -> void:
	var ruin := StaticBody3D.new()
	ruin.name = "Ruin"
	ruin.position = at
	_region_root.add_child(ruin)

	var collision := CollisionShape3D.new()
	var shape := BoxShape3D.new()
	shape.size = size
	collision.shape = shape
	ruin.add_child(collision)

	var mesh_instance := MeshInstance3D.new()
	var mesh := BoxMesh.new()
	mesh.size = size
	mesh_instance.mesh = mesh
	mesh_instance.material_override = _make_material(Color(0.5, 0.47, 0.43))
	ruin.add_child(mesh_instance)


func _place_spire(at: Vector3, height: float, radius: float) -> void:
	var spire := StaticBody3D.new()
	spire.name = "Spire"
	spire.position = at
	_region_root.add_child(spire)

	var collision := CollisionShape3D.new()
	var shape := CylinderShape3D.new()
	shape.height = height
	shape.radius = radius * 0.6
	collision.shape = shape
	collision.position = Vector3(0.0, height * 0.5, 0.0)
	spire.add_child(collision)

	var mesh_instance := MeshInstance3D.new()
	var mesh := PrismMesh.new()
	mesh.left_to_right = 0.72
	mesh.size = Vector3(radius * 1.8, height, radius * 1.8)
	mesh_instance.mesh = mesh
	mesh_instance.position = Vector3(0.0, height * 0.5, 0.0)
	mesh_instance.material_override = _make_material(Color(0.5, 0.4, 0.28))
	spire.add_child(mesh_instance)


func _build_boss_arena(center: Vector3, radius: float, color: Color) -> void:
	var arena_floor := MeshInstance3D.new()
	var arena_disc := SphereMesh.new()
	arena_disc.radius = radius
	arena_disc.height = 0.12
	arena_floor.mesh = arena_disc
	arena_floor.scale = Vector3(1.0, 0.12, 1.0)
	arena_floor.position = center
	arena_floor.material_override = _make_material(color)
	_region_root.add_child(arena_floor)

	for i in range(10):
		var pillar := MeshInstance3D.new()
		var pillar_mesh := BoxMesh.new()
		pillar_mesh.size = Vector3(0.56, 2.2, 0.56)
		pillar.mesh = pillar_mesh
		var angle := TAU * float(i) / 10.0
		pillar.position = center + Vector3(cos(angle) * (radius + 0.6), 1.1, sin(angle) * (radius + 0.6))
		pillar.material_override = _make_material(Color(0.44, 0.29, 0.24))
		_region_root.add_child(pillar)


func _make_material(color: Color) -> StandardMaterial3D:
	var material := StandardMaterial3D.new()
	material.albedo_color = color
	material.roughness = 0.9
	return material


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


func _get_session() -> GameSession:
	return get_node_or_null("/root/GameSession") as GameSession
