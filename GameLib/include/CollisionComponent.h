#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "AABB.h"
#include "OBB.h"
#include "Core/Mesh.h"
#include "Core/RessourcesManager.h"

struct Collider
{
	KGR::AABB3D localBox;
	KGR::AABB3D ComputeGlobalAABB(const glm::vec3 & scale,const glm::vec3& pos) const 
	{
		return  KGR::AABB3D(localBox.m_min * scale + pos, localBox.m_max * scale + pos);
	}
	KGR::OBB3D ComputeGlobalOBB(const glm::vec3& scale, const glm::vec3& pos,const glm::quat& orientation) const 
	{
		glm::vec3 forward =orientation * glm::vec3{0,0,-1} ;
		glm::vec3 right = orientation * glm::vec3{ 1,0,0 };
		glm::vec3 up = orientation *glm::vec3{ 0,1,0 };

		auto center = pos + orientation * (localBox.GetCenter() * scale);

		glm::vec3 halfSize = localBox.GetHalfSize() * scale;

		return KGR::OBB3D(center, halfSize, right, up, forward);

	}
};

struct CollisionComp
{
	Collider* collider;
};

inline std::unique_ptr<Collider> GenerateBox(const std::string& filePath,Mesh* mesh)
{
	std::unique_ptr<Collider> collide = std::make_unique<Collider>();

	float minX = std::numeric_limits<float>::max(), minY= std::numeric_limits<float>::max(), minZ = std::numeric_limits<float>::max();
	float maxX = std::numeric_limits<float>::lowest(), maxY = std::numeric_limits<float>::lowest(), maxZ = std::numeric_limits<float>::lowest();
	
	for (int i = 0 ; i < (mesh)->GetSubMeshesCount() ; ++i)
	{
		auto& sub = (mesh)->GetSubMesh(i);
		auto vertices = sub.GetVertices();
		for (int j = 0 ; j < sub.VertexCount() ; ++j)
		{
			minX = std::min(minX, vertices[j].pos.x);
			minY = std::min(minY, vertices[j].pos.y);
			minZ = std::min(minZ, vertices[j].pos.z);

			maxX = std::max(maxX, vertices[j].pos.x);
			maxY = std::max(maxY, vertices[j].pos.y);
			maxZ = std::max(maxZ, vertices[j].pos.z);
		}
	}
	KGR::AABB3D box;
	box.m_min.x = minX;
	box.m_min.y = minY;
	box.m_min.z = minZ;

	box.m_max.x = maxX;
	box.m_max.y = maxY;
	box.m_max.z = maxZ;
	collide->localBox = box;

	return std::move(collide);
}
using ColliderManager = KGR::ResourceManager<Collider, KGR::TypeWrapper<Mesh*>, GenerateBox>;


// How to use : load a collider ColComp.collider = ColliderManager::Load("name(probably name og the obj you can chose)",pointer of Mesh or nullptr if you already have load the collider)
// use the collider : comp.collider->ComputeAABB(pos , scale ) or OBB : comp.collider->ComputeOBB(pos,scale,orientation)