#include "hazelPCH.h"
#include "Hazel/ImGui/ImGui.h"
#include "Hazel/Renderer/RendererAPI.h"
#include "Platform/OpenGL/OpenGLImage2D.h"

namespace Hazel::UI
{
	bool ImageButton(const Ref<Texture2D>& texture, const ImVec2& size, const ImVec2& uv0,
	                 const ImVec2& uv1,
	                 int frame_padding, const ImVec4& bg_col,
	                 const ImVec4& tint_col)
	{
		return ImageButton(nullptr, texture, size, uv0, uv1, frame_padding, bg_col, tint_col);
	}

	bool ImageButton(const char* stringID, const Ref<Texture2D>& texture, const ImVec2& size,
	                 const ImVec2& uv0, const ImVec2& uv1,
	                 int frame_padding, const ImVec4& bg_col,
	                 const ImVec4& tint_col)
	{
		HZ_CORE_ASSERT(texture);
		if(!texture)
			return false;

		if(RendererAPI::GetAPI() == RendererAPIType::OpenGL)
		{
			Ref<OpenGLImage2D> image = texture->GetImage().As<OpenGLImage2D>();
			return ImGui::ImageButton((ImTextureID)(size_t)image->GetRendererID(), size, uv0, uv1, frame_padding, bg_col, tint_col);
		}
		else
		{
			return false;
		}

	}
}
