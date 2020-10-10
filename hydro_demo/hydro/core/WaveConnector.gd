extends WaterArea

onready var ocean = $"../Ocean"

func _get_water_heights(positions):
	var ret : PoolVector3Array
	for p in positions:
		ret.append(ocean.get_displace(p))
	return ret
