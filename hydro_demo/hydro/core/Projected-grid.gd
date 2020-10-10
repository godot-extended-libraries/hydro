tool
extends ImmediateGeometry

const NUMBER_OF_WAVES = 10;

export(float, 0, 10000) var wavelength = 60.0 setget set_wavelength
export(float, 0, 1) var steepness = 0.01 setget set_steepness
export(float, 0, 10000) var amplitude = 0.1 setget set_amplitude
export(Vector2) var wind_direction = Vector2(1, 0) setget set_wind_direction
export(float, 0, 1) var wind_align = 0.0 setget set_wind_align
export(float) var speed = 10.0 setget set_speed

export(bool) var noise_enabled = true setget set_noise_enabled
export(float) var noise_amplitude = 0.28 setget set_noise_amplitude
export(float) var noise_frequency = 0.065 setget set_noise_frequency
export(float) var noise_speed = 0.48 setget set_noise_speed

export(int) var seed_value = 0 setget set_seed

var res = 100.0
var initialized = false

var counter = 0.5
var cube_cam = preload("res://hydro/core/CubeCamera.tscn")
var cube_cam_inst;

var waves = []
var waves_in_tex = ImageTexture.new()

func _ready():
	
	for j in range(res):
		var y = j/res - 0.5
		var n_y = (j+1)/res - 0.5
		begin(Mesh.PRIMITIVE_TRIANGLE_STRIP)
		for i in range(res):
			var x = i/res - 0.5
			
			var new_x = x 
			var new_y = y
			
			add_vertex(Vector3(x*2, 0, -y*2))
			
			new_y = n_y - translation.z
			add_vertex(Vector3(x*2, 0, -n_y*2))
		end()
	begin(Mesh.PRIMITIVE_POINTS)
	add_vertex(-Vector3(1,1,1)*pow(2,32))
	add_vertex(Vector3(1,1,1)*pow(2,32))
	end()
	
	waves_in_tex = ImageTexture.new()
	update_waves()
	
	cube_cam_inst = cube_cam.instance()
	add_child(cube_cam_inst)


func _process(delta):
	counter -= delta
	if counter <= 0 && cube_cam_inst:
		var cube_map = cube_cam_inst.update_cube_map()
		material_override.set_shader_param('environment', cube_map)
		counter = INF
	
	material_override.set_shader_param('time_offset', OS.get_ticks_msec()/1000.0 * speed)
	initialized = true

func set_wavelength(value):
	wavelength = value
	if initialized:
		update_waves()

func set_steepness(value):
	steepness = value
	if initialized:
		update_waves()

func set_amplitude(value):
	amplitude = value
	if initialized:
		update_waves()

func set_wind_direction(value):
	wind_direction = value
	if initialized:
		update_waves()

func set_wind_align(value):
	wind_align = value
	if initialized:
		update_waves()

func set_seed(value):
	seed_value = value
	if initialized:
		update_waves()

func set_speed(value):
	speed = value
	material_override.set_shader_param('speed', value)

func set_noise_enabled(value):
	noise_enabled = value
	var old_noise_params = material_override.get_shader_param('noise_params')
	old_noise_params.d = 1 if value else 0
	material_override.set_shader_param('noise_params', old_noise_params)

func set_noise_amplitude(value):
	noise_amplitude = value
	var old_noise_params = material_override.get_shader_param('noise_params')
	old_noise_params.x = value
	material_override.set_shader_param('noise_params', old_noise_params)

func set_noise_frequency(value):
	noise_frequency = value
	var old_noise_params = material_override.get_shader_param('noise_params')
	old_noise_params.y = value
	material_override.set_shader_param('noise_params', old_noise_params)

func set_noise_speed(value):
	noise_speed = value
	var old_noise_params = material_override.get_shader_param('noise_params')
	old_noise_params.z = value
	material_override.set_shader_param('noise_params', old_noise_params)

func get_displace(position):
	
	var new_p;
	if typeof(position) == TYPE_VECTOR3:
		new_p = Vector3(position.x, 0.0, position.z)
		position = Vector2(position.x, position.z)
	elif typeof(position) == TYPE_VECTOR2:
		new_p = Vector3(position.x, 0.0, position.y)
	else:
		printerr('Position is not a vector3!')
		breakpoint
	
	var w; var amp; var steep; var phase; var dir
	for i in waves:
		amp = i['amplitude']
		if amp == 0.0: continue
		
		dir = Vector2(i['wind_directionX'], i['wind_directionY'])
		w = i['frequency']
		steep = i['steepness'] / (w*amp)
		phase = 2.0 * w
		
		var W = position.dot(w*dir) + phase * OS.get_ticks_msec()/1000.0 * speed
		
		new_p.x += steep*amp * dir.x * cos(W)
		new_p.z += steep*amp * dir.y * cos(W)
		new_p.y += amp * sin(W)
	return new_p;

func update_waves():
	#Generate Waves..
	seed(seed_value)
	var amp_length_ratio = amplitude / wavelength
	waves.clear()
	for i in range(NUMBER_OF_WAVES):
		var _wavelength = rand_range(wavelength/2.0, wavelength*2.0)
		var _wind_direction = wind_direction.rotated(rand_range(-PI, PI)*(1-wind_align))
		
		waves.append({
			'amplitude': amp_length_ratio * _wavelength,
			'steepness': rand_range(0, steepness),
			'wind_directionX': _wind_direction.x,
			'wind_directionY': _wind_direction.y,
			'frequency': sqrt(0.098 * TAU/_wavelength)
		})
	#Put Waves in Texture..
	var img = Image.new()
	img.create(5, NUMBER_OF_WAVES, false, Image.FORMAT_RF)
	img.lock()
	for i in range(NUMBER_OF_WAVES):
		img.set_pixel(0, i, Color(waves[i]['amplitude'], 0,0,0))
		img.set_pixel(1, i, Color(waves[i]['steepness'], 0,0,0))
		img.set_pixel(2, i, Color(waves[i]['wind_directionX'], 0,0,0))
		img.set_pixel(3, i, Color(waves[i]['wind_directionY'], 0,0,0))
		img.set_pixel(4, i, Color(waves[i]['frequency'], 0,0,0))
	img.unlock()
	waves_in_tex.create_from_image(img, 0)
	
	material_override.set_shader_param('waves', waves_in_tex)
