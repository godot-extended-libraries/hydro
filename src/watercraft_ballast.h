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

#ifndef WATERCRAFT_BALLAST_H
#define WATERCRAFT_BALLAST_H

#include "scene/3d/node_3d.h"

class WatercraftBallast : public Node3D {
	GDCLASS(WatercraftBallast, Node3D)

public:
	WatercraftBallast();
	void set_origin(Vector3 origin) { m_origin = origin; }
	Vector3 get_origin() const { return m_origin; }
	void set_mass(float mass) { m_mass = mass; }
	float get_mass() { return m_mass; }
	String get_configuration_warning() const;

protected:
	Vector3 m_origin;
	float m_mass;

	static void _bind_methods();
	void _notification(int p_what);

	friend class HydroRigidDynamicBody;
};

#endif // WATERCRAFT_BALLAST_H
