@tool
extends Node3D


func _ready():
	for i in get_children():
		i.size = Vector2(256, 256)
		i.own_world_3d = true


func update_cube_map():
	var images = {
		"left": RenderingServer.CUBEMAP_LAYER_LEFT,
		"right": RenderingServer.CUBEMAP_LAYER_RIGHT,
		"front": RenderingServer.CUBEMAP_LAYER_FRONT,
		"back": RenderingServer.CUBEMAP_LAYER_BACK,
		"top": RenderingServer.CUBEMAP_LAYER_TOP,
		"bottom": RenderingServer.CUBEMAP_LAYER_BOTTOM
	}
	var cube_map = Cubemap.new()

	var layers = [].duplicate()
	for i in get_children():
		if i.name in images:
			var img = Image.new()
			img.copy_from(i.get_texture().get_image())
			layers.push_back(img)
	cube_map.create_from_images(layers)
	return cube_map
