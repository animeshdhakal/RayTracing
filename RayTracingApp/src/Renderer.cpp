#include "Renderer.h"
#include "Walnut/Random.h"

uint32_t Utils::ConvertToRGBA(glm::vec4 color) {

	// Little Endian - Least Significant Byte is in top of memory
	// Big Endian - Most Significant Byte is in top of memory
	// So In this Case My Computer is Little Endian So Alpha Channel is in top of memory so we shift it by 24 bits

	uint8_t r = (uint8_t)255.0f * color.r;
	uint8_t g = (uint8_t)255.0f * color.g;
	uint8_t b = (uint8_t)255.0f * color.b;
	uint8_t a = (uint8_t)255.0f * color.a;

	return (a << 24) | (b << 16) | (g << 8) | r;
}


void Renderer::Render()
{
	for (int y = 0; y < m_FinalImage->GetHeight(); y++) {
		for (int x = 0; x < m_FinalImage->GetWidth(); x++) {
			glm::vec2 coord = { (float)x / (float)m_FinalImage->GetWidth(), (float)y / (float)m_FinalImage->GetHeight() };
			coord = coord * 2.0f - 1.0f;
			glm::vec4 color = PerPixel(coord);

			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
		
			m_ImageData[m_FinalImage->GetWidth() * y + x] = Utils::ConvertToRGBA(color);
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

glm::vec4 Renderer::PerPixel(glm::vec2 coord)
{	
	// Aspect Ratio of Viewport
	float aspectRatio = (float)m_FinalImage->GetWidth() / (float)m_FinalImage->GetHeight();


	// This will fix the resize problem
	coord.x *= aspectRatio;

	// Radius of Sphere
	float r = 1.0f;

	auto rayOrigin = glm::vec3(0.0f, 0.0f, 2.0f);
	auto rayDirection = glm::vec3(coord.x, coord.y, -1);


	// We can normalize the vector but it will be more expensive than dot product
	// Normalize the RayDirection Vector
	// rayDirection = glm::normalize(rayDirection);
	// dot product of normalized vector or unit product with itself is 1

	// t^2(bx^2 + by^2) + t(2 * ax * bx + 2 * ay * by) + ax^2 + ay^2 - r^2 = 0

	float a = glm::dot(rayDirection, rayDirection);
	float b = 2.0f * glm::dot(rayOrigin, rayDirection);
	float c = glm::dot(rayOrigin, rayOrigin) - r * r;

	float d = b * b - 4.0f * a * c;

	if (d < 0.0f) {
		return glm::vec4(0, 0, 0, 1);
	}

	// float farthestT = (-b + glm::sqrt(d)) / (2.0f * a);
	// glm::vec3 farthestHitPoint = rayOrigin + rayDirection * t0;

	// Closest Value of T
	float closestT = (-b - glm::sqrt(d)) / (2.0f * a);

	// Closest Hit Point
	glm::vec3 hitPoint = rayOrigin + rayDirection * closestT;

	glm::vec3 sphereOrigin = glm::vec3(0.0f);

	glm::vec3 normal = glm::normalize(hitPoint - sphereOrigin);

	// Light Direction and Normal are facing opposite direction so we need to make them in same direction
	float dr = glm::max(glm::dot(normal, -m_LightDirection), 0.0f);

	return glm::vec4(m_ObjectColor * dr, 1);
}
