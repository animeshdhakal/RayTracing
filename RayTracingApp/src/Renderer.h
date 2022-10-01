#pragma once

#include "Walnut/Image.h"
#include <memory>
#include <glm/glm.hpp>
#include <iostream>
#include <chrono>


namespace Utils {
	uint32_t ConvertToRGBA(glm::vec4 color);
}

class Renderer
{
public:
	Renderer() : m_ObjectColor(1.0f, 1.0f, 0.5f), m_LightDirection(-1) {}
	void Render();
	void OnResize(uint32_t width, uint32_t height);
	glm::vec4 PerPixel(glm::vec2 coord);
	std::shared_ptr<Walnut::Image> GetFinalImage() { return m_FinalImage; }
	
	void SetObjectColor(glm::vec3 color) {
		m_ObjectColor = color;
	}

	void setLightDirection(glm::vec3 lightDirection) {
		m_LightDirection = lightDirection;
	}

private:
	std::shared_ptr<Walnut::Image> m_FinalImage;
	uint32_t* m_ImageData = nullptr;
	glm::vec3 m_ObjectColor;
	glm::vec3 m_LightDirection;
};

