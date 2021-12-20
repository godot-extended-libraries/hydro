def can_build(env, platform):
	return True

def configure(env):
	pass

def get_doc_classes():
	return [
		"HydroRigidBody",
		"WaterArea3D",
		"WatercraftBallast",
		"WatercraftPropulsion",
		"WatercraftRudder",
	]

def get_doc_path():
	return "doc_classes"
