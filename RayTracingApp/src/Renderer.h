#pragma once

#include "Walnut/Image.h"
#include <memory>
#include <glm/glm.hpp>

class Renderer
{
public:
	void Render();
	void OnResize(uint32_t width, uint32_t height);
	uint32_t PerPixel(glm::vec2 coord);
	std::shared_ptr<Walnut::Image> GetFinalImage() { return m_FinalImage; }

private:
	std::shared_ptr<Walnut::Image> m_FinalImage;
	uint32_t* m_ImageData = nullptr;
};

