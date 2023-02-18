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

#include "editor/editor_node.h"
#include "editor/editor_settings.h"

#include "hydro_gizmo_plugins.h"
#include "watercraft_ballast.h"
#include "watercraft_propulsion.h"
#include "watercraft_rudder.h"

WatercraftBallastNode3DGizmoPlugin::WatercraftBallastNode3DGizmoPlugin() {
	Color gizmo_color =
			EDITOR_DEF("editors/3d_gizmos/gizmo_colors/shape", Color(0.5, 0.7, 1));
	create_material("shape_material", gizmo_color);
}

bool WatercraftBallastNode3DGizmoPlugin::has_gizmo(Node3D *p_spatial) {
	return Object::cast_to<WatercraftBallast>(p_spatial) != nullptr;
}

String WatercraftBallastNode3DGizmoPlugin::get_gizmo_name() const {
	return "WatercraftBallast";
}

int WatercraftBallastNode3DGizmoPlugin::get_priority() const {
	return -1;
}

void WatercraftBallastNode3DGizmoPlugin::redraw(EditorNode3DGizmo *p_gizmo) {
	WatercraftBallast *ballast =
			Object::cast_to<WatercraftBallast>(p_gizmo->get_node_3d());
	Vector3 origin = ballast->get_origin();

	const real_t top_offset = 0.15f;
	const real_t bottom_offset = 0.25f;
	const real_t height = 0.1f;
	Vector3 top1 = origin + Vector3(top_offset, height, top_offset);
	Vector3 top2 = origin + Vector3(top_offset, height, -top_offset);
	Vector3 top3 = origin + Vector3(-top_offset, height, -top_offset);
	Vector3 top4 = origin + Vector3(-top_offset, height, top_offset);

	Vector3 bottom1 = origin + Vector3(bottom_offset, -height, bottom_offset);
	Vector3 bottom2 = origin + Vector3(bottom_offset, -height, -bottom_offset);
	Vector3 bottom3 = origin + Vector3(-bottom_offset, -height, -bottom_offset);
	Vector3 bottom4 = origin + Vector3(-bottom_offset, -height, bottom_offset);

	Vector<Vector3> points;
	points.push_back(top1);
	points.push_back(top2);
	points.push_back(top2);
	points.push_back(top3);
	points.push_back(top3);
	points.push_back(top4);
	points.push_back(top4);
	points.push_back(top1);

	points.push_back(bottom1);
	points.push_back(bottom2);
	points.push_back(bottom2);
	points.push_back(bottom3);
	points.push_back(bottom3);
	points.push_back(bottom4);
	points.push_back(bottom4);
	points.push_back(bottom1);

	points.push_back(top1);
	points.push_back(bottom1);
	points.push_back(top2);
	points.push_back(bottom2);
	points.push_back(top3);
	points.push_back(bottom3);
	points.push_back(top4);
	points.push_back(bottom4);

	Ref<Material> material = get_material("shape_material", p_gizmo);
	p_gizmo->clear();
	p_gizmo->add_lines(points, material);
}

///////////////////////////////////////

WatercraftPropulsionSpatialGizmoPlugin::
		WatercraftPropulsionSpatialGizmoPlugin() {
	Color gizmo_color =
			EDITOR_DEF("editors/3d_gizmos/gizmo_colors/shape", Color(0.5, 0.7, 1));
	create_material("shape_material", gizmo_color);
}

bool WatercraftPropulsionSpatialGizmoPlugin::has_gizmo(Node3D *p_spatial) {
	return Object::cast_to<WatercraftPropulsion>(p_spatial) != nullptr;
}

String WatercraftPropulsionSpatialGizmoPlugin::get_gizmo_name() const {
	return "WatercraftPropulsion";
}

int WatercraftPropulsionSpatialGizmoPlugin::get_priority() const {
	return -1;
}

void WatercraftPropulsionSpatialGizmoPlugin::redraw(
		EditorNode3DGizmo *p_gizmo) {
	WatercraftPropulsion *prop =
			Object::cast_to<WatercraftPropulsion>(p_gizmo->get_node_3d());
	Vector3 origin = prop->get_origin();
	Vector3 direction = prop->get_direction().normalized();
	Vector3 thrust_end = origin + direction;
	Vector3 up = Vector3(0, 1, 0);
	Vector3 side = direction.cross(up).normalized();
	Vector<Vector3> points;
	Vector3 blade1 = up * 0.25 + side * 0.075 + direction * 0.025;
	Vector3 blade2 = up * 0.25 - side * 0.075 - direction * 0.025;

	// thrust arrow
	points.push_back(origin);
	points.push_back(thrust_end);
	points.push_back(thrust_end);
	points.push_back(origin + direction * 0.8 + side * 0.2);
	points.push_back(thrust_end);
	points.push_back(origin + direction * 0.8 - side * 0.2);

	// propeller
	for (int i = 0; i < 6; i++) {
		points.push_back(origin);
		points.push_back(origin + blade1.rotated(direction, i * 1.047f));
		points.push_back(origin + blade1.rotated(direction, i * 1.047f));
		points.push_back(origin + blade2.rotated(direction, i * 1.047f));
		points.push_back(origin + blade2.rotated(direction, i * 1.047f));
		points.push_back(origin);
	}

	Ref<Material> material = get_material("shape_material", p_gizmo);
	p_gizmo->clear();
	p_gizmo->add_lines(points, material);
}

///////////////////////////////////////

WatercraftRudderSpatialGizmoPlugin::WatercraftRudderSpatialGizmoPlugin() {
	Color gizmo_color =
			EDITOR_DEF("editors/3d_gizmos/gizmo_colors/shape", Color(0.5, 0.7, 1));
	create_material("shape_material", gizmo_color);
}

bool WatercraftRudderSpatialGizmoPlugin::has_gizmo(Node3D *p_spatial) {
	return Object::cast_to<WatercraftRudder>(p_spatial) != nullptr;
}

String WatercraftRudderSpatialGizmoPlugin::get_gizmo_name() const {
	return "WatercraftRudder";
}

int WatercraftRudderSpatialGizmoPlugin::get_priority() const {
	return -1;
}

void WatercraftRudderSpatialGizmoPlugin::redraw(EditorNode3DGizmo *p_gizmo) {
	WatercraftRudder *rudder =
			Object::cast_to<WatercraftRudder>(p_gizmo->get_node_3d());
	Vector<Face3> faces = rudder->get_faces();
	const Vector3 &p1 = faces[0].vertex[0];
	const Vector3 &p2 = faces[0].vertex[1];
	const Vector3 &p3 = faces[0].vertex[2];
	const Vector3 &p4 = faces[1].vertex[1];

	Vector<Vector3> points;
	points.push_back(p1);
	points.push_back(p2);
	points.push_back(p2);
	points.push_back(p4);
	points.push_back(p4);
	points.push_back(p3);
	points.push_back(p3);
	points.push_back(p1);

	Ref<Material> material = get_material("shape_material", p_gizmo);
	p_gizmo->clear();
	p_gizmo->add_lines(points, material);
}

///////////////////////////////////////

EditorPluginHydro::EditorPluginHydro() {
	Ref<WatercraftBallastNode3DGizmoPlugin> ballast_gizmo_plugin =
			Ref<WatercraftBallastNode3DGizmoPlugin>(
					memnew(WatercraftBallastNode3DGizmoPlugin));
	Ref<WatercraftPropulsionSpatialGizmoPlugin> prop_gizmo_plugin =
			Ref<WatercraftPropulsionSpatialGizmoPlugin>(
					memnew(WatercraftPropulsionSpatialGizmoPlugin));
	Ref<WatercraftRudderSpatialGizmoPlugin> rudder_gizmo_plugin =
			Ref<WatercraftRudderSpatialGizmoPlugin>(
					memnew(WatercraftRudderSpatialGizmoPlugin));
	Node3DEditor::get_singleton()->add_gizmo_plugin(ballast_gizmo_plugin);
	Node3DEditor::get_singleton()->add_gizmo_plugin(prop_gizmo_plugin);
	Node3DEditor::get_singleton()->add_gizmo_plugin(rudder_gizmo_plugin);
}
