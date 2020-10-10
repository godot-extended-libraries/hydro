tool
extends MeshInstance

func _process(delta):
	var ocean = get_parent().get_node('Ocean')
	
	translation = ocean.get_displace(Vector2())
