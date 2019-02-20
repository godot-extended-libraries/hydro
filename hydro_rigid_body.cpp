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

#include "hydro_rigid_body.h"
#include "clippable_mesh.h"
#include "core/class_db.h"
#include "core/math/aabb.h"
#include "core/math/plane.h"
#include "core/pool_vector.h"
#include "core/os/os.h"
#include "core/variant.h"
#include "scene/3d/immediate_geometry.h"
#include "scene/3d/mesh_instance.h"

HydroRigidBody::HydroRigidBody() :
		RigidBody() {
	m_hull_mesh = nullptr;
	m_debug_mesh = nullptr;
	m_thrust_rotation = 0;
	m_thrust_value = 0;
}

void HydroRigidBody::_bind_methods() {

	ClassDB::bind_method(D_METHOD("set_thrust_origin", "thrust_origin"), &HydroRigidBody::set_thrust_origin);
	ClassDB::bind_method(D_METHOD("get_thrust_origin"), &HydroRigidBody::get_thrust_origin);
	ClassDB::bind_method(D_METHOD("set_thrust_direction", "thrust_direction"), &HydroRigidBody::set_thrust_direction);
	ClassDB::bind_method(D_METHOD("get_thrust_direction"), &HydroRigidBody::get_thrust_direction);
	ClassDB::bind_method(D_METHOD("set_thrust_rotation", "thrust_rotation"), &HydroRigidBody::set_thrust_rotation);
	ClassDB::bind_method(D_METHOD("get_thrust_rotation"), &HydroRigidBody::get_thrust_rotation);
	ClassDB::bind_method(D_METHOD("set_thrust_value", "thrust_value"), &HydroRigidBody::set_thrust_value);
	ClassDB::bind_method(D_METHOD("get_thrust_value"), &HydroRigidBody::get_thrust_value);

	ADD_GROUP("Thrust", "thrust_");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "thrust_origin"), "set_thrust_origin", "get_thrust_origin");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "thrust_direction"), "set_thrust_direction", "get_thrust_direction");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "thrust_rotation"), "set_thrust_rotation", "get_thrust_rotation");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "thrust_value"), "set_thrust_value", "get_thrust_value");
}

void HydroRigidBody::_notification(int p_what) {
	if (p_what == NOTIFICATION_READY) {
		for (int i = 0; i < get_child_count(); i++) {
			if (!m_hull_mesh)
				m_hull_mesh = Object::cast_to<MeshInstance>(get_child(i));
			if (!m_debug_mesh)
				m_debug_mesh = Object::cast_to<ImmediateGeometry>(get_child(i));
		}
		if (m_hull_mesh) {
			ClippableMesh tmp_mesh_data = ClippableMesh(m_hull_mesh);
			m_density = get_mass() / tmp_mesh_data.get_volume();
		} else {
			print_error("HydroRigidBody has no hull mesh!");
		}
	}
}

void HydroRigidBody::_direct_state_changed(Object *p_state) {

	RigidBody::_direct_state_changed(p_state);

	if (!m_hull_mesh)
		return;

	if (m_debug_mesh)
		m_debug_mesh->clear();

	state = Object::cast_to<PhysicsDirectBodyState>(p_state);
	Transform global_transform = get_global_transform();
	Transform local_transform = global_transform.affine_inverse();

	//Generate hull data
	ClippableMesh hull_mesh_data(m_hull_mesh);
	ClippableMesh under_mesh_data(hull_mesh_data);

	if (m_thrust_direction.length_squared() > 0) {
		Vector3 thrustAngle = m_thrust_direction.rotated(Vector3(0, 1, 0), m_thrust_rotation);
		Vector3 rudderBack = m_thrust_origin + thrustAngle;
		Vector3 rudderBottom = m_thrust_origin + Vector3(0, -1, 0);
		under_mesh_data.add_face(m_thrust_origin, rudderBack, rudderBottom, global_transform);
		under_mesh_data.add_face(m_thrust_origin, rudderBottom, rudderBack, global_transform);
	}

	//Generate water planes
	PoolVector3Array wave_samples;
	AABB aabb = global_transform.xform(m_hull_mesh->get_aabb());
	float half_x = aabb.size.x / 2;
	float half_z = aabb.size.z / 2;
	Vector3 wave_center = aabb.get_position() + Vector3(half_x, 0, half_z);
	wave_samples.append(Vector3(wave_center.x, 0, wave_center.z));
	wave_samples.append(Vector3(wave_center.x + half_x, 0, wave_center.z));
	wave_samples.append(Vector3(wave_center.x, 0, wave_center.z + half_z));
	wave_samples.append(Vector3(wave_center.x - half_x, 0, wave_center.z));
	wave_samples.append(Vector3(wave_center.x, 0, wave_center.z - half_z));
	update_water_heights(wave_samples);

	PoolVector<Plane> wave_planes;
	wave_planes.append(Plane(wave_samples[0], wave_samples[1], wave_samples[2]));
	wave_planes.append(Plane(wave_samples[0], wave_samples[2], wave_samples[3]));
	wave_planes.append(Plane(wave_samples[0], wave_samples[3], wave_samples[4]));
	wave_planes.append(Plane(wave_samples[0], wave_samples[4], wave_samples[1]));

	//Clip hull to water planes
	under_mesh_data.clip_to_plane_quadrant(wave_center, wave_planes);

	//are we underwater?
	if (under_mesh_data.face_count() == 0)
		return;

	if (m_debug_mesh) {
		draw_debug_mesh(under_mesh_data, local_transform);
		draw_debug_face(Face3(wave_samples[0], wave_samples[1], wave_samples[2]), local_transform);
		draw_debug_face(Face3(wave_samples[0], wave_samples[2], wave_samples[3]), local_transform);
		draw_debug_face(Face3(wave_samples[0], wave_samples[3], wave_samples[4]), local_transform);
		draw_debug_face(Face3(wave_samples[0], wave_samples[4], wave_samples[1]), local_transform);
	}

	Vector3 origin = get_global_transform().get_origin();

	//Apply thrust
	if (m_thrust_direction.length_squared() > 0) {
		Vector3 start = global_transform.xform(m_thrust_origin);
		int thrust_quadrant = ClippableMesh::get_quadrant(wave_center, start);
		if (!wave_planes[thrust_quadrant].is_point_over(start)) {
			Basis b = global_transform.get_basis();
			Vector3 thrust = b.xform(m_thrust_direction * m_thrust_value).rotated(b.get_axis(1), m_thrust_rotation);
			state->add_force(thrust, start - origin);
			if (m_debug_mesh)
				draw_debug_vector(thrust, start, local_transform);
		}
	}

	float fluidDensity = 1000;
	float gravity = -state->get_total_gravity().length();

	//Calculate buoyancy, drag, and lift per-face
	for (int i = 0; i < under_mesh_data.face_count(); i++) {
		const Face3 &f = under_mesh_data.get_face(i);
		Vector3 center_tri = f.get_median_point();
		Vector3 normal = f.get_plane().normal;
		int q = under_mesh_data.get_quadrant(wave_center, center_tri);

		//Buoyant force
		float depth = fabsf(wave_planes[q].distance_to(center_tri));
		Vector3 buoyancy_force = fluidDensity * gravity * depth * f.get_area() * normal;
		buoyancy_force.x = 0;
		buoyancy_force.z = 0;
		state->add_force(buoyancy_force, center_tri - origin);
		if (m_debug_mesh)
			draw_debug_vector(buoyancy_force, center_tri, local_transform);

		//Drag and lift forces
		Vector3 vel = get_linear_velocity() + get_angular_velocity().cross(center_tri - origin);
		Vector3 vel_dir = vel.normalized();
		float drag_coef = vel_dir.dot(normal);
		if (drag_coef > 0) {
			float area = f.get_area();
			float mag = area * m_density * vel.length_squared();
			Vector3 drag_lift = -vel_dir * drag_coef * mag;

			float c = vel_dir.cross(normal).length();
			float lift_coef = fmin(c, drag_coef) / fmax(c, drag_coef);
			Vector3 lift_dir = vel_dir.cross(normal).cross(vel_dir).normalized();
			drag_lift += -lift_dir * lift_coef * mag;
			state->add_force(drag_lift, center_tri - origin);
			if (m_debug_mesh)
				draw_debug_vector(drag_lift, center_tri, local_transform);
		}
	}
}

void HydroRigidBody::update_water_heights(PoolVector3Array &points) {
	if (get_script_instance()) {
		points = get_script_instance()->call("_get_water_heights", points);
	}
}

void HydroRigidBody::draw_debug_face(const Face3 &face, const Transform &transform) {
	m_debug_mesh->begin(Mesh::PRIMITIVE_LINE_LOOP);
	m_debug_mesh->set_color(Color(0, 1, 1));
	for (int i = 0; i < 3; i++)
		m_debug_mesh->add_vertex(transform.xform(face.vertex[i]));
	m_debug_mesh->end();
}

void HydroRigidBody::draw_debug_mesh(const ClippableMesh &mesh, const Transform &transform) {
	const float scale = 1.001;
	for (int i = 0; i < mesh.face_count(); i++) {
		const Face3 &f = mesh.get_face(i);
		draw_debug_face(f, transform);
		m_debug_mesh->begin(Mesh::PRIMITIVE_LINE_LOOP);
		m_debug_mesh->set_color(Color(1, 1, 0));
		for (int j = 0; j < 3; j++)
			m_debug_mesh->add_vertex(transform.xform(f.vertex[j]) * scale);
		m_debug_mesh->end();
	}
}

void HydroRigidBody::draw_debug_vector(const Vector3 &dir, const Vector3 &origin, const Transform &transform) {
	m_debug_mesh->begin(Mesh::PRIMITIVE_LINES);
	m_debug_mesh->set_color(Color(1, 0, 1));
	m_debug_mesh->add_vertex(transform.xform(origin));
	m_debug_mesh->add_vertex(transform.xform(origin + dir * 50 / get_mass()));
	m_debug_mesh->end();
}
