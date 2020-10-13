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
#include "water_area.h"
#include "watercraft_ballast.h"
#include "watercraft_propulsion.h"
#include "watercraft_rudder.h"

#include "core/class_db.h"
#include "core/math/aabb.h"
#include "core/math/plane.h"
#include "core/os/os.h"
#include "core/pool_vector.h"
#include "core/variant.h"
#include "scene/3d/immediate_geometry.h"
#include "scene/3d/mesh_instance.h"

HydroRigidBody::HydroRigidBody() :
		RigidBody() {
	m_hull_mesh = nullptr;
	m_debug_mesh = nullptr;
	m_water_area = nullptr;
	m_volume = 0;
	m_density = 0;
}

void HydroRigidBody::_bind_methods() {
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

	if (m_debug_mesh)
		m_debug_mesh->clear();

	RigidBody::_direct_state_changed(p_state);
	state = Object::cast_to<PhysicsDirectBodyState>(p_state);

	Vector3 origin = get_global_transform().get_origin();
	Transform global_transform = get_global_transform();
	Transform local_transform = global_transform.affine_inverse();

	//Apply ballast weight
	for (int i = 0; i < m_ballast.size(); i++) {
		WatercraftBallast *ballast = m_ballast[i];
		Vector3 start = global_transform.xform(ballast->m_origin);
		Vector3 weight = state->get_total_gravity() * ballast->m_mass;
		state->add_force(weight, start - origin);
		if (m_debug_mesh)
			draw_debug_vector(weight, start, local_transform);
	}

	//Shortcut out if we aren't in the water
	if (!m_water_area || !m_hull_mesh)
		return;

	//Generate hull data
	ClippableMesh hull_mesh_data(m_hull_mesh);
	ClippableMesh under_mesh_data(hull_mesh_data);

	//Add rudders
	for (int i = 0; i < m_rudders.size(); i++) {
		WatercraftRudder *rudder = m_rudders[i];
		PoolVector<Face3> faces = rudder->get_faces();
		for (int j = 0; j < faces.size(); j++)
			under_mesh_data.add_face(faces[j], global_transform);
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
	m_water_area->update_water_heights(wave_samples);

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

	//Apply propulsion
	for (int i = 0; i < m_propulsion.size(); i++) {
		WatercraftPropulsion *prop = m_propulsion[i];
		if (prop->m_direction.length_squared() > 0 && Math::absf(prop->m_value) > CMP_EPSILON) {
			Vector3 start = global_transform.xform(prop->m_origin);
			int prop_quadrant = ClippableMesh::get_quadrant(wave_center, start);
			if (!wave_planes[prop_quadrant].is_point_over(start)) {
				Basis b = global_transform.get_basis();
				Vector3 thrust = b.xform(prop->m_direction * prop->m_value);
				state->add_force(thrust, start - origin);
				if (m_debug_mesh)
					draw_debug_vector(thrust, start, local_transform);
			}
		}
	}

	float fluid_density = m_water_area->get_density();
	float gravity = -state->get_total_gravity().length();

	//Calculate buoyancy, drag, and lift per-face
	Vector3 base_velocity = get_linear_velocity() - m_water_area->get_flow_direction();
	for (int i = 0; i < under_mesh_data.face_count(); i++) {
		const Face3 &f = under_mesh_data.get_face(i);
		Vector3 center_tri = f.get_median_point();
		Vector3 normal = f.get_plane().normal;
		int q = under_mesh_data.get_quadrant(wave_center, center_tri);

		//Buoyant force
		float depth = fabsf(wave_planes[q].distance_to(center_tri));
		Vector3 buoyancy_force = fluid_density * gravity * depth * f.get_area() * normal;
		buoyancy_force.x = 0;
		buoyancy_force.z = 0;
		state->add_force(buoyancy_force, center_tri - origin);
		if (m_debug_mesh)
			draw_debug_vector(buoyancy_force, center_tri, local_transform);

		//Drag and lift forces
		Vector3 vel = base_velocity + get_angular_velocity().cross(center_tri - origin);
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
