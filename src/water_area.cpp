/*************************************************************************/
/* Copyright (c) 2019 Jon Ring                                           */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#include "water_area.h"
#include "hydro_rigid_body.h"
#include "scene/scene_string_names.h"

WaterArea3D::WaterArea3D() {
	m_density = 1000;
	m_viscosity = 1.0f;
	m_water_height = 0;
}

void WaterArea3D::update_water_heights(PackedVector3Array &points) {
	ScriptInstance *script = get_script_instance();
	if (script && script->has_method("_get_water_heights")) {
		points = get_script_instance()->call("_get_water_heights", points);
	} else {
		for (int i = 0; i < points.size(); i++)
			points.write[i].y = m_water_height;
	}
}

void WaterArea3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_density", "density"),
			&WaterArea3D::set_density);
	ClassDB::bind_method(D_METHOD("get_density"), &WaterArea3D::get_density);
	ClassDB::bind_method(D_METHOD("set_viscosity", "viscosity"),
			&WaterArea3D::set_viscosity);
	ClassDB::bind_method(D_METHOD("get_viscosity"), &WaterArea3D::get_viscosity);
	ClassDB::bind_method(D_METHOD("set_water_height", "water_height"),
			&WaterArea3D::set_water_height);
	ClassDB::bind_method(D_METHOD("get_water_height"),
			&WaterArea3D::get_water_height);
	ClassDB::bind_method(D_METHOD("set_flow_direction", "flow_direction"),
			&WaterArea3D::set_flow_direction);
	ClassDB::bind_method(D_METHOD("get_flow_direction"),
			&WaterArea3D::get_flow_direction);
	ClassDB::bind_method(D_METHOD("_body_entered", "node"),
			&WaterArea3D::_body_entered);
	ClassDB::bind_method(D_METHOD("_body_exited", "node"),
			&WaterArea3D::_body_exited);

	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "density"), "set_density",
			"get_density");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "viscosity"), "set_viscosity",
			"get_viscosity");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "water_height"), "set_water_height",
			"get_water_height");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "flow_direction"),
			"set_flow_direction", "get_flow_direction");
}

void WaterArea3D::_notification(int p_what) {
	if (p_what == NOTIFICATION_READY) {
		connect(SceneStringNames::get_singleton()->body_entered,
				callable_mp(this, &WaterArea3D::_body_entered));
		connect(SceneStringNames::get_singleton()->body_exited,
				callable_mp(this, &WaterArea3D::_body_exited));
	}
}

void WaterArea3D::_body_entered(Node *node) {
	HydroRigidBody *body = Object::cast_to<HydroRigidBody>(node);
	if (body) {
		body->m_water_area = this;
	}
}

void WaterArea3D::_body_exited(Node *node) {
	HydroRigidBody *body = Object::cast_to<HydroRigidBody>(node);
	if (body && body->m_water_area == this) {
		body->m_water_area = nullptr;
	}
}
