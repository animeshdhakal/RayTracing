#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"
#include "Walnut/Image.h"
#include "Walnut/Timer.h"
#include "Renderer.h"
#include "Camera.h"
#include "Scene.h"
#include <glm/gtc/type_ptr.hpp>

using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
public:
	ExampleLayer() : m_Camera(45.0f, 0.1f, 100.0f), m_LightDirection(-1, -1, -1) {
		Sphere sphere;
		sphere.position = { 0.0f, 0.0f, 0.0f };
		sphere.radius = 0.5f;
		sphere.color = { 1.0f, 0.0f, 1.0f };
		m_Scene.spheres.push_back(sphere);

		sphere.position = { 1.0f, 0.0f, -5.0f };
		sphere.radius = 1.5f;
		sphere.color = { 0.2f, 0.3f, 1.0f };
		m_Scene.spheres.push_back(sphere);
	}

	virtual void OnUpdate(float ts) override {
		m_Camera.OnUpdate(ts);
	}

	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");
		
		ImGui::Text("Last render: %.3f ms", m_LastRenderTime);
		ImGui::Text("Viewport: %dx%d", m_ViewportWidth, m_ViewportHeight);

		ImGui::SliderFloat3("Direction of Light", glm::value_ptr(m_LightDirection), -10, 10);

		for (size_t i = 0; i < m_Scene.spheres.size(); i++)
		{
			ImGui::PushID(i);

			Sphere& sphere = m_Scene.spheres[i];
			ImGui::DragFloat3("Position", glm::value_ptr(sphere.position), 0.1f);
			ImGui::DragFloat("Radius", &sphere.radius, 0.1f);
			ImGui::ColorEdit3("Color", glm::value_ptr(sphere.color));

			ImGui::Separator();

			ImGui::PopID();
		}
		
		// Simple Way to Check Endianness of Machine
		
		/*std::string machinetype = "Big Endian";
		int n = 1;
		
		if (*(char*)&n == 1) {
			machinetype = "Little Endian";
		}

		ImGui::Text("Machine Type: %s", machinetype.c_str());*/

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");

		m_ViewportWidth = ImGui::GetContentRegionAvail().x;
		m_ViewportHeight = ImGui::GetContentRegionAvail().y;

		auto image = m_Renderer.GetFinalImage();
		if (image)
			ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(), (float)image->GetHeight() },
				ImVec2(0, 1), ImVec2(1, 0));

		ImGui::End();
		ImGui::PopStyleVar();

		m_Renderer.setLightDirection(m_LightDirection);

		Render();
	}

	void Render()
	{
		Timer timer;

		m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Renderer.Render(m_Scene, m_Camera);

		m_LastRenderTime = timer.ElapsedMillis();
	}
private:
	Renderer m_Renderer;
	Camera m_Camera;
	Scene m_Scene;
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
	float m_LastRenderTime = 0.0f;
	glm::vec3 m_LightDirection;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Ray Tracing";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}