extends WaterArea3D

@onready var ocean = $"../Ocean"

func _get_water_heights(p_positions):
	var ret : PackedVector3Array = PackedVector3Array()
	ret.resize(p_positions.size())
	for p in p_positions.size():
		ret[p] = ocean.get_displace(p_positions[p])
	return ret
