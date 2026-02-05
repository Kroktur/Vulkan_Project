#include "SAT.h"

#include <glm/geometric.hpp>

//SAT HELPER FUNCTIONS
namespace SatHelper
{
	//Absolute dot product -> avoid negative projection values
	static float ABSdot(const glm::vec3& a, const glm::vec3& b)
	{
		return std::fabs(glm::dot(a, b));
	}

	//Returns the projection of the OBB onto the axis
	static float OBBprojection(const OBB3D& box, const glm::vec3& axis)
	{
		return ABSdot(box.GetAxis(0), axis) * box.GetHalfSize().x +
			ABSdot(box.GetAxis(1), axis) * box.GetHalfSize().y +
			ABSdot(box.GetAxis(2), axis) * box.GetHalfSize().z;
	}

	//Tests the overlap of the projections of the boxes onto the axis, and updates minimum penetration and collision normal if necessary
	static bool AxisTest(const OBB3D& box1, const OBB3D& box2, const glm::vec3& axis, const glm::vec3& deltaCenter, float& minPenetration, glm::vec3& collisionNormal)
	{
		glm::vec3 normalizedAxis = glm::normalize(axis);
		//Project the boxes onto the axis
		float projection1 = OBBprojection(box1, normalizedAxis);
		float projection2 = OBBprojection(box2, normalizedAxis);
		//Calculate the distance between the projections
		float dist = ABSdot(deltaCenter, normalizedAxis);
		//Calculate the penetration depth
		float penetration = projection1 + projection2 - dist;
		//If there is no overlap, return false
		if (penetration <= 0)
			return false;
		//Update minimum penetration and collision normal if this axis has less penetration
		if (penetration < minPenetration)
		{
			minPenetration = penetration;
			collisionNormal = normalizedAxis;
		}
		return true;
	}

	//Convert an AABB to an OBB with the same center and half size, and local axes aligned with the world axes
	static OBB3D ConvertAABBtoOBB(const AABB3D& box)
	{
		return OBB3D(box.GetCenter(), box.GetHalfSize(), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	}

}

//Check collision between two AABBs using th)e Separating Axis Theorem
Collision3D SeparatingAxisTheorem::CheckCollisionAABB3D(const AABB3D& box1, const AABB3D& box2)
{
	//getters for centers and half sizes of the boxes
	glm::vec3 box1Center = box1.GetCenter();
	glm::vec3 box2Center = box2.GetCenter();
	glm::vec3 box1HalfSize = box1.GetHalfSize();
	glm::vec3 box2HalfSize = box2.GetHalfSize();
	glm::vec3 deltaCenter = box2Center - box1Center;

	//Check for overlap on each axis, and calculate penetration depth and collision normal
	float overlapX = box1HalfSize.x + box2HalfSize.x - std::fabs(deltaCenter.x);
	if (overlapX <= 0.0f)
		return Collision3D(false, 0.0f, glm::vec3(0.0f));

	float overlapY = box1HalfSize.y + box2HalfSize.y - std::fabs(deltaCenter.y);
	if (overlapY <= 0.0f)
		return Collision3D(false, 0.0f, glm::vec3(0.0f));

	float overlapZ = box1HalfSize.z + box2HalfSize.z - std::fabs(deltaCenter.z);
	if (overlapZ <= 0.0f)
		return Collision3D(false, 0.0f, glm::vec3(0.0f));

	float penetration = overlapX;
	glm::vec3 collisionNormal;

	//Find the axis of least penetration to determine the collision normal
	if (overlapX < overlapY && overlapX < overlapZ)
		collisionNormal = (deltaCenter.x < 0) ? glm::vec3(-1.0f, 0.0f, 0.0f) : glm::vec3(1.0f, 0.0f, 0.0f);
	else if (overlapY < overlapZ)
	{
		penetration = overlapY;
		collisionNormal = (deltaCenter.y < 0) ? glm::vec3(0.0f, -1.0f, 0.0f) : glm::vec3(0.0f, 1.0f, 0.0f);
	}
	else
	{
		penetration = overlapZ;
		collisionNormal = (deltaCenter.z < 0) ? glm::vec3(0.0f, 0.0f, -1.0f) : glm::vec3(0.0f, 0.0f, 1.0f);
	}

	//Collision detected, return the result with penetration depth and collision normal
	return Collision3D(true, penetration, collisionNormal);
}

//Check collision between two OBBs using the Separating Axis Theorem
Collision3D SeparatingAxisTheorem::CheckCollisionOBB3D(const OBB3D& box1, const OBB3D& box2)
{
	using namespace SatHelper;

	//getters for cezters and half sizes of the boxes
	glm::vec3 box1Center = box1.GetCenter();
	glm::vec3 box2Center = box2.GetCenter();
	glm::vec3 deltaCenter = box2Center - box1Center;

	float minPenetration = std::numeric_limits<float>::max();
	glm::vec3 collisionNormal;

	//Check the 3 axis of the first box
	for (int i = 0; i < 3; ++i)
		if (!AxisTest(box1, box2, box1.GetAxis(i), deltaCenter, minPenetration, collisionNormal))
			return Collision3D(false, 0.0f, glm::vec3(0.0f));

	//Check the 3 axis of the second box
	for (int i = 0; i < 3; ++i)
		if (!AxisTest(box1, box2, box2.GetAxis(i), deltaCenter, minPenetration, collisionNormal))
			return Collision3D(false, 0.0f, glm::vec3(0.0f));

	//Check the 9 cross product axes
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			if (!AxisTest(box1, box2, glm::cross(box1.GetAxis(i), box2.GetAxis(j)), deltaCenter, minPenetration, collisionNormal))
				return Collision3D(false, 0.0f, glm::vec3(0.0f));

	//Make sure the collision normal points from box1 to box2
	if (glm::dot(collisionNormal, deltaCenter) < 0)
		collisionNormal = -collisionNormal;

	//Collision detected, return the result with penetration depth and collision normal
	return Collision3D(true, minPenetration, collisionNormal);
}

//Check collision between an AABB and an OBB using the Separating Axis Theorem
Collision3D SeparatingAxisTheorem::CheckCollisionAABBvsOBB(const AABB3D& box1, const OBB3D& box2)
{
	//Make teh AABB to a OBB
	OBB3D box1AsOBB = SatHelper::ConvertAABBtoOBB(box1);
	return CheckCollisionOBB3D(box1AsOBB, box2);
}