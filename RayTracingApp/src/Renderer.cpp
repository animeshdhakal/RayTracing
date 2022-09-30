#include "Renderer.h"
#include "Walnut/Random.h"

void Renderer::Render()
{
	for (int y = 0; y < m_FinalImage->GetHeight(); y++) {
		for (int x = 0; x < m_FinalImage->GetWidth(); x++) {
			glm::vec2 coord = { (float)x / (float)m_FinalImage->GetWidth(), (float)y / (float)m_FinalImage->GetHeight() };
			coord = coord * 2.0f - 1.0f;
			m_ImageData[m_FinalImage->GetWidth() * y + x] = PerPixel(coord);
		}
	}

	m_FinalImage->SetData(m_ImageData);
}

void Renderer::OnResize(uint32_t width, uint32_t height)
{
	if (m_FinalImage)
	{
		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
			return;

		m_FinalImage->Resize(width, height);
	}
	else
	{
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	delete[] m_ImageData;
	m_ImageData = new uint32_t[width * height];

}

uint32_t Renderer::PerPixel(glm::vec2 coord)
{

	float r = 0.5f;
	auto rayOrigin = glm::vec3(0.0f, 0.0f, 3.0f);
	auto rayDirection = glm::vec3(coord.x, coord.y, -1);


	float a = glm::dot(rayDirection, rayDirection);
	float b = 2.0f * glm::dot(rayOrigin, rayDirection);
	float c = glm::dot(rayOrigin, rayOrigin) - r * r;


	float d = b * b - 4.0f * a * c;

	if (d >= 0.0f) {
		return 0xFF00FFFF;
	}

	return 0xFF0000FF;
}
