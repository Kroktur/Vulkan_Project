#pragma once

#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>

#include <string>
#include <vector>

#include "Vertex.h"
#include "Animation.h"

namespace KGR
{
	namespace GLB
	{
		/*
		* @brief Represents a raw image loaded from a GLB file, containing its name and binary data.
		*/
		struct RawImage
		{
			std::string m_name;
			std::vector<uint8_t> m_data;
		};

		/*
		* @brief GLB_Loader is responsible for loading 3D models, textures, skeletons, and animations from GLB files using the fastgltf library.
		* It provides methods to access the loaded vertices, indices, images, skeletons, and animations for use in rendering and animation systems.
		*/
		class GLB_Loader
		{
		public:
			GLB_Loader() = default;
			~GLB_Loader() = default;

			/*
			* @brief Loads a GLB file from the specified filepath and extracts its vertices, indices, images, skeletons, and animations.
			* @param filepath The path to the GLB file to be loaded.
			* @return true if the file was successfully loaded and parsed, false otherwise.
			*/
			bool Load(const std::string& filepath);

			/*
			* @brief Getters for the loaded data.
			*/
			const std::vector<Vertex>& GetVertices() const;
			const std::vector<uint32_t>& GetIndices() const;
			const std::vector<RawImage>& GetImages() const;
			const std::vector<KGR::Animation::Skeleton>& GetSkeletons() const;
			const std::vector<KGR::Animation::AnimationClip>& GetAnimations() const;

		private:

			std::vector<Vertex> m_vertices;
			std::vector<uint32_t> m_indices;
			std::vector<RawImage> m_images;
			std::vector<KGR::Animation::Skeleton> m_skeletons;
			std::vector<KGR::Animation::AnimationClip> m_animations;
		};
	}
}