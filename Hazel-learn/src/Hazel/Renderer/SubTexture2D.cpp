#include "hazelPCH.h"
#include "SubTexture2D.h"

namespace Hazel
{
	SubTexture2D::SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max)
		: m_Texture(texture)
	{
		m_TextureCoords[0] = {min.x, min.y};
		m_TextureCoords[1] = {max.x, min.y};
		m_TextureCoords[2] = {max.x, max.y};
		m_TextureCoords[3] = {min.x, max.y};
	}

	// coords is the coordinates in sprite sheet
	Ref<SubTexture2D> SubTexture2D::CreateFromCoords(const Ref<Texture2D>& texture, const glm::vec2& coords, const glm::vec2& spriteSize)
	{
		float textureWidth = texture->GetWidth();
		float textureHeight = texture->GetHeight();
		glm::vec2 min = {coords.x / textureWidth, coords.y / textureHeight};
		glm::vec2 max = {(coords.x + spriteSize.x) / textureWidth, (coords.y + spriteSize.y) / textureHeight};

		return  CreateRef<SubTexture2D>(texture, min, max);
	}
}
