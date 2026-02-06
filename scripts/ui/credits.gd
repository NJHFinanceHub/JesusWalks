extends Control
class_name CreditsScreen


func _ready() -> void:
	Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)
	_build_ui()


func _unhandled_input(event: InputEvent) -> void:
	if event.is_action_pressed("pause") or (event is InputEventKey and event.pressed and event.keycode == Key.KEY_ESCAPE):
		_return_to_menu()


func _build_ui() -> void:
	set_anchors_preset(Control.PRESET_FULL_RECT)

	var backdrop := ColorRect.new()
	backdrop.color = Color(0.05, 0.05, 0.06, 1.0)
	backdrop.set_anchors_preset(Control.PRESET_FULL_RECT)
	add_child(backdrop)

	var panel := PanelContainer.new()
	panel.offset_left = 280.0
	panel.offset_top = 140.0
	panel.offset_right = 1320.0
	panel.offset_bottom = 760.0
	add_child(panel)

	var style := StyleBoxFlat.new()
	style.bg_color = Color(0.08, 0.08, 0.07, 0.94)
	style.corner_radius_top_left = 16
	style.corner_radius_top_right = 16
	style.corner_radius_bottom_left = 16
	style.corner_radius_bottom_right = 16
	panel.add_theme_stylebox_override("panel", style)

	var layout := VBoxContainer.new()
	layout.add_theme_constant_override("separation", 12)
	layout.offset_left = 24.0
	layout.offset_right = 960.0
	layout.offset_top = 24.0
	layout.offset_bottom = 580.0
	panel.add_child(layout)

	var title := Label.new()
	title.text = "Pilgrimage Complete"
	title.add_theme_color_override("font_color", Color(0.96, 0.9, 0.78))
	title.add_theme_font_size_override("font_size", 28)
	layout.add_child(title)

	var body := Label.new()
	body.text = "Thank you for walking the path.\n\nDesigned & built in Godot 4.\nNarrative systems, combat systems, and level scripting\ncrafted for this prototype.\n\nMay your journey continue."
	body.add_theme_color_override("font_color", Color(0.85, 0.82, 0.7))
	body.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
	layout.add_child(body)

	var button := Button.new()
	button.text = "Return to Main Menu"
	button.pressed.connect(_return_to_menu)
	layout.add_child(button)


func _return_to_menu() -> void:
	get_tree().change_scene_to_file("res://scenes/MainMenu.tscn")
