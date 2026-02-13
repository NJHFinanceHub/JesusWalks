extends Node
class_name AudioManager


## Centralized audio system for game-wide sound effects and music playback
## Manages pooled AudioStreamPlayers for efficient sound effect playback


signal music_changed(track_name: String)
signal sfx_volume_changed(volume_db: float)
signal music_volume_changed(volume_db: float)


enum SFXType {
	## Combat sounds
	PLAYER_LIGHT_ATTACK,
	PLAYER_HEAVY_ATTACK,
	PLAYER_DODGE,
	PLAYER_BLOCK,
	PLAYER_PARRY,
	PLAYER_PARRY_SUCCESS,
	PLAYER_HIT,
	PLAYER_DEATH,

	## Enemy sounds
	ENEMY_ATTACK,
	ENEMY_HIT,
	ENEMY_BLOCK,
	ENEMY_STAGGER,
	ENEMY_DEATH,
	ENEMY_REDEEMED,

	## Miracle sounds
	MIRACLE_HEAL,
	MIRACLE_CAST,

	## Environment sounds
	PRAYER_SITE_ACTIVATE,
	PRAYER_SITE_REST,
	SAVE_GAME,
	LOAD_GAME,

	## UI sounds
	LOCK_ON,
	LOCK_OFF,
}


@export_category("Audio Settings")
@export var max_concurrent_sounds: int = 32
@export var default_sfx_volume_db: float = 0.0
@export var default_music_volume_db: float = -6.0


var _sfx_pool: Array[AudioStreamPlayer] = []
var _music_player: AudioStreamPlayer
var _ambient_player: AudioStreamPlayer
var _sfx_volume_db: float
var _music_volume_db: float
var _sound_library: Dictionary = {}
var _music_library: Dictionary = {}


func _ready() -> void:
	_sfx_volume_db = default_sfx_volume_db
	_music_volume_db = default_music_volume_db
	_initialize_audio_players()
	_load_sound_library()


func _initialize_audio_players() -> void:
	# Create music player
	_music_player = AudioStreamPlayer.new()
	_music_player.name = "MusicPlayer"
	_music_player.bus = "Music"
	_music_player.volume_db = _music_volume_db
	add_child(_music_player)

	# Create ambient player
	_ambient_player = AudioStreamPlayer.new()
	_ambient_player.name = "AmbientPlayer"
	_ambient_player.bus = "Ambient"
	_ambient_player.volume_db = _music_volume_db
	add_child(_ambient_player)

	# Create SFX pool
	for i in max_concurrent_sounds:
		var player := AudioStreamPlayer.new()
		player.name = "SFXPlayer_%d" % i
		player.bus = "SFX"
		player.volume_db = _sfx_volume_db
		add_child(player)
		_sfx_pool.append(player)


func _load_sound_library() -> void:
	_sound_library = {}
	_music_library = {
		"epic_biblical_theme": preload("res://assets/music/epic_biblical_theme.wav"),
		"main_theme": preload("res://assets/music/epic_biblical_theme.wav"),
	}


func play_sfx(sfx_type: SFXType, pitch_variation: float = 0.0, volume_offset_db: float = 0.0) -> void:
	## Play a sound effect with optional pitch variation and volume offset
	var player := _get_available_sfx_player()
	if player == null:
		push_warning("AudioManager: No available SFX players in pool")
		return

	var stream := _sound_library.get(sfx_type, null)
	if stream == null:
		return

	player.stream = stream
	player.volume_db = _sfx_volume_db + volume_offset_db

	if pitch_variation != 0.0:
		player.pitch_scale = 1.0 + randf_range(-pitch_variation, pitch_variation)
	else:
		player.pitch_scale = 1.0

	player.play()


func play_music(track_name: String, fade_in_duration: float = 0.0) -> void:
	## Play a music track with optional fade-in
	var stream := _music_library.get(track_name, null)
	if stream == null:
		push_warning("AudioManager: Unknown track '%s'" % track_name)
		return

	_music_player.stream = stream
	if fade_in_duration > 0.0:
		_fade_in_music(fade_in_duration)
	else:
		_music_player.play()
	music_changed.emit(track_name)


func stop_music(fade_out_duration: float = 0.0) -> void:
	## Stop the current music track with optional fade-out
	if fade_out_duration > 0.0:
		_fade_out_music(fade_out_duration)
	else:
		_music_player.stop()


func play_ambient(ambient_name: String, loop: bool = true) -> void:
	## Play an ambient sound loop (wind, waves, etc.)
	## Placeholder for when ambient audio is added
	# Disconnect any existing connection to prevent memory leaks
	if _ambient_player.finished.is_connected(_on_ambient_finished):
		_ambient_player.finished.disconnect(_on_ambient_finished)
	# Connect with loop parameter
	if not _ambient_player.finished.is_connected(_on_ambient_finished):
		_ambient_player.finished.connect(_on_ambient_finished.bind(loop))


func set_sfx_volume(volume_db: float) -> void:
	_sfx_volume_db = volume_db
	for player in _sfx_pool:
		player.volume_db = volume_db
	sfx_volume_changed.emit(volume_db)


func set_music_volume(volume_db: float) -> void:
	_music_volume_db = volume_db
	_music_player.volume_db = volume_db
	_ambient_player.volume_db = volume_db
	music_volume_changed.emit(volume_db)


func _get_available_sfx_player() -> AudioStreamPlayer:
	for player in _sfx_pool:
		if not player.playing:
			return player
	# Return the oldest playing sound if all are busy
	return _sfx_pool[0]


func _fade_in_music(duration: float) -> void:
	_music_player.volume_db = -80.0
	_music_player.play()
	var tween := create_tween()
	tween.tween_property(_music_player, "volume_db", _music_volume_db, duration)


func _fade_out_music(duration: float) -> void:
	var tween := create_tween()
	tween.tween_property(_music_player, "volume_db", -80.0, duration)
	tween.tween_callback(_music_player.stop)


func _on_ambient_finished(should_loop: bool) -> void:
	if should_loop:
		_ambient_player.play()
