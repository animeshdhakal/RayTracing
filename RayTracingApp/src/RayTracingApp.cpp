#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include "Renderer.h"

using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
public:
	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");
		
		ImGui::Text("Last render: %.3f ms", m_LastRenderTime);
		ImGui::Text("Viewport: %dx%d", m_ViewportWidth, m_ViewportHeight);

		ImGui::ColorPicker3("ObjectColor", m_ObjectColor, ImGuiColorEditFlags_DisplayRGB);

		ImGui::SliderInt3("Direction of Light", &m_LightDirection[0], -10, 10);
		
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

		glm::vec3 color(m_ObjectColor[0], m_ObjectColor[1], m_ObjectColor[2]);
		m_Renderer.SetObjectColor(color);
		m_Renderer.setLightDirection(glm::vec3((float)m_LightDirection[0], (float)m_LightDirection[1], (float)m_LightDirection[2]));

		Render();
	}

	void Render()
	{
		Timer timer;

		m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Renderer.Render();

		m_LastRenderTime = timer.ElapsedMillis();
	}
private:
	Renderer m_Renderer;
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

	float m_LastRenderTime = 0.0f;
	float m_ObjectColor[3] = {1, 1, 1};
	
	int m_LightDirection[3] = { -1, -1, -1 };
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