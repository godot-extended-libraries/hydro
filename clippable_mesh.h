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

#include "core/math/face3.h"
#include "core/math/plane.h"
#include "core/pool_vector.h"
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
