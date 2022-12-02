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

#include "clippable_mesh.h"
#include "core/math/face3.h"
#include "core/math/plane.h"
#include "core/math/transform_3d.h"
#include "hydro_rigid_body.h"
#include "scene/3d/mesh_instance_3d.h"

#define MAX_RUDDER_FACES 64

ClippableMesh::ClippableMesh() {
	m_model_face_count = 0;
	m_rudder_face_count = 0;
	m_clipped_face_count = 0;
	m_clipped_faces = nullptr;
}

ClippableMesh::~ClippableMesh() {
	if (m_clipped_faces)
		memdelete_arr(m_clipped_faces);
}

void ClippableMesh::load(const MeshInstance3D *mesh) {
	// Pre-allocate all memory used for storing faces:
	//   m_hull_faces: Holds the faces of the un-transformed model, initialized
	//   once here, plus a number of dynamic faces added per-step for the rudders
	//   m_clipped_faces: Holds the faces generated by clipping m_hull_faces to
	//   the plane of the water.  These faces will be converted to global space in
	//   the process.
	//                    In the theoretical worst-case, each face could get split
	//                    into 3 faces (see Face3::split_by_plane()).  We will
	//                    always discard at least one of those 3 faces due to it
	//                    being the one that was created above the water plane, so
	//                    at worst we will generate 2x the number of input faces.
	m_hull_faces = mesh->get_mesh()->get_faces();
	m_model_face_count = m_hull_faces.size();
	m_hull_faces.resize(m_model_face_count + MAX_RUDDER_FACES);

	int new_clipped_face_count = m_hull_faces.size() * 2;
	if (new_clipped_face_count > m_clipped_face_count) {
		if (m_clipped_faces)
			memdelete_arr(m_clipped_faces);
		m_clipped_faces = memnew_arr(Face3, new_clipped_face_count);
		m_clipped_face_count = new_clipped_face_count;
	}

	m_rudder_face_count = 0;
	m_aabb = mesh->get_aabb();
}

void ClippableMesh::add_rudder_faces(const Vector<Face3> &rudder_faces) {
	m_rudder_face_count = rudder_faces.size();
	if (m_rudder_face_count > MAX_RUDDER_FACES) {
		m_rudder_face_count = MAX_RUDDER_FACES;
		WARN_PRINT(
				"Too many rudders defined, you may need to increase MAX_RUDDER_FACES.");
	}
	for (int i = 0; i < m_rudder_face_count; i++) {
		m_hull_faces.write[m_model_face_count + i] = rudder_faces[i];
	}
}

float ClippableMesh::get_volume() const {
	float volume = 0;
	for (int i = 0; i < m_model_face_count; i++) {
		const Face3 &f = m_hull_faces[i];
		volume += f.vertex[0].dot(f.vertex[1].cross(f.vertex[2])) / 6.0f;
	}
	return fabsf(volume);
}

void ClippableMesh::clip_to_plane_quadrant(
		const Vector3 &center, const Plane planes[4],
		const Transform3D &global_transform) {
	m_clipped_face_count = 0;
	int total_faces = m_model_face_count + m_rudder_face_count;

	for (int i = 0; i < total_faces; i++) {
		const Face3 &local_face = m_hull_faces[i];

		Face3 current_face;
		for (int j = 0; j < 3; j++)
			current_face.vertex[j] = global_transform.xform(local_face.vertex[j]);

		int quadrant = get_quadrant(center, current_face.get_median_point());
		const Plane &plane = planes[quadrant];

		bool over[3];
		for (int j = 0; j < 3; j++)
			over[j] = plane.is_point_over(current_face.vertex[j]);

		if (over[0] && over[1] && over[2]) {
			// face is over the plane, discard it
		} else if (!(over[0] || over[1] || over[2])) {
			// Face is under the plane, keep it
			m_clipped_faces[m_clipped_face_count++] = current_face;
		} else {
			// Face crosses the plane, split it
			Face3 split_faces[3];
			bool is_face_over[3];
			int split_face_count =
					current_face.split_by_plane(plane, split_faces, is_face_over);
			for (int j = 0; j < split_face_count; j++) {
				// Only keep the ones under the plane
				if (!is_face_over[j]) {
					m_clipped_faces[m_clipped_face_count++] = split_faces[j];
				}
			}
		}
	}
}
