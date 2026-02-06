extends Area3D
class_name TravelGate


signal travel_requested(target_region_index: int)


@export var target_region_index: int = 0
@export var prompt_text: String = "Press E to travel onward"

var _player_inside: PlayerController
var _pulse_time: float = 0.0


func _ready() -> void:
	monitoring = true
	monitorable = true
	_build_gate_visual()
	body_entered.connect(_on_body_entered)
	body_exited.connect(_on_body_exited)


func _physics_process(delta: float) -> void:
	_pulse_time += delta
	var ring := get_node_or_null("GateRing") as MeshInstance3D
	if ring != null:
		var pulse := 1.0 + sin(_pulse_time * 2.3) * 0.06
		ring.scale = Vector3.ONE * pulse

	if _player_inside != null and Input.is_action_just_pressed("interact"):
		travel_requested.emit(target_region_index)


func set_gate_enabled(enabled: bool) -> void:
	visible = enabled
	monitoring = enabled
	monitorable = enabled


func _build_gate_visual() -> void:
	var interaction_shape := get_node_or_null("InteractionShape") as CollisionShape3D
	if interaction_shape == null:
		interaction_shape = CollisionShape3D.new()
		interaction_shape.name = "InteractionShape"
		var sphere := SphereShape3D.new()
		sphere.radius = 2.8
		interaction_shape.shape = sphere
		interaction_shape.position = Vector3(0.0, 1.0, 0.0)
		add_child(interaction_shape)

	var ring := get_node_or_null("GateRing") as MeshInstance3D
	if ring == null:
		ring = MeshInstance3D.new()
		ring.name = "GateRing"
		var torus := TorusMesh.new()
		torus.ring_radius = 1.2
		torus.pipe_radius = 0.22
		ring.mesh = torus
		ring.position = Vector3(0.0, 1.1, 0.0)
		var material := StandardMaterial3D.new()
		material.albedo_color = Color(0.95, 0.86, 0.62)
		material.emission_enabled = true
		material.emission = Color(0.96, 0.8, 0.5)
		material.emission_energy_multiplier = 1.2
		material.roughness = 0.35
		ring.material_override = material
		add_child(ring)

	var pillar := get_node_or_null("GatePillar") as MeshInstance3D
	if pillar == null:
		pillar = MeshInstance3D.new()
		pillar.name = "GatePillar"
		var column := CylinderMesh.new()
		column.top_radius = 0.45
		column.bottom_radius = 0.55
		column.height = 1.2
		pillar.mesh = column
		pillar.position = Vector3(0.0, 0.6, 0.0)
		var pillar_material := StandardMaterial3D.new()
		pillar_material.albedo_color = Color(0.6, 0.54, 0.44)
		pillar_material.roughness = 0.85
		pillar.material_override = pillar_material
		add_child(pillar)

	var light := get_node_or_null("GateLight") as OmniLight3D
	if light == null:
		light = OmniLight3D.new()
		light.name = "GateLight"
		light.position = Vector3(0.0, 1.4, 0.0)
		light.light_color = Color(1.0, 0.86, 0.6)
		light.light_energy = 1.2
		light.omni_range = 10.0
		add_child(light)


func _on_body_entered(body: Node) -> void:
	var player := body as PlayerController
	if player == null:
		return
	_player_inside = player
	_player_inside.set_context_hint(prompt_text)


func _on_body_exited(body: Node) -> void:
	if body != _player_inside:
		return
	_player_inside.set_context_hint("")
	_player_inside = null

