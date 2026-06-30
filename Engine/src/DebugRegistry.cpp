#include "../include/DebugRegistry.hpp"
#include <string>
#include <functional>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>

void DebugRegistry::registerMember(std::string member_name, std::function<void()> getter) {
	members[member_name] = getter;
}

void DebugRegistry::removeMember(std::string& member_name) {
	members.erase(member_name);
}

void DebugRegistry::renderMemberData() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Debug");
	for (const auto& [name, func] : members) {	
		if (ImGui::CollapsingHeader(name.c_str())) {
			func();
		}
	}
	ImGui::End();

	ImGui::Render();
	ImGui::GetDrawData();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
