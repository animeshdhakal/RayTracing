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
	m_ActiveCamera = &camera;
	m_ActiveScene = &scene;

	
	for (int y = 0; y < m_FinalImage->GetHeight(); y++) {
		for (int x = 0; x < m_FinalImage->GetWidth(); x++) {
						
			glm::vec4 color = PerPixel(x, y);

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

Renderer::HitPayload Renderer::TraceRay(const Ray& ray)
{	
	if (m_ActiveScene->spheres.size() == 0)
		return Miss(ray);

	
	int closestSphere = -1;
	float hitDistance = FLT_MAX;

	for (int i = 0; i < m_ActiveScene->spheres.size(); i++) {
		const Sphere& sphere = m_ActiveScene->spheres[i];

		glm::vec3 origin = ray.origin - sphere.position;
		
		float a = glm::dot(ray.direction, ray.direction);
		float b = 2.0f * glm::dot(origin, ray.direction);
		float c = glm::dot(origin, origin) - sphere.radius * sphere.radius;
		float d = b * b - 4.0f * a * c;

		if (d < 0.0f) {
			continue;
		}

		float closestT = (-b - glm::sqrt(d)) / (2.0f * a);

		if (closestT > 0.0f && closestT < hitDistance) {
			hitDistance = closestT;
			closestSphere = i;
		}
		
	}

	
	if (closestSphere < 0) {
		return Miss(ray);
	}

	return ClosestHit(ray, hitDistance, closestSphere);
}

Renderer::HitPayload Renderer::ClosestHit(const Ray& ray, float hitDistance, int closestSphere)
{
	Renderer::HitPayload payload = {};

	payload.hitDistance = hitDistance;
	payload.objectIndex = closestSphere;
	

	glm::vec3 origin = ray.origin - m_ActiveScene->spheres[closestSphere].position;

	payload.worldPosition = origin + ray.direction * hitDistance;
	payload.worldNormal = glm::normalize(payload.worldPosition);


	payload.worldPosition += m_ActiveScene->spheres[closestSphere].position;

	return payload;
}


glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y) {
	Ray ray;
	ray.origin = m_ActiveCamera->GetPosition();
	ray.direction = m_ActiveCamera->GetRayDirections()[x + y * m_FinalImage->GetWidth()];

	glm::vec3 color(0.0f);
	float multiplier = 1.0f;

	int bounces = 2;
	for (int i = 0; i < bounces; i++)
	{
		Renderer::HitPayload payload = TraceRay(ray);
		if (payload.hitDistance < 0.0f)
		{
			glm::vec3 skyColor = glm::vec3(0.0f, 0.0f, 0.0f);
			color += skyColor * multiplier;
			break;
		}

		glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));
		float lightIntensity = glm::max(glm::dot(payload.worldNormal, -lightDir), 0.0f); // == cos(angle)

		const Sphere& sphere = m_ActiveScene->spheres[payload.objectIndex];
		glm::vec3 sphereColor = sphere.color;
		sphereColor *= lightIntensity;
		color += sphereColor * multiplier;

		multiplier *= 0.7f;

		ray.origin = payload.worldPosition + payload.worldNormal * 0.0001f;
		ray.direction = glm::reflect(ray.direction, payload.worldNormal);
	}

	return glm::vec4(color, 1.0f);
}

Renderer::HitPayload Renderer::Miss(const Ray& ray) {
	Renderer::HitPayload payload = {};
	payload.hitDistance = -1;
	return payload;
}