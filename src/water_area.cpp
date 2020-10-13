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

WaterArea::WaterArea() {
	m_density = 1000;
	m_water_height = 0;
}

void WaterArea::update_water_heights(PoolVector3Array &points) {
	ScriptInstance *script = get_script_instance();
	if (script && script->has_method("_get_water_heights")) {
		points = get_script_instance()->call("_get_water_heights", points);
	} else {
		PoolVector3Array::Write write_points = points.write();
		for (int i = 0; i < points.size(); i++)
			write_points[i].y = m_water_height;
	}
}

void WaterArea::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_density", "density"), &WaterArea::set_density);
	ClassDB::bind_method(D_METHOD("get_density"), &WaterArea::get_density);
	ClassDB::bind_method(D_METHOD("set_water_height", "water_height"), &WaterArea::set_water_height);
	ClassDB::bind_method(D_METHOD("get_water_height"), &WaterArea::get_water_height);
	ClassDB::bind_method(D_METHOD("set_flow_direction", "flow_direction"), &WaterArea::set_flow_direction);
	ClassDB::bind_method(D_METHOD("get_flow_direction"), &WaterArea::get_flow_direction);
	ClassDB::bind_method(D_METHOD("_body_entered", "node"), &WaterArea::_body_entered);
	ClassDB::bind_method(D_METHOD("_body_exited", "node"), &WaterArea::_body_exited);

	ADD_PROPERTY(PropertyInfo(Variant::REAL, "density"), "set_density", "get_density");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "water_height"), "set_water_height", "get_water_height");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "flow_direction"), "set_flow_direction", "get_flow_direction");
}

void WaterArea::_notification(int p_what) {
	if (p_what == NOTIFICATION_READY) {
		connect(SceneStringNames::get_singleton()->body_entered, this, "_body_entered");
		connect(SceneStringNames::get_singleton()->body_exited, this, "_body_exited");
	}
}

void WaterArea::_body_entered(Node *node) {
	HydroRigidBody *body = Object::cast_to<HydroRigidBody>(node);
	if (body) {
		body->m_water_area = this;
	}
}

void WaterArea::_body_exited(Node *node) {
	HydroRigidBody *body = Object::cast_to<HydroRigidBody>(node);
	if (body && body->m_water_area == this) {
		body->m_water_area = nullptr;
	}
}
