extends WaterArea3D

@onready var ocean = $"../Ocean"

func _get_water_heights(positions):
	var ret : PackedVector3Array
	for p in positions:
		ret.append(ocean.get_displace(p))
	return ret
