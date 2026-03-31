#pragma once

#include "Animation.h"
#include <vector>

namespace KGR
{
	namespace Animation
	{
		/*
		* @brief AnimationComponent is responsible for managing the state of an animation instance, 
		* including the current time, skeleton, animation clip, 
		* and calculating the global transformation matrices for each joint based on the animation data.
		*/
		class AnimationComponent
		{
		public:
			AnimationComponent() = default;
			~AnimationComponent() = default;

			/*
			* @brief Initializes the animation component with a given skeleton and animation clip.
			* @param skeleton Pointer to the skeleton that defines the joint hierarchy and inverse bind matrices.
			* @param clip Pointer to the animation clip that contains the keyframe data for animating the skeleton.
			*/
			void Init(const Skeleton* skeleton, const AnimationClip* clip);

			/*
			* @brief Updates the animation state by advancing the current time based on the delta time 
			* and recalculating the global transformation matrices for each joint.
			* @param deltaTime The time elapsed since the last update, used to advance the animation time.
			*/
			void Update(float deltaTime);

			/*
			* @brief Returns the current global transformation matrices for each joint, which can be used for skinning in the vertex shader.
			*/
			const std::vector<glm::mat4>& GetLastBoneMatrices() const;

		private:
			const Skeleton* m_skeleton = nullptr;
			const AnimationClip* m_clip = nullptr;
			float m_currentTime = 0.0f;

			std::vector<glm::mat4> m_globalMAtrices;
			std::vector<glm::mat4> m_lastBoneMatrices;

			/*
			* @brief Recursively calculates the global transformation matrix for each joint 
			* based on the current animation time and the parent transformation.
			* @param joint The current joint for which to calculate the transformation.
			* @param parentTransform The global transformation matrix of the parent joint, used to compute the current joint's global transform.
			*/
			void CalculateBoneTransform(const Joint& joint, const glm::mat4& parentTransform);

			/*
			* @brief Interpolates the position, rotation, and scale for a given time based on the keyframes in the animation track.
			* @param time The current animation time for which to interpolate the transformation.
			* @param track The animation track containing the keyframes for the joint being animated.
			* @return The interpolated position, rotation, and scale as a tuple.
			*/
			glm::vec3 InterpolatePosition(float time, const Track& track);
			glm::quat InterpolateRotation(float time, const Track& track);
			glm::vec3 InterpolateScale(float time, const Track& track);
		};
	}
}