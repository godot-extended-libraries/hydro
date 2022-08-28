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

#ifndef HYDRO_GIZMO_PLUGINS_H
#define HYDRO_GIZMO_PLUGINS_H

#include "editor/editor_node.h"
#include "editor/plugins/node_3d_editor_plugin.h"

class WatercraftBallastNode3DGizmoPlugin : public EditorNode3DGizmoPlugin {
	GDCLASS(WatercraftBallastNode3DGizmoPlugin, EditorNode3DGizmoPlugin)
public:
	bool has_gizmo(Node3D *p_spatial) override;
	String get_name() const;
	int get_priority() const override;
	void redraw(EditorNode3DGizmo *p_gizmo) override;

	WatercraftBallastNode3DGizmoPlugin();
};

class WatercraftPropulsionSpatialGizmoPlugin : public EditorNode3DGizmoPlugin {
	GDCLASS(WatercraftPropulsionSpatialGizmoPlugin, EditorNode3DGizmoPlugin)
public:
	bool has_gizmo(Node3D *p_spatial) override;
	String get_name() const;
	int get_priority() const override;
	void redraw(EditorNode3DGizmo *p_gizmo) override;

	WatercraftPropulsionSpatialGizmoPlugin();
};

class WatercraftRudderSpatialGizmoPlugin : public EditorNode3DGizmoPlugin {
	GDCLASS(WatercraftRudderSpatialGizmoPlugin, EditorNode3DGizmoPlugin)
public:
	bool has_gizmo(Node3D *p_spatial) override;
	String get_name() const;
	int get_priority() const override;
	void redraw(EditorNode3DGizmo *p_gizmo) override;

	WatercraftRudderSpatialGizmoPlugin();
};

class EditorPluginHydro : public EditorPlugin {
	GDCLASS(EditorPluginHydro, EditorPlugin)
public:
	EditorPluginHydro();
};

#endif
