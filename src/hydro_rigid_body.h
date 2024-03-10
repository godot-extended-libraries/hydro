/**************************************************************************/
/*  hydro_rigid_body.h                                                    */
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

#ifndef WATERCRAFT_BODY_H
#define WATERCRAFT_BODY_H

#include "clippable_mesh.h"
#include "scene/3d/physics/physics_body_3d.h"
#include "scene/3d/physics/rigid_body_3d.h"

class ImmediateMesh;
class MeshInstance;
class WaterArea3D;
class WatercraftBallast;
class WatercraftPropulsion;
class WatercraftRudder;

class HydroRigidBody : public RigidBody3D {
	GDCLASS(HydroRigidBody, RigidBody3D)

public:
	HydroRigidBody();

protected:
	NodePath m_hull_path;
	ClippableMesh m_hull_mesh;
	Ref<ImmediateMesh> m_debug_mesh;
	Vector3 m_thrust_origin;
	Vector3 m_thrust_direction;
	float m_thrust_rotation;
	float m_thrust_value;
	float m_density;
	float m_volume;
	WaterArea3D *m_water_area;

	Vector<WatercraftBallast *> m_ballast;
	Vector<WatercraftPropulsion *> m_propulsion;
	Vector<WatercraftRudder *> m_rudders;

	static void _bind_methods();
	void _notification(int p_what);
	void _body_state_changed(PhysicsDirectBodyState3D *p_state) override;

private:
	void update_hull();
	void draw_debug_face(const Face3 &face, const Transform3D &transform);
	void draw_debug_mesh(const ClippableMesh &mesh, const Transform3D &transform);
	void draw_debug_vector(const Vector3 &dir, const Vector3 &origin,
			const Transform3D &transform);

	friend class WaterArea3D;
	friend class WatercraftBallast;
	friend class WatercraftPropulsion;
	friend class WatercraftRudder;
};

#endif
