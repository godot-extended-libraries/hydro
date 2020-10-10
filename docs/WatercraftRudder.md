WatercraftRudder
================

Brief Description
-----------------

Adds a steerable rudder to a `HydroRigidBody`.

Properties
----------

float   depth

Vector3 direction

float   length

Vector3 origin

Description
-----------

This will generate a rudder of a given size and calculate the forces along with the rest of the hull.  As with the rest of the hull, only the submerged portion of the rudder will function.

Property Descriptions
---------------------

- float **depth**

The extent of the rudder below the origin.

- Vector3 **direction**

The direction the rudder will face.  Change the direction to steer the rudder.

- float **length**

The length of the rudder in the specified direction.

- Vector3 **origin**

The location of the rudder.
