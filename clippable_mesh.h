#ifndef CLIPPABLEMESH_H
#define CLIPPABLEMESH_H

#include "core/dvector.h"
#include "core/math/face3.h"
#include "core/math/plane.h"
#include "core/math/vector3.h"

class MeshInstance;
class Transform;

class ClippableMesh {
public:
	ClippableMesh();
	ClippableMesh(const MeshInstance *mesh);

	void add_face(const Vector3 &a, const Vector3 &b, const Vector3 &c, const Transform &transform);
	int face_count() const { return m_face_count; }
	const Face3 &get_face(int index) const { return m_faces.read()[index]; }
	float get_volume() const;

	void clip_to_plane(const Plane &plane);
	void clip_to_plane_quadrant(const Vector3 &center, const PoolVector<Plane> &planes);

	static int get_quadrant(const Vector3 &center, const Vector3 &point) {
		if (point.z > center.z)
			return point.x > center.x ? 0 : 1;
		else
			return point.x < center.x ? 2 : 3;
	}

private:
	PoolVector<Face3> m_faces;
	int m_face_count;
};

#endif
