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

#include "watercraft_propulsion.h"
#include "hydro_rigid_body.h"

WatercraftPropulsion::WatercraftPropulsion() {
	m_value = 0;
}

String WatercraftPropulsion::get_configuration_warning() const {
	if (!Object::cast_to<HydroRigidBody>(get_parent())) {
		return TTR("WatercraftPropulsion serves to provide a propulsion system to a HydroRigidBody. Please use it as a child of a HydroRigidBody.");
	}

	return String();
}

void WatercraftPropulsion::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_origin", "origin"), &WatercraftPropulsion::set_origin);
	ClassDB::bind_method(D_METHOD("get_origin"), &WatercraftPropulsion::get_origin);
	ClassDB::bind_method(D_METHOD("set_direction", "direction"), &WatercraftPropulsion::set_direction);
	ClassDB::bind_method(D_METHOD("get_direction"), &WatercraftPropulsion::get_direction);
	ClassDB::bind_method(D_METHOD("set_value", "value"), &WatercraftPropulsion::set_value);
	ClassDB::bind_method(D_METHOD("get_value"), &WatercraftPropulsion::get_value);

	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "origin"), "set_origin", "get_origin");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "direction"), "set_direction", "get_direction");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "value"), "set_value", "get_value");
}

void WatercraftPropulsion::_notification(int p_what) {

	if (p_what == NOTIFICATION_ENTER_TREE) {

		HydroRigidBody *parent = Object::cast_to<HydroRigidBody>(get_parent());
		if (!parent)
			return;

		parent->m_propulsion.push_back(this);
	}
	if (p_what == NOTIFICATION_EXIT_TREE) {
		HydroRigidBody *parent = Object::cast_to<HydroRigidBody>(get_parent());
		if (!parent)
			return;

		parent->m_propulsion.erase(this);
	}
}
