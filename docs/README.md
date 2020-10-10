**Godot Hydrodynamics module**

Hydro is a module for Godot 3.1+ which allows rigid bodies to behave realistically in water.  When an object enters the water, three forces will be calculated:

*  Buoyancy - the pressure of the water causes an object to float.
*  Drag - an object moving through the water will slow down.  The amount of drag is dependent on the object's shape.
*  Lift - an object moving through the water at an angle will generate lift, the way a boat lifts out of the water.  This is also dependent on the object's shape.

In addition to applying these forces, this module also does the following:

*  Allows you to plug in your own wave-generation function to have your objects bob in the ocean
*  Contains convenience nodes for you to apply thrust and steering, making it easy to create a boat or other watercraft.

To see all of these features in action, watch the demo [here](https://vimeo.com/318214966).  A simple demo project is also available for you to download [here](https://gitlab.com/ringtechsolutions/godot-tools/hydro/hydrosimpledemo).

**Installation**

For performance reasons, this is a native module that must be built from source with Godot.

1.  Get the source code for Godot: https://docs.godotengine.org/en/latest/development/compiling/getting_source.html
2.  Enter the source code directory, and go to the modules directory.  Clone this project into a folder called "hydro": `git clone https://gitlab.com/ringtechsolutions/godot-tools/hydro/hydro.git`
3.  Build Godot for your platform: https://docs.godotengine.org/en/latest/development/compiling/introduction_to_the_buildsystem.html

**Usage**

With this module, Godot will contain a several new Node types. [HydroRigidBody](HydroRigidBody) will do everything a normal RigidBody does, and adds the hydrodynamic forces on top of it.

[WaterArea](WaterArea) defines the area in which hydrodynamic forces will be calculated.

To customize the ocean height or add waves of your WaterArea, attach a script to your WaterArea and add a new function:

``` 
func _get_water_heights(positions):
	var ret : PoolVector3Array
	for p in positions:
		ret.append(Vector3(p.x, 42, p.z))
	return ret
```

This example will set the water's Y position to 42.  You could also hook the Y value into another function that generates waves.

The HydroRigidBody will expect one mesh to be a direct child.  This mesh will be the only one used to calculate the buoyancy and related forces.  This mesh may be concave, but it must be closed and not intersect itself.  Buoyancy is related to volume, and an open shape does not have a volume.  If you have other meshes that you want to come along for the ride, place them as a child of the main one.  For example, you could place your boat hull as the main mesh, and then have your deck fittings, outboard motor, etc as a child mesh.  If you want a hollow boat, this can be done as well using some [shader tricks](hollow-boats).

If you have a more complex object that is not floating properly, you can add an ImmediateGeometry node as a child of the HydroRigidBody, and it will automatically be used for debugging.  While your game is running, this will display a wireframe of the submerged portion of your hull, a line representing each of the forces that are generated, and a diamond shape representing the wave heights immediately surrounding your object.  You can use these to diagnose whether something is out of place.  Here are a couple things to watch out for:

*  Make sure your shape is totally closed.
*  Make sure all your normals face outwards.  If you see one force line facing the opposite direction of the rest, it's a good bet you need to check that face.
*  Make sure you set a realistic mass!  For reference, a 1m cube of styrofoam weighs 50 kg, so dropping in a default 2x2x2 cube and leaving the mass set at the default 1 will not work very well.
*  If you want an object to start in the water, place it as close to its "rest" position as possible.  Starting a little too high is usually better than starting too low.

For developing a watercraft, there are several other convenience nodes:

*  [WatercraftBallast](WatercraftBallast)
*  [WatercraftPropulsion](WatercraftPropulsion)
*  [WatercraftRudder](WatercraftRudder)

**Roadmap**

Here are the upcoming features for the next version:

*  Create editor gizmos.
*  Improve the method for debugging the forces.
*  Allow the actual shape of a WaterArea to automatically define the water height.
*  Add the ability to have a simple water flow such as a river affect HydroRigidBody objects.
*  Create a simplified mathematical model that allows you to optionally trade accuracy for speed.  This would accommodate use-cases where you have hundreds of objects, or offer a fallback for less powerful mobile devices.

**Technical Details**

I'd like to do a full technical writeup of the math behind this module, but you can get a sense of the basics in my posts [here](https://jonri.gonevis.com/tag/hydro/).  Here are some other resources I found useful or insightful along the way, in no particular order:

*  http://www.iforce2d.net/b2dtut/buoyancy
*  https://www.engineeringtoolbox.com/lift-drag-fluid-flow-d_1657.html
*  https://www.gamasutra.com/view/news/237528/Water_interaction_model_for_boats_in_video_games.php
*  https://www.gamasutra.com/view/news/263237/Water_interaction_model_for_boats_in_video_games_Part_2.php

While my approach has some differences, the writeup in the final two links above does an excellent job explaining and visualizing the physics involved.
