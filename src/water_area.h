/**************************************************************************/
/*  water_area.h                                                          */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#ifndef WATER_AREA_H
#define WATER_AREA_H

#include "scene/3d/area_3d.h"

class WaterArea3D : public Area3D {
	GDCLASS(WaterArea3D, Area3D)

public:
	WaterArea3D();
	void set_density(float density) { m_density = density; }
	float get_density() { return m_density; }
	void set_viscosity(float viscosity) { m_viscosity = viscosity; }
	float get_viscosity() { return m_viscosity; }
	void set_water_height(float water_height) { m_water_height = water_height; }
	float get_water_height() { return m_water_height; }
	void set_flow_direction(const Vector3 &direction) { m_flow_direction = direction; }
	Vector3 get_flow_direction(const Vector3 &point);

	void update_water_heights(PackedVector3Array &points);

	void _validate_property(PropertyInfo &p_property) const {
	}

protected:
	float m_density;
	float m_viscosity;
	float m_water_height;
	Vector3 m_flow_direction;

	static void _bind_methods();
	void _notification(int p_what);
	void _body_entered(Node *node);
	void _body_exited(Node *node);
};

#endif // WATER_AREA_H
