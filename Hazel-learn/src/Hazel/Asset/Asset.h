#pragma once
#include "AssetTypes.h"

namespace Hazel
{
	//todo
	using AssetHandle = uint32_t;
	
	/**
	 * \brief Mesh, Texture, Scene is Asset, but Shader, Material, Vertex/Index buffer is not , TODO: why?
	 */
	class Asset : public RefCounted
	{
	public:
		AssetHandle Handle;
		uint16_t Flags = (uint16_t)AssetFlag::None;

		virtual ~Asset() {}
		virtual AssetType GetAssetType() const { return AssetType::None;}
		virtual bool operator==(const Asset& other) const
		{
			return Handle == other.Handle;
		}

		virtual bool operator!=(const Asset& other) const
		{
			return !(other == *this);
		}

		bool IsFlagSet(AssetFlag flag) const
		{
			return (uint16_t)flag & Flags;
		}

		void SetFlag(AssetFlag flag, bool value)
		{
			if(value)
				Flags |= (uint16_t)flag;
			else
				Flags &= ~(uint16_t)flag;
		}
	};
}
