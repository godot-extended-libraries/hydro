extends SceneTree

var step_count = 0
var start_time
var stop_depth

var cubes = []

const AREA_EXTENT = 16384

func display_help():
	print("""
		Arguments:
			--cubes=n
				The number of cubes to generate
			--density=n
				Sets the density of the cubes.  Must be larger than 1.0 in order to sink.
			--depth=n
				Exit when all cubes sink to this depth
			--faces=n
				The number of faces to generate per-cube.  For exact results, use 6 times a square number.
				For example, 3 * 3 * 6 = 54 faces.
			--size=n
				The size of each cube
	""")

func generate_water_area():
	var area = WaterArea.new()
	var area_shape = CollisionShape.new()
	area_shape.shape = BoxShape.new()
	area_shape.shape.extents = Vector3(AREA_EXTENT, AREA_EXTENT, AREA_EXTENT)
	area.add_child(area_shape)
	return area

func generate_cube(density, size, num_faces):
	var rb = HydroRigidBody.new()
	rb.mass = size * size * size * 1000 * density
	
	var rb_shape = CollisionShape.new()
	rb_shape.shape = BoxShape.new()
	rb_shape.shape.extents = Vector3(size / 2.0, size / 2.0, size / 2.0)
	rb.add_child(rb_shape)
	
	var rb_hull = MeshInstance.new()
	rb_hull.mesh = CubeMesh.new()
	rb_hull.mesh.size = Vector3(size, size, size)
	var subdiv = round(sqrt(num_faces / 6.0)) - 1
	rb_hull.mesh.set_subdivide_width(subdiv)
	rb_hull.mesh.set_subdivide_height(subdiv)
	rb_hull.mesh.set_subdivide_depth(subdiv)
	rb.add_child(rb_hull)
	
	return rb

func _initialize():
	
	var arguments = {
		"cubes": 1,
		"faces": 6,
		"size": 10,
		"depth": 10,
		"density": 1.1
	}
	
	var found = false
	for argument in OS.get_cmdline_args():
		if argument.find("=") > -1:
			var key_value = argument.split("=")
			arguments[key_value[0].lstrip("--")] = key_value[1]
			found = true
	
	if !found:
		display_help()
		quit(0)
		return
	
	var num_cubes = int(arguments["cubes"])
	var faces = int(arguments["faces"])
	var size = int(arguments["size"])
	var density = float(arguments["density"])
	stop_depth = -abs(float(arguments["depth"]))
	
	root.add_child(generate_water_area())
	
	for i in range(0, num_cubes):
		var cube = generate_cube(density, size, faces)
		cube.translate(Vector3((i * size * 2) % AREA_EXTENT , 0, (i * size * 2) / AREA_EXTENT))
		root.add_child(cube);
		cubes.append(cube)
		
	start_time = OS.get_ticks_msec()

func _iteration(delta):
	step_count += 1
	for c in cubes:
		#print(c.global_transform.origin.y)
		if c.global_transform.origin.y > stop_depth:
			return false

	var totalTime = (OS.get_ticks_msec() - start_time) / 1000.0

	print("Total steps:   ", step_count)
	print("Game time:     ", step_count * delta)
	print("Actual time:   ", totalTime)
	print("Step delta:    ", delta)
	print("Avg step time: ", totalTime / step_count)
	
	quit(0)
	return true
