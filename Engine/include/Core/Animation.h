#pragma once

#include <string>
#include <vector>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace KGR
{
	namespace Animation
	{
		/*
		* @brief Represents a single joint in a skeleton, 
		* containing its name, ID, inverse bind matrix, 
		* local transformation (translation, rotation, scale), and child joint indices.
		*/
		struct Joint
		{
			std::string name;
			int id;
			glm::mat4 inverseBindMatrix{ 1.0f };
			glm::vec3 translation{ 0.0f };
			glm::quat rotation{ 1.0f, 0.0f, 0.0f, 0.0f };
			glm::vec3 scale{ 1.0f };
			std::vector<int> m_children;
		};

		/*
		* @brief Represents a skeleton, which is a hierarchical structure of joints used for skeletal animation.
		*/
		struct Skeleton
		{
			std::string name;
			std::vector<Joint> m_joints;
		};

		/*
		* @brief Represents a keyframe for vector properties (position or scale) in an animation track.
		* Contains the time of the keyframe and the corresponding vector value.
		*/
		struct VectorKeyFrame
		{
			float time;
			glm::vec3 m_value;
		};

		/*
		* @brief Represents a keyframe for rotation properties in an animation track.
		*/
		struct QuaternionKeyFrame
		{
			float time;
			glm::quat m_value;
		};

		/*
		* @brief Represents an animation track for a specific joint/node, 
		* containing keyframes for position, rotation, and scale.
		* Each track corresponds to a single joint 
		* and defines how that joint transforms over time in an animation clip.
		*/
		struct Track
		{
			int nodeId;
			std::vector<VectorKeyFrame> m_positions;
			std::vector<QuaternionKeyFrame> m_rotations;
			std::vector<VectorKeyFrame> m_scales;
		};

		/*
		* @brief Represents an animation clip, 
		* which is a collection of animation tracks for various joints/nodes, along with the clip's name and duration.
		*/
		struct AnimationClip
		{
			std::string name;
			float duration = 0.0f;
			std::vector<Track> m_tracks;
		};
	}
}