#include "clippable_mesh.h"
#include "core/dvector.h"
#include "core/math/face3.h"
#include "core/math/plane.h"
#include "core/math/transform.h"
#include "hydro_rigid_body.h"
#include "scene/3d/mesh_instance.h"

ClippableMesh::ClippableMesh() {
	m_face_count = 0;
}

ClippableMesh::ClippableMesh(const MeshInstance *mesh) {
	Transform transform = mesh->get_global_transform();
	PoolVector<Face3> mesh_faces = mesh->get_mesh()->get_faces();
	m_face_count = mesh_faces.size();
	m_faces.resize(m_face_count);
	PoolVector<Face3>::Read read_faces = mesh_faces.read();
	PoolVector<Face3>::Write write_faces = m_faces.write();

	for (int i = 0; i < m_face_count; i++) {
		//Save face as global
		const Face3 &local_face = read_faces[i];
		Face3 &global_face = write_faces[i];
		for (int j = 0; j < 3; j++)
			global_face.vertex[j] = transform.xform(local_face.vertex[j]);
	}
}

void ClippableMesh::add_face(const Vector3 &a, const Vector3 &b, const Vector3 &c, const Transform &transform) {
	Face3 global_face(transform.xform(a), transform.xform(b), transform.xform(c));
	m_faces.append(global_face);
	++m_face_count;
}

float ClippableMesh::get_volume() const {
	float volume = 0;
	PoolVector<Face3>::Read read_faces = m_faces.read();
	for (int i = 0; i < m_face_count; i++) {
		const Face3 &f = read_faces[i];
		volume += f.vertex[0].dot(f.vertex[1].cross(f.vertex[2])) / 6.0f;
	}
	return fabsf(volume);
}

void ClippableMesh::clip_to_plane_quadrant(const Vector3 &center, const PoolVector<Plane> &planes) {
	PoolVector<Face3> new_faces;
	PoolVector<Plane>::Read read_planes = planes.read();
	PoolVector<Face3>::Read read_faces = m_faces.read();

	for (int i = 0; i < m_face_count; i++) {
		const Face3 &f = read_faces[i];
		if (f.is_degenerate())
			continue;

		int quadrant = get_quadrant(center, f.get_median_point());

		if (quadrant < 0) {
			new_faces.append(f);
			continue;
		}
		const Plane &plane = read_planes[quadrant];

		bool over[3];
		for (int i = 0; i < 3; i++)
			over[i] = plane.is_point_over(f.vertex[i]);

		if (over[0] && over[1] && over[2]) {
			//face is over the plane, discard it
		} else if (!(over[0] || over[1] || over[2])) {
			//Face is under the plane, keep it
			new_faces.append(f);
		} else {
			//Face crosses the plane, split it
			Face3 split_faces[3];
			bool is_face_over[3];
			int split_face_count = f.split_by_plane(plane, split_faces, is_face_over);
			for (int j = 0; j < split_face_count; j++) {
				//Only keep the ones under the plane
				if (!is_face_over[j]) {
					new_faces.append(split_faces[j]);
				}
			}
		}
	}
	m_faces = new_faces;
	m_face_count = m_faces.size();
}

void ClippableMesh::clip_to_plane(const Plane &plane) {
	PoolVector<Face3> new_faces;
	PoolVector<Face3>::Read read_faces = m_faces.read();
	for (int i = 0; i < m_face_count; i++) {
		const Face3 &f = read_faces[i];
		if (f.is_degenerate())
			continue;

		bool over[3];
		for (int i = 0; i < 3; i++)
			over[i] = plane.is_point_over(f.vertex[i]);

		if (over[0] && over[1] && over[2]) {
			//face is over the plane, discard it
		} else if (!(over[0] || over[1] || over[2])) {
			//Face is under the plane, keep it
			new_faces.append(f);
		} else {
			//Face crosses the plane, split it
			Face3 split_faces[3];
			bool is_face_over[3];
			int split_face_count = f.split_by_plane(plane, split_faces, is_face_over);
			for (int j = 0; j < split_face_count; j++) {
				//Only keep the ones under the plane
				if (!is_face_over[j])
					new_faces.append(split_faces[j]);
			}
		}
	}
	m_faces = new_faces;
	m_face_count = m_faces.size();
}
