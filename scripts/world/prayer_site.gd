extends Area3D
class_name PrayerSite


@export var site_id: String = "galilee_site_01"
@export var site_name: String = "Galilee Prayer Site"
@export var prompt_text: String = "Press E to pray and rest"


var _player_inside: PlayerController
var _pulse_time: float = 0.0


func _ready() -> void:
	add_to_group("prayer_sites")
	monitoring = true
	monitorable = true
	_build_site_visual()
	body_entered.connect(_on_body_entered)
	body_exited.connect(_on_body_exited)


func _physics_process(delta: float) -> void:
	_pulse_time += delta
	var altar_mesh := get_node_or_null("AltarMesh") as MeshInstance3D
	if altar_mesh != null:
		var pulse := 1.0 + sin(_pulse_time * 2.4) * 0.05
		altar_mesh.scale = Vector3.ONE * pulse

	if _player_inside != null and Input.is_action_just_pressed("interact"):
		_player_inside.rest_at_prayer_site(self)


func _build_site_visual() -> void:
	var interaction_shape := get_node_or_null("InteractionShape") as CollisionShape3D
	if interaction_shape == null:
		interaction_shape = CollisionShape3D.new()
		interaction_shape.name = "InteractionShape"
		var sphere := SphereShape3D.new()
		sphere.radius = 2.4
		interaction_shape.shape = sphere
		interaction_shape.position = Vector3(0.0, 1.0, 0.0)
		add_child(interaction_shape)

	var altar_mesh := get_node_or_null("AltarMesh") as MeshInstance3D
	if altar_mesh == null:
		altar_mesh = MeshInstance3D.new()
		altar_mesh.name = "AltarMesh"
		var cylinder := CylinderMesh.new()
		cylinder.top_radius = 0.65
		cylinder.bottom_radius = 0.85
		cylinder.height = 0.5
		altar_mesh.mesh = cylinder
		altar_mesh.position = Vector3(0.0, 0.25, 0.0)
		var altar_material := StandardMaterial3D.new()
		altar_material.albedo_color = Color(0.77, 0.71, 0.6)
		altar_material.emission_enabled = true
		altar_material.emission = Color(0.84, 0.76, 0.52)
		altar_material.emission_energy_multiplier = 0.9
		altar_material.roughness = 0.78
		altar_mesh.material_override = altar_material
		add_child(altar_mesh)

	var flame_light := get_node_or_null("PrayerLight") as OmniLight3D
	if flame_light == null:
		flame_light = OmniLight3D.new()
		flame_light.name = "PrayerLight"
		flame_light.position = Vector3(0.0, 1.6, 0.0)
		flame_light.light_color = Color(1.0, 0.9, 0.74)
		flame_light.light_energy = 1.2
		flame_light.omni_range = 8.5
		add_child(flame_light)


func _on_body_entered(body: Node) -> void:
	var player := body as PlayerController
	if player == null:
		return

	_player_inside = player
	_player_inside.set_active_prayer_site(self)


func _on_body_exited(body: Node) -> void:
	if body != _player_inside:
		return

	_player_inside.clear_active_prayer_site(self)
	_player_inside = null


func get_respawn_position() -> Vector3:
	return global_position + Vector3(0.0, 1.1, 2.4)


func get_prompt_message() -> String:
	return "%s: %s | Save F1-F3 | Load F5-F7" % [site_name, prompt_text]

