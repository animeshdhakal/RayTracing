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


void Renderer::Render(const Scene& scene, const Camera& camera)
{
	Ray ray;
	ray.origin = camera.GetPosition();

	for (int y = 0; y < m_FinalImage->GetHeight(); y++) {
		for (int x = 0; x < m_FinalImage->GetWidth(); x++) {
			const glm::vec3& rayDirection = camera.GetRayDirections()[y * m_FinalImage->GetWidth() + x];

			ray.direction = rayDirection;
			
			glm::vec4 color = TraceRay(scene, ray);

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

glm::vec4 Renderer::TraceRay(const Scene& scene, const Ray& ray)
{	
	if (scene.spheres.size() == 0)
		return glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	
	const Sphere* closestSphere = nullptr;
	float hitDistance = FLT_MAX;

	for (const Sphere& sphere : scene.spheres) {
		glm::vec3 origin = ray.origin - sphere.position;
		
		float a = glm::dot(ray.direction, ray.direction);
		float b = 2.0f * glm::dot(origin, ray.direction);
		float c = glm::dot(origin, origin) - sphere.radius * sphere.radius;
		float d = b * b - 4.0f * a * c;

		if (d < 0.0f) {
			continue;
		}

		float closestT = (-b - glm::sqrt(d)) / (2.0f * a);

		if (closestT < hitDistance) {
			hitDistance = closestT;
			closestSphere = &sphere;
		}

		
	}

	
	if (closestSphere == nullptr) {
		return glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}
	
	glm::vec3 origin = ray.origin - closestSphere->position;

	glm::vec3 hitPoint = origin + ray.direction * hitDistance;

	glm::vec3 normal = glm::normalize(hitPoint);

	float dr = glm::max(glm::dot(normal, -m_LightDirection), 0.0f);

	
	return glm::vec4(closestSphere->color * dr, 1);
}
