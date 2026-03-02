#pragma once

#include <glm/glm.hpp>

#include <glm/ext/matrix_clip_space.hpp>
#include "Tools/Util.h"


struct alignas(16) LightData
{
	enum class Type : std::uint32_t
	{
		Point = 0,
		Directional = 1,
		Spot = 2
	};
	PadData<glm::vec3,16> pos = glm::vec3();
	PadData <glm::vec3,16> dir = glm::vec3();
	PadData <glm::vec3,16> lightColor = glm::vec3();
	PadData <glm::vec3,16> specularColor = glm::vec3();
	float intensity = 0.0f;
	float openAngle = 0.0f;
	float additionalSmooth = 0.0f;
	float shiny =0.0f;
	PadData <uint32_t,16> type =static_cast<std::uint32_t>(LightData::Type::Point);
};


template<LightData::Type type>
struct LightComponent;

template<>
struct LightComponent<LightData::Type::Point>
{
	glm::vec3 GetLightColor() const;
	glm::vec3 GetSpecularColor() const;
	float GetIntensity() const;
	float GetShiny() const;
	void SetColor(const glm::vec3& lightColor, const glm::vec3& specularColor);
	void SetParam(float intensity, float shiny);
	LightData ToData() const;
	static LightComponent<LightData::Type::Point> Create(const glm::vec3& lightColor,const glm::vec3& specularColor,float intensity,float shiny);

private :
	glm::vec3 m_lightColor = glm::vec3{ 1.0f,1.0f,1.0f };
	glm::vec3 m_specularColor = glm::vec3{ 1.0f,1.0f,1.0f };
	float m_intensity = 100.0f;
	float m_shiny = 100.0f;
};

template<>
struct LightComponent<LightData::Type::Directional>
{
	glm::vec3 GetLightColor() const;
	glm::vec3 GetSpecularColor() const;
	float GetShiny() const;
	void SetColor(const glm::vec3& lightColor, const glm::vec3& specularColor);
	void SetParam(float shiny);
	LightData ToData() const;
	static LightComponent<LightData::Type::Directional> Create(const glm::vec3& lightColor, const glm::vec3& specularColor, float shiny);
private:
	glm::vec3 m_lightColor = glm::vec3{ 1.0f,1.0f,1.0f };
	glm::vec3 m_specularColor = glm::vec3{ 1.0f,1.0f,1.0f };
	float m_shiny = 100.0f;
};

template<>
struct LightComponent<LightData::Type::Spot>
{
	glm::vec3 GetLightColor() const;
	glm::vec3 GetSpecularColor() const;
	float GetIntensity() const;
	float GetShiny() const;
	float GetOpenAngle() const;
	float GetAdditionalSmooth() const;
	void SetColor(const glm::vec3& lightColor, const glm::vec3& specularColor);
	void SetParam(float intensity, float shiny);
	void SetAngle(float angle, float additionalSmooth);
	LightData ToData() const;
	static LightComponent<LightData::Type::Spot> Create(const glm::vec3& lightColor, const glm::vec3& specularColor,float intensity, float shiny,float openAngle,float addSmooth);

private:
	glm::vec3 m_lightColor = glm::vec3{ 1.0f,1.0f,1.0f };
	glm::vec3 m_specularColor = glm::vec3{ 1.0f,1.0f,1.0f };
	float m_intensity = 100.0f;
	float m_shiny = 100.0f;
	float m_openAngle = glm::radians(45.0f);
	float m_additionalSmooth = 5.0f;
};


