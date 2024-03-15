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

#ifndef CLIPPABLEMESH_H
#define CLIPPABLEMESH_H

#include "core/math/aabb.h"
#include "core/math/face3.h"
#include "core/math/plane.h"
#include "core/math/vector3.h"
#include "scene/3d/mesh_instance_3d.h"

class MeshInstance;
class Transform;

class ClippableMesh {
public:
	ClippableMesh();
	~ClippableMesh();

	void load(const MeshInstance3D *mesh);
	void add_rudder_faces(const Vector<Face3> &rudder_faces);
	int clipped_face_count() const { return m_clipped_face_count; }
	const Face3 &get_clipped_face(int index) const {
		return m_clipped_faces[index];
	}
	float get_volume() const;
	AABB get_aabb() const { return m_aabb; }
	bool is_empty() const { return m_model_face_count == 0; }

	void clip_to_plane_quadrant(const Vector3 &center, const Plane planes[4],
			const Transform3D &global_transform);

	static int get_quadrant(const Vector3 &center, const Vector3 &point) {
		if (point.z > center.z)
			return point.x > center.x ? 0 : 1;
		else
			return point.x < center.x ? 2 : 3;
	}

private:
	Vector<Face3> m_hull_faces;
	Face3 *m_clipped_faces;
	int m_model_face_count;
	int m_rudder_face_count;
	int m_clipped_face_count;
	AABB m_aabb;
};

#endif
