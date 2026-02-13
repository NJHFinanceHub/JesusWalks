extends CanvasLayer
class_name PauseMenu


signal resume_requested
signal return_to_menu_requested


const SaveSystemScript := preload("res://scripts/persistence/save_system.gd")


var _player: PlayerController
var _campaign: CampaignManager
var _slot_labels: Array[Label] = []


func _ready() -> void:
	_build_ui()
	visible = false
	process_mode = Node.PROCESS_MODE_ALWAYS


func _unhandled_input(event: InputEvent) -> void:
	if not visible:
		return
	if event.is_action_pressed("pause") or (event is InputEventKey and event.pressed and event.keycode == Key.KEY_ESCAPE):
		resume_requested.emit()


func bind(player: PlayerController, campaign: CampaignManager) -> void:
	_player = player
	_campaign = campaign
	_refresh_slots()


func show_menu() -> void:
	_refresh_slots()
	visible = true


func hide_menu() -> void:
	visible = false


func _build_ui() -> void:
	var root := Control.new()
	root.name = "PauseRoot"
	root.set_anchors_preset(Control.PRESET_FULL_RECT)
	add_child(root)

	var dim := ColorRect.new()
	dim.color = Color(0.03, 0.03, 0.03, 0.7)
	dim.set_anchors_preset(Control.PRESET_FULL_RECT)
	root.add_child(dim)

	var panel := PanelContainer.new()
	panel.offset_left = 460.0
	panel.offset_top = 140.0
	panel.offset_right = 1140.0
	panel.offset_bottom = 760.0
	root.add_child(panel)

	var style := StyleBoxFlat.new()
	style.bg_color = Color(0.08, 0.08, 0.06, 0.92)
	style.corner_radius_top_left = 12
	style.corner_radius_top_right = 12
	style.corner_radius_bottom_left = 12
	style.corner_radius_bottom_right = 12
	panel.add_theme_stylebox_override("panel", style)

	var layout := VBoxContainer.new()
	layout.add_theme_constant_override("separation", 8)
	layout.offset_left = 24.0
	layout.offset_right = 640.0
	layout.offset_top = 24.0
	layout.offset_bottom = 620.0
	panel.add_child(layout)

	var title := Label.new()
	title.text = "Paused"
	title.add_theme_color_override("font_color", Color(0.95, 0.9, 0.78))
	layout.add_child(title)

	layout.add_child(_make_button("Resume", _on_resume_pressed))

	var save_label := Label.new()
	save_label.text = "Quick Saves"
	save_label.add_theme_color_override("font_color", Color(0.85, 0.82, 0.7))
	layout.add_child(save_label)

	layout.add_child(_make_button("Save Slot 1", func(): _save_slot(1)))
	layout.add_child(_make_button("Save Slot 2", func(): _save_slot(2)))
	layout.add_child(_make_button("Save Slot 3", func(): _save_slot(3)))

	var load_label := Label.new()
	load_label.text = "Quick Loads"
	load_label.add_theme_color_override("font_color", Color(0.85, 0.82, 0.7))
	layout.add_child(load_label)

	layout.add_child(_make_button("Load Slot 1", func(): _load_slot(1)))
	layout.add_child(_make_button("Load Slot 2", func(): _load_slot(2)))
	layout.add_child(_make_button("Load Slot 3", func(): _load_slot(3)))

	var slot_panel := PanelContainer.new()
	var slot_style := StyleBoxFlat.new()
	slot_style.bg_color = Color(0.05, 0.05, 0.04, 0.75)
	slot_style.corner_radius_top_left = 8
	slot_style.corner_radius_top_right = 8
	slot_style.corner_radius_bottom_left = 8
	slot_style.corner_radius_bottom_right = 8
	slot_panel.add_theme_stylebox_override("panel", slot_style)
	layout.add_child(slot_panel)

	var slot_layout := VBoxContainer.new()
	slot_layout.add_theme_constant_override("separation", 4)
	slot_layout.offset_left = 12.0
	slot_layout.offset_right = 560.0
	slot_layout.offset_top = 8.0
	slot_layout.offset_bottom = 140.0
	slot_panel.add_child(slot_layout)

	for i in range(3):
		var label := Label.new()
		label.text = "Slot %d: Empty" % (i + 1)
		label.add_theme_color_override("font_color", Color(0.83, 0.8, 0.68))
		slot_layout.add_child(label)
		_slot_labels.append(label)

	layout.add_child(_make_button("Return to Main Menu", _on_return_to_menu_pressed))
	layout.add_child(_make_button("Quit Game", _on_quit_pressed))


func _make_button(text: String, action: Callable) -> Button:
	var button := Button.new()
	button.text = text
	button.pressed.connect(action)
	return button


func _refresh_slots() -> void:
	for i in range(_slot_labels.size()):
		_slot_labels[i].text = SaveSystemScript.slot_summary(i + 1)


func _save_slot(slot_id: int) -> void:
	if _player == null:
		return
	_player.save_to_slot(slot_id)
	_refresh_slots()


func _load_slot(slot_id: int) -> void:
	if _player == null:
		return
	_player.load_from_slot(slot_id)
	_refresh_slots()


func _on_resume_pressed() -> void:
	resume_requested.emit()


func _on_return_to_menu_pressed() -> void:
	return_to_menu_requested.emit()


func _on_quit_pressed() -> void:
	get_tree().quit()
