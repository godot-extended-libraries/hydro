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


#include "watercraft_rudder.h"
#include "hydro_rigid_body.h"

WatercraftRudder::WatercraftRudder() {
	m_length = 1.f;
	m_depth = 1.f;
}

String WatercraftRudder::get_configuration_warning() const {
	if (!Object::cast_to<HydroRigidBody>(get_parent())) {
		return RTR("WatercraftRudder serves to provide a rudder for a "
				   "HydroRigidBody. Please use it as a child of a "
				   "HydroRigidBody.");
	}

	return String();
}

void WatercraftRudder::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_origin", "origin"),
			&WatercraftRudder::set_origin);
	ClassDB::bind_method(D_METHOD("get_origin"), &WatercraftRudder::get_origin);
	ClassDB::bind_method(D_METHOD("set_direction", "direction"),
			&WatercraftRudder::set_direction);
	ClassDB::bind_method(D_METHOD("get_direction"),
			&WatercraftRudder::get_direction);
	ClassDB::bind_method(D_METHOD("set_length", "length"),
			&WatercraftRudder::set_length);
	ClassDB::bind_method(D_METHOD("get_length"), &WatercraftRudder::get_length);
	ClassDB::bind_method(D_METHOD("set_depth", "length"),
			&WatercraftRudder::set_depth);
	ClassDB::bind_method(D_METHOD("get_depth"), &WatercraftRudder::get_depth);

	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "origin"), "set_origin",
			"get_origin");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "direction"), "set_direction",
			"get_direction");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "length"), "set_length",
			"get_length");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "depth"), "set_depth", "get_depth");
}

Vector<Face3> WatercraftRudder::get_faces() {
	Vector3 depth = Vector3(0, m_depth, 0);
	Vector3 p1 = m_origin;
	Vector3 p2 = p1 + m_direction * m_length;
	Vector3 p3 = p1 - depth;
	Vector3 p4 = p2 - depth;

	Vector<Face3> faces;
	faces.append(Face3(p1, p2, p3));
	faces.append(Face3(p2, p4, p3));
	faces.append(Face3(p3, p2, p1));
	faces.append(Face3(p3, p4, p2));

	return faces;
}

void WatercraftRudder::_notification(int p_what) {
	HydroRigidBody *parent =
			Object::cast_to<HydroRigidBody>(get_parent());
	if (!parent)
		return;

	if (p_what == NOTIFICATION_ENTER_TREE) {
		parent->m_rudders.push_back(this);
	} else if (p_what == NOTIFICATION_EXIT_TREE) {
		parent->m_rudders.erase(this);
	}
}
