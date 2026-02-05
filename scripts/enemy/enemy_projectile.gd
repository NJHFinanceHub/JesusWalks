extends Area3D
class_name EnemyProjectile


@export var speed: float = 18.0
@export var damage: float = 12.0
@export var posture_damage: float = 14.0
@export var lifetime: float = 6.0


var direction: Vector3 = Vector3.FORWARD
var owner_enemy: EnemyAI


func _ready() -> void:
	monitoring = true
	monitorable = true
	body_entered.connect(_on_body_entered)
	_build_visuals()


func _physics_process(delta: float) -> void:
	global_position += direction * speed * delta
	lifetime -= delta
	if lifetime <= 0.0:
		queue_free()


func _build_visuals() -> void:
	var collision := CollisionShape3D.new()
	var shape := SphereShape3D.new()
	shape.radius = 0.17
	collision.shape = shape
	add_child(collision)

	var mesh_instance := MeshInstance3D.new()
	var mesh := SphereMesh.new()
	mesh.radius = 0.16
	mesh.height = 0.32
	mesh_instance.mesh = mesh
	var material := StandardMaterial3D.new()
	material.albedo_color = Color(0.78, 0.18, 0.13)
	material.emission_enabled = true
	material.emission = Color(0.85, 0.21, 0.11)
	material.emission_energy_multiplier = 1.2
	material.roughness = 0.24
	mesh_instance.material_override = material
	add_child(mesh_instance)

	var light := OmniLight3D.new()
	light.light_color = Color(0.95, 0.4, 0.21)
	light.light_energy = 0.9
	light.omni_range = 4.2
	add_child(light)


func _on_body_entered(body: Node) -> void:
	var player := body as PlayerController
	if player == null:
		return

	player.receive_enemy_attack(owner_enemy, damage, posture_damage)
	queue_free()

