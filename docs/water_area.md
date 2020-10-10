WaterArea
=========

Brief Description
-----------------

Defines an area in which a `HydroRigidBody` will be affected by buoyancy and related forces.

Properties
----------

float density

float water_height

Description
-----------

The purpose of the WaterArea node is threefold.  First, it limits the area in which a `HydroRigidBody` is evaluated for potential buoyancy. This means that if you only need water in a small area of your scene, the `HydroRigidBody` will not incur a performance penalty while it is outside the bounds of the WaterArea.

The second purpose of the WaterArea is to define the surface of the water.  For a flat body of water, set the water_height parameter to the global-space height of the water. For a body of water with waves or any other advanced scenarios, attach a script to the WaterArea and define a `_get_water_heights(positions)` function.  The positions parameter is a PoolVector3Array.  For each Vector3 in the array, adjust its Y value to the height of the water at the given X and Z position.  The return value of the function should be the PoolVector3Array with the Y-values changed.  When this function is defined, the water_height parameter is ignored.

Finally, the WaterArea acts as a standard Area node.  It can be used to trigger sound or particle effects to make an object splash when it hits the water.

Property Descriptions
---------------------

* float **density**

Sets the density of the water in this area.  Freshwater is 1000 kg/m^3, and seawater ranges from 1020 - 1029 kg/m^3.

* float **water_height**

Sets a constant global-space water height.  This value is not used if you create a custom wave height function instead.
