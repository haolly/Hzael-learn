﻿#pragma once
#include <cstdint>
#include "Hazel/Renderer/Texture.h"
#include <imgui.h>
#include <imgui_internal.h>

namespace Hazel::UI
{
	static int s_UIContextID = 0;
	static uint32_t s_Counter = 0;
	static char s_IDBuffer[16];

	static void PushID()
	{
		ImGui::PushID(s_UIContextID++);
		s_Counter = 0;
	}

	static void PopID()
	{
		ImGui::PopID();
		s_UIContextID--;
	}

	static void BeginPropertyGrid()
	{
		PushID();
		ImGui::Columns(2);
	}

	static void EndPropertyGrid()
	{
		ImGui::Columns(1);
		PopID();
	}

	static bool TreeNode(const std::string& id, const std::string& label, ImGuiTreeNodeFlags flags = 0)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if(window->SkipItems)
			return false;
		return ImGui::TreeNodeBehavior(window->GetID(id.c_str()), flags, label.c_str(), NULL);
	}

	/*
	bool ImageButton(const Ref<Image2D>& image, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0),
	                 const ImVec2 uv1 = ImVec2(1, 1),
	                 int frame_padding = -1, const ImVec4& bg_col = ImVec4(0, 0, 0, 0),
	                 const ImVec4& tint_col = ImVec4(1, 1, 1, 1));
	                 */
	bool ImageButton(const Ref<Texture2D>& texture, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0),
	                 const ImVec2& uv1 = ImVec2(1, 1),
	                 int frame_padding = -1, const ImVec4& bg_col = ImVec4(0, 0, 0, 0),
	                 const ImVec4& tint_col = ImVec4(1, 1, 1, 1));
	bool ImageButton(const char* stringID, const Ref<Texture2D>& texture, const ImVec2& size,
	                 const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1),
	                 int frame_padding = -1, const ImVec4& bg_col = ImVec4(0, 0, 0, 0),
	                 const ImVec4& tint_col = ImVec4(1, 1, 1, 1));

}