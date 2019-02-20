#include "register_types.h"
#include "core/class_db.h"
#include "hydro_rigid_body.h"

void register_hydro_types() {
	ClassDB::register_class<HydroRigidBody>();
}

void unregister_hydro_types() {
}
