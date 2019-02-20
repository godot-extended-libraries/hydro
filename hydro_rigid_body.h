#ifndef WATERCRAFT_BODY_H
#define WATERCRAFT_BODY_H

#include "clippable_mesh.h"
#include "scene/3d/physics_body.h"

class ImmediateGeometry;
class MeshInstance;

class HydroRigidBody : public RigidBody {

	GDCLASS(HydroRigidBody, RigidBody);

public:
	HydroRigidBody();

	void set_thrust_origin(Vector3 origin) { m_thrust_origin = origin; }
	Vector3 get_thrust_origin() const { return m_thrust_origin; }
	void set_thrust_direction(Vector3 dir) { m_thrust_direction = dir; }
	Vector3 get_thrust_direction() const { return m_thrust_direction; }
	void set_thrust_rotation(float angle) { m_thrust_rotation = angle; }
	float get_thrust_rotation() const { return m_thrust_rotation; }
	void set_thrust_value(float value) { m_thrust_value = value; }
	float get_thrust_value() const { return m_thrust_value; }

protected:
	MeshInstance *m_hull_mesh;
	ImmediateGeometry *m_debug_mesh;
	Vector3 m_thrust_origin;
	Vector3 m_thrust_direction;
	float m_thrust_rotation;
	float m_thrust_value;
	float m_density;

	static void _bind_methods();
	void _notification(int p_what);
	void _direct_state_changed(Object *p_state);
	void update_water_heights(PoolVector3Array &points);

private:
	void draw_debug_face(const Face3 &face, const Transform &transform);
	void draw_debug_mesh(const ClippableMesh &mesh, const Transform &transform);
	void draw_debug_vector(const Vector3 &dir, const Vector3 &origin, const Transform &transform);
};

#endif
