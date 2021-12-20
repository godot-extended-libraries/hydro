@tool
extends MeshInstance3D

func _process(delta):
	var ocean = get_parent().get_node('Ocean')
	
	position = ocean.get_displace(Vector2())
