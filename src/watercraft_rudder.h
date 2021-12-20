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

#ifndef WATERCRAFT_RUDDER_H
#define WATERCRAFT_RUDDER_H

#include "scene/3d/node_3d.h"

class WatercraftRudder : public Node3D {
	GDCLASS(WatercraftRudder, Node3D)

public:
	WatercraftRudder();
	void set_origin(Vector3 origin) {
		m_origin = origin;
		update_gizmos();
	}
	Vector3 get_origin() const { return m_origin; }
	void set_direction(Vector3 dir) {
		m_direction = dir;
		update_gizmos();
	}
	Vector3 get_direction() const { return m_direction; }
	void set_length(float value) {
		m_length = value;
		update_gizmos();
	}
	float get_length() const { return m_length; }
	void set_depth(float value) {
		m_depth = value;
		update_gizmos();
	}
	float get_depth() const { return m_depth; }

	Vector<Face3> get_faces();
	String get_configuration_warning() const;

protected:
	Vector3 m_origin;
	Vector3 m_direction;
	float m_length;
	float m_depth;

	static void _bind_methods();
	void _notification(int p_what);

	friend class HydroRigidDynamicBody;
};

#endif // WATERCRAFT_RUDDER_H
