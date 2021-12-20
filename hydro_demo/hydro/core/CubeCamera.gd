@tool
extends Node3D

func _ready():
	for i in get_children():
		i.size = Vector2(256, 256)
		i.own_world = true

func update_cube_map():
	var images = {
		'left': Cubemap.SIDE_LEFT,
		'right': Cubemap.SIDE_RIGHT,
		'front': Cubemap.SIDE_FRONT,
		'back': Cubemap.SIDE_BACK,
		'top': Cubemap.SIDE_TOP,
		'bottom': Cubemap.SIDE_BOTTOM
	}
	var cube_map = Cubemap.new()
	
	for i in get_children():
		if i.name in images:
			var img = Image.new()
			img.copy_from(i.get_texture().get_data())
			cube_map.set_side(images[i.name], img)
	
	return cube_map
