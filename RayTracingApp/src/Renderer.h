#pragma once

#include "Walnut/Image.h"
#include <memory>
#include <glm/glm.hpp>
#include <iostream>
#include <chrono>
#include "Camera.h"
#include "Ray.h"
#include "Scene.h"

namespace Utils {
	uint32_t ConvertToRGBA(glm::vec4 color);
}

class Renderer
{
public:
	Renderer() : m_LightDirection(-1) {}
	void Render(const Scene& scene, const Camera& camera);
	void OnResize(uint32_t width, uint32_t height);
	glm::vec4 TraceRay(const Scene& scene, const Ray& ray);
	std::shared_ptr<Walnut::Image> GetFinalImage() { return m_FinalImage; }

	void setLightDirection(glm::vec3 lightDirection) {
		m_LightDirection = lightDirection;
	}
private:
	struct HitPayload {
		float hitDistance;
		glm::vec3 worldPosition;
		glm::vec3 worldNormal;
		
		int objectIndex;
	};
	
	// Also Known as Raygen in Vulkan and DirectX
	glm::vec4 PerPixel(uint32_t x, uint32_t y);
	HitPayload TraceRay(const Ray& ray);
	HitPayload ClosestHit(const Ray& ray, float hitDistance, int objectIndex);
	HitPayload Miss(const Ray& ray);

private:
	std::shared_ptr<Walnut::Image> m_FinalImage;
	uint32_t* m_ImageData = nullptr;
	glm::vec3 m_LightDirection;

	const Scene* m_ActiveScene = nullptr;
	const Camera* m_ActiveCamera = nullptr;
};

