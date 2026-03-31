#include "Core/GLBLoader.h"
#include <fastgltf/tools.hpp>
#include <fastgltf/glm_element_traits.hpp>

#include <iostream>
#include <variant>

namespace KGR
{
	namespace GLB
	{
		bool GLB_Loader::Load(const std::string& filepath)
		{
			fastgltf::Parser parser;

			auto data = fastgltf::GltfDataBuffer::FromPath(filepath);
			if (data.error() != fastgltf::Error::None)
			{
				std::cerr << "[GLB_Loader] Reading error : " << filepath << "\n";
				return false;
			}

			constexpr auto options = fastgltf::Options::LoadGLBBuffers | fastgltf::Options::LoadExternalBuffers;
			auto asset = parser.loadGltfBinary(data.get(), filepath, options);
			if (asset.error() != fastgltf::Error::None)
			{
				std::cerr << "[GLB_Loader] Error parsing gltf\n";
				return false;
			}

			fastgltf::Asset& gltf = asset.get();

			m_vertices.clear();
			m_indices.clear();
			m_images.clear();
			m_skeletons.clear();
			m_animations.clear();

			for (auto& mesh : gltf.meshes)
			{
				for (auto& primitive : mesh.primitives)
				{
					const size_t initialVertex = m_vertices.size();

					auto positionIt = primitive.findAttribute("POSITION");
					if (positionIt != primitive.attributes.end())
					{
						fastgltf::Accessor& positionAccessor = gltf.accessors[positionIt->accessorIndex];
						m_vertices.resize(initialVertex + positionAccessor.count);

						fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf, positionAccessor,
							[&](glm::vec3 pos, size_t index)
							{
								m_vertices[initialVertex + index].pos = pos;
								m_vertices[initialVertex + index].normal = glm::vec3(0, 1, 0);
								m_vertices[initialVertex + index].uv = glm::vec2(0);
								m_vertices[initialVertex + index].color = glm::vec4(1);
								m_vertices[initialVertex + index].tangent = glm::vec4(0);
								m_vertices[initialVertex + index].joints = glm::ivec4(0);
								m_vertices[initialVertex + index].weights = glm::vec4(1, 0, 0, 0);
							});
					}

					auto normalIt = primitive.findAttribute("NORMAL");
					if (normalIt != primitive.attributes.end())
					{
						fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf, gltf.accessors[normalIt->accessorIndex],
							[&](glm::vec3 normal, size_t idx)
							{
								m_vertices[initialVertex + idx].normal = normal;
							});
					}

					auto uvIt = primitive.findAttribute("TEXCOORD_0");
					if (uvIt != primitive.attributes.end())
					{
						fastgltf::iterateAccessorWithIndex<glm::vec2>(gltf, gltf.accessors[uvIt->accessorIndex],
							[&](glm::vec2 uv, size_t i) 
							{
								m_vertices[initialVertex + i].uv = uv; 
							});
					}

					auto jointsIt = primitive.findAttribute("JOINTS_0");
					if (jointsIt != primitive.attributes.end())
					{
						auto& jointsAccessor = gltf.accessors[jointsIt->accessorIndex];

						if (jointsAccessor.componentType == fastgltf::ComponentType::UnsignedByte)
						{
							fastgltf::iterateAccessorWithIndex<glm::u8vec4>(gltf, jointsAccessor,
								[&](glm::u8vec4 j, size_t i) 
								{
									m_vertices[initialVertex + i].joints = glm::ivec4(j);
								});
						}
						else if (jointsAccessor.componentType == fastgltf::ComponentType::UnsignedShort)
						{
							fastgltf::iterateAccessorWithIndex<glm::u16vec4>(gltf, jointsAccessor,
								[&](glm::u16vec4 j, size_t i) 
								{
									m_vertices[initialVertex + i].joints = glm::ivec4(j);
								});
						}
					}

					auto weightsIt = primitive.findAttribute("WEIGHTS_0");
					if (weightsIt != primitive.attributes.end())
					{
						fastgltf::iterateAccessorWithIndex<glm::vec4>(gltf, gltf.accessors[weightsIt->accessorIndex],
							[&](glm::vec4 weight, size_t i) 
							{
								m_vertices[initialVertex + i].weights = weight;
							});
					}

					if (primitive.indicesAccessor.has_value())
					{
						fastgltf::iterateAccessor<uint32_t>(gltf, gltf.accessors[primitive.indicesAccessor.value()],
							[&](uint32_t idx) 
							{ 
								m_indices.push_back(idx + initialVertex); 
							});
					}
				}
			}


			for (auto& image : gltf.images)
			{
				RawImage rawImg;
				rawImg.m_name = image.name.c_str();

				std::visit(fastgltf::visitor
					{
						[](auto& arg) {},
						[&](fastgltf::sources::Vector& vector) 
						{
							rawImg.m_data.assign(vector.bytes.begin(), vector.bytes.end());
						},
						[&](fastgltf::sources::BufferView& view) 
						{
							auto& bufferView = gltf.bufferViews[view.bufferViewIndex];
							auto& buffer = gltf.buffers[bufferView.bufferIndex];
							std::visit(fastgltf::visitor
							{
								[](auto& arg) {},
								[&](fastgltf::sources::Array& array) 
								{
									const uint8_t* start = reinterpret_cast<const uint8_t*>(array.bytes.data()) + bufferView.byteOffset;
									rawImg.m_data.assign(start, start + bufferView.byteLength);
								},
								[&](fastgltf::sources::Vector& vec) 
								{
									const uint8_t* start = reinterpret_cast<const uint8_t*>(vec.bytes.data()) + bufferView.byteOffset;
									rawImg.m_data.assign(start, start + bufferView.byteLength);
								}
							}, buffer.data);
						}
					}, image.data);

				if (!rawImg.m_data.empty())
					m_images.push_back(rawImg);
			}


			for (auto& skin : gltf.skins)
			{
				KGR::Animation::Skeleton skeleton;
				skeleton.name = skin.name.c_str();

				std::vector<glm::mat4> inverseBindMatrices(skin.joints.size(), glm::mat4(1.0f));
				if (skin.inverseBindMatrices.has_value())
				{
					fastgltf::iterateAccessorWithIndex<glm::mat4>(gltf, gltf.accessors[skin.inverseBindMatrices.value()],
						[&](glm::mat4 mat, size_t i)
						{
							inverseBindMatrices[i] = mat;
						});
				}

				for (size_t i = 0; i < skin.joints.size(); ++i)
				{
					size_t nodeIndex = skin.joints[i];
					auto& node = gltf.nodes[nodeIndex];

					KGR::Animation::Joint joint;
					joint.name = node.name.c_str();
					joint.id = static_cast<int>(i);
					joint.inverseBindMatrix = inverseBindMatrices[i];

					std::visit(fastgltf::visitor
						{
							[&](const fastgltf::math::fmat4x4 matrix) {},
							[&](const fastgltf::TRS& trs)
							{
								joint.translation = glm::vec3(trs.translation[0], trs.translation[1], trs.translation[2]);
								joint.rotation = glm::quat(trs.rotation[3], trs.rotation[0], trs.rotation[1], trs.rotation[2]);
								joint.scale = glm::vec3(trs.scale[0], trs.scale[1], trs.scale[2]); 
							},
							[&](const auto& arg) {}

						}, node.transform);

					for (size_t childNodeIndex : node.children)
					{
						auto it = std::find(skin.joints.begin(), skin.joints.end(), childNodeIndex);
						if (it != skin.joints.end())
							joint.m_children.push_back(static_cast<int>(std::distance(skin.joints.begin(), it)));
					}
					skeleton.m_joints.push_back(joint);
				}
				m_skeletons.push_back(skeleton);
			}


			for (auto& animation : gltf.animations)
			{
				KGR::Animation::AnimationClip clip;
				clip.name = animation.name.c_str();

				std::map<int, KGR::Animation::Track> nodeTracks;

				for (auto& channel : animation.channels)
				{
					if (!channel.nodeIndex.has_value()) 
						continue;

					int nodeID = static_cast<int>(channel.nodeIndex.value());

					if (nodeTracks.find(nodeID) == nodeTracks.end()) 
						nodeTracks[nodeID] = KGR::Animation::Track{ nodeID };

					auto& sampler = animation.samplers[channel.samplerIndex];
					auto& inputAccessor = gltf.accessors[sampler.inputAccessor];
					auto& outputAccessor = gltf.accessors[sampler.outputAccessor];

					std::vector<float> times;
					fastgltf::iterateAccessor<float>(gltf, inputAccessor, [&](float t)
					{
						times.push_back(t);
						if (t > clip.duration) 
							clip.duration = t;
					});

					size_t timeIndex = 0;
					if (channel.path == fastgltf::AnimationPath::Translation)
					{
						fastgltf::iterateAccessor<glm::vec3>(gltf, outputAccessor, [&](glm::vec3 v) 
						{
							nodeTracks[nodeID].m_positions.push_back({ times[timeIndex++], v });
						});
					}
					else if (channel.path == fastgltf::AnimationPath::Rotation)
					{
						fastgltf::iterateAccessor<glm::vec4>(gltf, outputAccessor, [&](glm::vec4 v) 
						{
							nodeTracks[nodeID].m_rotations.push_back({ times[timeIndex++], glm::quat(v.w, v.x, v.y, v.z) });
						});
					}
					else if (channel.path == fastgltf::AnimationPath::Scale)
					{
						fastgltf::iterateAccessor<glm::vec3>(gltf, outputAccessor, [&](glm::vec3 v) 
						{
							nodeTracks[nodeID].m_scales.push_back({ times[timeIndex++], v });
						});
					}
				}

				for (auto& [id, track] : nodeTracks) 
					clip.m_tracks.push_back(track);

				m_animations.push_back(clip);
			}

			return true;
		}

		const std::vector<Vertex>& GLB_Loader::GetVertices() const
		{
			return m_vertices;
		}

		const std::vector<uint32_t>& GLB_Loader::GetIndices() const
		{
			return m_indices;
		}

		const std::vector<RawImage>& GLB_Loader::GetImages() const
		{
			return m_images;
		}

		const std::vector<KGR::Animation::Skeleton>& GLB_Loader::GetSkeletons() const
		{
			return m_skeletons;
		}

		const std::vector<KGR::Animation::AnimationClip>& GLB_Loader::GetAnimations() const
		{
			return m_animations;
		}
	}
}