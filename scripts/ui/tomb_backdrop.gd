extends Control
class_name TombBackdrop

@export var pan_speed: float = 0.08
@export var zoom_out_rate: float = 0.011
@export var max_zoom_out: float = 0.2

var _time: float = 0.0

func _ready() -> void:
	set_anchors_preset(Control.PRESET_FULL_RECT)
	mouse_filter = Control.MOUSE_FILTER_IGNORE


func _process(delta: float) -> void:
	_time += delta
	queue_redraw()


func _draw_scaled_circle(center: Vector2, radii: Vector2, color: Color) -> void:
	if radii.x <= 0.0 or radii.y <= 0.0:
		return
	draw_set_transform(center, 0.0, radii)
	draw_circle(Vector2.ZERO, 1.0, color)
	draw_set_transform(Vector2.ZERO, 0.0, Vector2.ONE)


func _draw() -> void:
	var size := get_size()
	if size.x < 1.0 or size.y < 1.0:
		return

	var pan := Vector2(sin(_time * pan_speed) * 48.0, cos(_time * pan_speed * 0.85) * 24.0)
	var zoom := 1.0 - min(_time * zoom_out_rate, max_zoom_out)

	_draw_sky(size, pan)
	_draw_distance_glow(size, pan, zoom)
	_draw_stars(size)
	_draw_land(size, pan, zoom)
	_draw_tomb(size, pan, zoom)
	_draw_mist(size, pan)
	_draw_vignette(size)


func _draw_sky(size: Vector2, pan: Vector2) -> void:
	var slices := 18
	for i in range(slices):
		var t := float(i) / float(slices - 1)
		var y0 := size.y * t
		var h := size.y / float(slices)
		var color := Color(
			lerp(0.03, 0.16, t),
			lerp(0.04, 0.12, t),
			lerp(0.08, 0.11, t),
			1.0
		)
		draw_rect(Rect2(Vector2(0.0, y0 + pan.y * 0.18), Vector2(size.x, h + 1.0)), color, true)


func _draw_distance_glow(size: Vector2, pan: Vector2, zoom: float) -> void:
	for i in range(6):
		var r := Vector2(size.x * (0.2 + i * 0.14), size.y * (0.06 + i * 0.04)) * zoom
		var alpha := 0.16 - i * 0.018
		_draw_scaled_circle(
			Vector2(size.x * 0.5 + pan.x * 0.2, size.y * 0.3 + pan.y * 0.25),
			r,
			Color(0.97, 0.78, 0.5, max(alpha, 0.02))
		)


func _draw_stars(size: Vector2) -> void:
	for i in range(70):
		var seed := float(i) * 17.33
		var x := fmod(seed * 71.0, size.x)
		var y := fmod(seed * 43.0, size.y * 0.52)
		var pulse := 0.4 + abs(sin(_time * 0.35 + seed)) * 0.6
		draw_circle(Vector2(x, y), 0.8 + pulse, Color(1.0, 0.95, 0.82, 0.12 + pulse * 0.28))


func _draw_land(size: Vector2, pan: Vector2, zoom: float) -> void:
	var horizon := size.y * 0.57 + pan.y * 0.35
	draw_rect(Rect2(0.0, horizon, size.x, size.y - horizon), Color(0.08, 0.07, 0.07, 1.0), true)

	for i in range(4):
		var hill_w := size.x * (0.4 + i * 0.23) * zoom
		var hill_h := size.y * (0.1 + i * 0.05)
		var hill_x := size.x * (0.15 + i * 0.2) + sin(_time * 0.11 + i) * 20.0 + pan.x * (0.2 + i * 0.07)
		var hill_y := horizon + 14.0 + i * 26.0
		_draw_scaled_circle(Vector2(hill_x, hill_y), Vector2(hill_w, hill_h), Color(0.11 + i * 0.02, 0.1 + i * 0.015, 0.1 + i * 0.015, 0.9))


func _draw_tomb(size: Vector2, pan: Vector2, zoom: float) -> void:
	var ground := size.y * 0.63 + pan.y * 0.5
	var center := Vector2(size.x * 0.52 + pan.x * 0.9, ground)
	var width := size.x * 0.24 * zoom
	var height := size.y * 0.28 * zoom

	var shell := Rect2(center - Vector2(width * 0.5, height * 0.78), Vector2(width, height))
	draw_rect(shell, Color(0.24, 0.22, 0.2, 1.0), true)
	draw_rect(Rect2(shell.position + Vector2(8.0, 8.0), shell.size - Vector2(16.0, 16.0)), Color(0.16, 0.15, 0.14, 1.0), true)

	var opening_r := height * 0.27
	var opening_center := shell.position + Vector2(shell.size.x * 0.5, shell.size.y * 0.56)
	draw_circle(opening_center, opening_r, Color(0.02, 0.02, 0.03, 1.0))

	for i in range(5):
		var ring_r := opening_r * (1.2 + i * 0.4) + sin(_time * 0.9 + i * 0.8) * 4.0
		var ring_a := 0.2 - i * 0.03
		draw_circle(opening_center, ring_r, Color(0.96, 0.84, 0.58, max(ring_a, 0.03)))

	var stone_center := opening_center + Vector2(width * 0.42, height * 0.12)
	draw_circle(stone_center, opening_r * 0.84, Color(0.33, 0.3, 0.27, 1.0))
	draw_circle(stone_center + Vector2(-8.0, -7.0), opening_r * 0.22, Color(0.42, 0.38, 0.33, 0.7))


func _draw_mist(size: Vector2, pan: Vector2) -> void:
	var base_y := size.y * 0.65
	for i in range(7):
		var phase := _time * 0.13 + float(i) * 0.7
		var y := base_y + i * 20.0 + sin(phase) * 8.0
		var x := size.x * 0.5 + sin(phase * 0.85) * 34.0 + pan.x * (0.1 + i * 0.05)
		var w := size.x * (0.28 + i * 0.11)
		var h := 18.0 + i * 6.0
		_draw_scaled_circle(Vector2(x, y), Vector2(w, h), Color(0.42, 0.39, 0.39, 0.07))


func _draw_vignette(size: Vector2) -> void:
	draw_rect(Rect2(0.0, 0.0, size.x, 160.0), Color(0, 0, 0, 0.5), true)
	draw_rect(Rect2(0.0, size.y - 180.0, size.x, 180.0), Color(0, 0, 0, 0.58), true)
	draw_rect(Rect2(0.0, 0.0, 160.0, size.y), Color(0, 0, 0, 0.44), true)
	draw_rect(Rect2(size.x - 160.0, 0.0, 160.0, size.y), Color(0, 0, 0, 0.44), true)
