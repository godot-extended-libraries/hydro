tool
extends Spatial

func ready():
	for i in get_children():
		i.size = Vector2(256, 256)
		i.own_world = true

func update_cube_map():
	var images = {
		'left': CubeMap.SIDE_LEFT,
		'right': CubeMap.SIDE_RIGHT,
		'front': CubeMap.SIDE_FRONT,
		'back': CubeMap.SIDE_BACK,
		'top': CubeMap.SIDE_TOP,
		'bottom': CubeMap.SIDE_BOTTOM
	}
	var cube_map = CubeMap.new()
	
	for i in get_children():
		if i.name in images:
			var img = Image.new()
			img.copy_from(i.get_texture().get_data())
			cube_map.set_side(images[i.name], img)
	
	return cube_map
