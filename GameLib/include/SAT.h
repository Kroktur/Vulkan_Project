#pragma once
#include "AABB.h"
#include "OBB.h"
#include "Collision.h"

class SeparatingAxisTheorem
{
public:
	static Collision3D CheckCollisionAABB3D(const AABB3D& box1, const AABB3D& box2);
	static Collision3D CheckCollisionOBB3D(const OBB3D& box1, const OBB3D& box2);
	static Collision3D CheckCollisionAABBvsOBB(const AABB3D& box1, const OBB3D& box2);
};