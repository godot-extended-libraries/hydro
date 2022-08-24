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
#include "core/math/vector3.h"
#include "core/string/print_string.h"
#include "scene/3d/physics_body_3d.h"
#include "water_area.h"
#include "watercraft_ballast.h"
#include "watercraft_propulsion.h"
#include "watercraft_rudder.h"

#include "core/math/aabb.h"
#include "core/math/plane.h"
#include "core/object/class_db.h"
#include "core/os/os.h"
#include "core/variant/variant.h"
#include "core/version.h"
#include "scene/3d/mesh_instance_3d.h"
#include "scene/resources/immediate_mesh.h"

HydroRigidDynamicBody::HydroRigidDynamicBody() :
		RigidDynamicBody3D() {
	m_water_area = nullptr;
	m_volume = 0;
	m_density = 0;
}

void HydroRigidDynamicBody::_bind_methods() {}

void HydroRigidDynamicBody::_notification(int p_what) {
	if (p_what == NOTIFICATION_READY) {
		for (int i = 0; i < get_child_count(); i++) {
			if (m_hull_mesh.is_empty()) {
				MeshInstance3D *mesh = Object::cast_to<MeshInstance3D>(get_child(i));
				if (mesh) {
					m_hull_mesh.load(mesh);
					m_density = get_mass() / m_hull_mesh.get_volume();
				}
			}
			if (m_debug_mesh.is_valid()) {
				continue;
			}
			MeshInstance3D *mesh_instance = Object::cast_to<MeshInstance3D>(get_child(i));
			if (!mesh_instance) {
				continue;
			}
			Ref<Mesh> immediate_mesh = mesh_instance->get_mesh();
			if (immediate_mesh.is_valid() && immediate_mesh->is_class("ImmediateMesh")) {
				m_debug_mesh = immediate_mesh;
			}
		}
		if (m_hull_mesh.is_empty()) {
			print_error("HydroRigidDynamicBody has no hull mesh!");
		}
	}
}

void HydroRigidDynamicBody::_body_state_changed(PhysicsDirectBodyState3D *p_state) {
	if (m_debug_mesh.is_valid()) {
		m_debug_mesh->clear_surfaces();
	}

	Transform3D global_transform = get_global_transform();
	Transform3D local_transform = global_transform.affine_inverse();
	Vector3 origin = global_transform.get_origin();

	// Apply ballast weight
	for (int i = 0; i < m_ballast.size(); i++) {
		WatercraftBallast *ballast = m_ballast[i];
		Vector3 start = global_transform.xform(ballast->m_origin);
		Vector3 gravity = p_state->get_total_gravity();
		Vector3 weight = gravity * (ballast->m_mass);
		p_state->apply_force(weight, start - origin);
		if (m_debug_mesh.is_valid()) {
			draw_debug_vector(weight, start, local_transform);
		}
	}

	// Shortcut out if we aren't in the water
	if (!m_water_area || m_hull_mesh.is_empty()) {
		RigidDynamicBody3D::_body_state_changed(p_state);
		return;
	}

	// Add rudders
	Vector<Face3> rudder_faces;
	for (int i = 0; i < m_rudders.size(); i++) {
		WatercraftRudder *rudder = m_rudders[i];
		rudder_faces.append_array(rudder->get_faces());
	}
	m_hull_mesh.add_rudder_faces(rudder_faces);

	// Generate water planes
	AABB aabb = global_transform.xform(m_hull_mesh.get_aabb());
	float half_x = aabb.size.x / 2;
	float half_z = aabb.size.z / 2;
	Vector3 wave_center = aabb.get_position() + Vector3(half_x, 0, half_z);

	PackedVector3Array wave_samples;
	wave_samples.resize(5);
	{
		wave_samples.write[0] = Vector3(wave_center.x, 0, wave_center.z);
		wave_samples.write[1] = Vector3(wave_center.x + half_x, 0, wave_center.z);
		wave_samples.write[2] = Vector3(wave_center.x, 0, wave_center.z + half_z);
		wave_samples.write[3] = Vector3(wave_center.x - half_x, 0, wave_center.z);
		wave_samples.write[4] = Vector3(wave_center.x, 0, wave_center.z - half_z);
	}
	m_water_area->update_water_heights(wave_samples);

	Plane wave_planes[4];
	wave_planes[0] = Plane(wave_samples[0], wave_samples[1], wave_samples[2]);
	wave_planes[1] = Plane(wave_samples[0], wave_samples[2], wave_samples[3]);
	wave_planes[2] = Plane(wave_samples[0], wave_samples[3], wave_samples[4]);
	wave_planes[3] = Plane(wave_samples[0], wave_samples[4], wave_samples[1]);

	// Clip hull to water planes
	m_hull_mesh.clip_to_plane_quadrant(wave_center, wave_planes,
			global_transform);

	// are we underwater?
	if (m_hull_mesh.clipped_face_count() == 0) {
		RigidDynamicBody3D::_body_state_changed(p_state);
		return;
	}

	if (m_debug_mesh.is_valid()) {
		draw_debug_mesh(m_hull_mesh, local_transform);
		draw_debug_face(Face3(wave_samples[0], wave_samples[1], wave_samples[2]),
				local_transform);
		draw_debug_face(Face3(wave_samples[0], wave_samples[2], wave_samples[3]),
				local_transform);
		draw_debug_face(Face3(wave_samples[0], wave_samples[3], wave_samples[4]),
				local_transform);
		draw_debug_face(Face3(wave_samples[0], wave_samples[4], wave_samples[1]),
				local_transform);
	}

	// Apply propulsion
	for (int i = 0; i < m_propulsion.size(); i++) {
		WatercraftPropulsion *prop = m_propulsion[i];
		if (prop->m_direction.length_squared() > 0 &&
				Math::absf(prop->m_value) > CMP_EPSILON) {
			Vector3 start = global_transform.xform(prop->m_origin);
			int prop_quadrant = ClippableMesh::get_quadrant(wave_center, start);
			if (!wave_planes[prop_quadrant].is_point_over(start)) {
				Basis b = global_transform.get_basis();
				Vector3 thrust = b.xform(prop->m_direction * prop->m_value);
				p_state->apply_force(thrust, start - origin);
				if (m_debug_mesh.is_valid()) {
					draw_debug_vector(thrust, start, local_transform);
				}
			}
		}
	}

	float fluid_density = m_water_area->get_density();
	float fluid_viscosity = m_water_area->get_viscosity();
	float gravity = -p_state->get_total_gravity().length();

	// Calculate buoyancy, drag, and lift per-face
	Vector3 base_velocity =
			get_linear_velocity() - m_water_area->get_flow_direction();
	for (int i = 0; i < m_hull_mesh.clipped_face_count(); i++) {
		const Face3 &f = m_hull_mesh.get_clipped_face(i);
		Vector3 center_tri = f.get_median_point();
		Vector3 normal = f.get_plane().normal;
		float area = f.get_area();
		int q = m_hull_mesh.get_quadrant(wave_center, center_tri);

		// Buoyant force
		float depth = fabsf(wave_planes[q].distance_to(center_tri));
		Vector3 buoyancy_force = fluid_density * gravity * depth * area * normal;
		buoyancy_force.x = 0;
		buoyancy_force.z = 0;
		p_state->apply_force(buoyancy_force, center_tri - origin);
		if (m_debug_mesh.is_valid()) {
			draw_debug_vector(buoyancy_force, center_tri, local_transform);
		}

		// Drag and lift forces
		Vector3 vel =
				base_velocity + get_angular_velocity().cross(center_tri - origin);
		Vector3 vel_dir = vel.normalized();
		float drag_coef = vel_dir.dot(normal);
		if (drag_coef > 0) {
			float mag = area * fluid_density * fluid_viscosity * vel.length_squared();
			Vector3 drag_lift = -vel_dir * drag_coef * mag;

			float c = vel_dir.cross(normal).length();
			float lift_coef = fmin(c, drag_coef) / fmax(c, drag_coef);
			Vector3 lift_dir = vel_dir.cross(normal).cross(vel_dir).normalized();
			drag_lift += -lift_dir * lift_coef * mag;
			p_state->apply_force(drag_lift, center_tri - origin);
			if (m_debug_mesh.is_valid()) {
				draw_debug_vector(drag_lift, center_tri, local_transform);
			}
		}
	}
	RigidDynamicBody3D::_body_state_changed(p_state);
}

void HydroRigidDynamicBody::draw_debug_face(const Face3 &face,
		const Transform3D &transform) {
	// TODO: fire 2021-19-12
	// m_debug_mesh->surface_begin(Mesh::PRIMITIVE_LINE_LOOP);
	// m_debug_mesh->surface_set_color(Color(0, 1, 1));
	// for (int i = 0; i < 3; i++) {
	// 	m_debug_mesh->surface_add_vertex(transform.xform(face.vertex[i]));
	// }
	// m_debug_mesh->surface_end();
}

void HydroRigidDynamicBody::draw_debug_mesh(const ClippableMesh &mesh,
		const Transform3D &transform) {
	// TODO: fire 2021-19-12
	// const float scale = 1.001;
	// for (int i = 0; i < mesh.clipped_face_count(); i++) {
	// 	const Face3 &f = mesh.get_clipped_face(i);
	// 	draw_debug_face(f, transform);
	// 	m_debug_mesh->surface_begin(Mesh::PRIMITIVE_LINE_LOOP);
	// 	m_debug_mesh->surface_set_color(Color(1, 1, 0));
	// 	for (int j = 0; j < 3; j++)
	// 		m_debug_mesh->surface_add_vertex(transform.xform(f.vertex[j]) * scale);
	// 	m_debug_mesh->surface_end();
	// }
}

void HydroRigidDynamicBody::draw_debug_vector(const Vector3 &dir,
		const Vector3 &origin,
		const Transform3D &transform) {
	m_debug_mesh->surface_begin(Mesh::PRIMITIVE_LINES);
	m_debug_mesh->surface_set_color(Color(1, 0, 1));
	m_debug_mesh->surface_add_vertex(transform.xform(origin));
	m_debug_mesh->surface_add_vertex(transform.xform(origin + dir * 50 / get_mass()));
	m_debug_mesh->surface_end();
}
