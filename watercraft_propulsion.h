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

#ifndef WATERCRAFT_PROPULSION_H
#define WATERCRAFT_PROPULSION_H

#include "scene/3d/spatial.h"

class WatercraftPropulsion : public Spatial {
	GDCLASS(WatercraftPropulsion, Spatial)

public:
	WatercraftPropulsion();
	void set_origin(Vector3 origin) { m_origin = origin; update_gizmo(); }
	Vector3 get_origin() const { return m_origin; }
	void set_direction(Vector3 dir) { m_direction = dir; update_gizmo(); }
	Vector3 get_direction() const { return m_direction; }
	void set_value(float value) { m_value = value; }
	float get_value() const { return m_value; }
	String get_configuration_warning() const;

protected:
	Vector3 m_origin;
	Vector3 m_direction;
	float m_value;

	static void _bind_methods();
	void _notification(int p_what);

	friend class HydroRigidBody;
};

#endif // WATERCRAFT_PROPULSION_H
