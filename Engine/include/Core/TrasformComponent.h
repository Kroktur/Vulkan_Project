#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

template<typename T>
struct DataDirty
{
	bool isDirty = false;
	T data = T{};
};

struct RotData
{
	enum class Dir
	{
		Up,
		Down,
		Forward,
		Backward,
		Left,
		Right
	};
	template<Dir dir>
	static glm::vec3 ToVector()
	{
		if		constexpr (dir == Dir::Up)
			return { 0,1,0 };
		else if constexpr (dir == Dir::Down)
			return { 0,-1,0 };
		else if constexpr (dir == Dir::Forward)
			return { 0,0,1 };
		else if constexpr (dir == Dir::Backward)
			return { 0,0,-1 };
		else if constexpr (dir == Dir::Right)
			return { 1,0,0 };
		else 
			return { -1,0,0 };
	}

	enum class Orientation
	{
		Yaw,
		Pitch,
		Roll
	};
	template<Orientation orientation>
	static glm::vec3 ToAxes()
	{
		if		constexpr (orientation == Orientation::Pitch)
			return { 1,0,0 };
		else if constexpr (orientation == Orientation::Yaw)
			return { 0,1, 0 };
		else 
			return { 0,0,1 };
	}
};


class TransformComponent
{
public:
	// POS
	glm::vec3 GetPosition() const;
	void SetPosition(const glm::vec3& other);
	void Translate(const glm::vec3& other);
	glm::mat4 GetTranslationMatrix();

	// SCALE
	glm::vec3 GetScale() const;
	void SetScale(const glm::vec3& other);
	glm::mat4 GetScaleMatrix();

	// ORIENTATION
	glm::quat GetOrientation() const;
	void SetOrientation(const glm::quat& other);
	template<RotData::Orientation orientation>
	void RotateQuat(float angleRad);
	void LookAt(const glm::vec3& target);

	// ROTATION 
	glm::vec3 GetRotation() const;
	void SetRotation(const glm::vec3& other);
	template<RotData::Orientation orientation>
	void RotateEuler(float angleRad);

	glm::mat4 GetRotationMatrix();

	// AXES
	template<RotData::Dir dir>
	glm::vec3 GetLocalAxe() const;

	// FULL TRANSFORM
	glm::mat4 GetFullTransform();
private:
	void UpdateEulerAngle();
	void UpdateQuaternion();
	bool IsDirty() const;
	DataDirty<glm::quat> m_rotation = {true,glm::quat{1,0,0,0}};
	 glm::mat4 m_rotationMat = glm::mat4{1.0f};
	 glm::vec3 m_eulerAngle = glm::vec3{0,0,0};
	DataDirty<glm::vec3> m_position = { true,glm::vec3{0,0,0} };
	 glm::mat4 m_positionMat = glm::mat4{ 1.0f };
	DataDirty <glm::vec3> m_scale = { true,glm::vec3{0,0,0} };
	 glm::mat4 m_scaleMat = glm::mat4{ 1.0f };
	 glm::mat4 m_fullTransform = glm::mat4{ 1.0f };
};

template <RotData::Orientation orientation>
void TransformComponent::RotateQuat(float angleRad)
{
	m_rotation.data = glm::normalize(glm::rotate(m_rotation.data, angleRad, RotData::ToAxes<orientation>()));
	UpdateEulerAngle();
	m_rotation.isDirty = true;
}

template <RotData::Orientation orientation>
void TransformComponent::RotateEuler(float angleRad)
{
	if constexpr (orientation == RotData::Orientation::Pitch)
		m_eulerAngle.x += angleRad;
	else if constexpr (orientation == RotData::Orientation::Roll)
		m_eulerAngle.z += angleRad;
	else
		m_eulerAngle.y += angleRad;
	UpdateQuaternion();
	m_rotation.isDirty = true;

}

template <RotData::Dir dir>
glm::vec3 TransformComponent::GetLocalAxe() const
{
	return glm::rotate(m_rotation.data, RotData::ToVector<dir>());
}
