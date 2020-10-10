WatercraftBallast
=================

Brief Description
-----------------

Allows off-center weight to be added to a `HydroRigidBody`
Properties
----------

float mass

Vector3 origin

Description
-----------

By default, the mass of a `HydroRigidBody` is centered at the origin of the object.  Adding additional weight at various points can improve realism in some cases. The WatercraftBallast node allows you to position a point mass at a specific location.  This can be useful for cases such as:

* Creating a keel on a sailboat

* Simulating cargo on a ship

* Shifting the weight of a rider

Property Descriptions
---------------------

* float **mass**

The mass of the ballast.

* Vector3 **origin**

The location of the ballast.
