#pragma once

namespace Hazel 
{
	enum class ShaderDataType
	{
		None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool,
	};

	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		case Hazel::ShaderDataType::None:
			HZ_CORE_ASSERT(false, "Invalide ShaderDataType");
			break;
		case Hazel::ShaderDataType::Float:
			return 4;
			break;
		case Hazel::ShaderDataType::Float2:
			return 4 * 2;
			break;
		case Hazel::ShaderDataType::Float3:
			return 4 * 3;
			break;
		case Hazel::ShaderDataType::Float4:
			return 4 * 4;
			break;
		case Hazel::ShaderDataType::Mat3:
			return 4 * 3 * 3;
			break;
		case Hazel::ShaderDataType::Mat4:
			return 4 * 4 * 4;
			break;
		case Hazel::ShaderDataType::Int:
			return 4;
			break;
		case Hazel::ShaderDataType::Int2:
			return 4 * 2;
			break;
		case Hazel::ShaderDataType::Int3:
			return 4 * 3;
			break;
		case Hazel::ShaderDataType::Int4:
			return 4 * 4;
			break;
		case Hazel::ShaderDataType::Bool:
			return 1;
			break;
		}
		HZ_CORE_ASSERT(false, "Invalide ShaderDataType");
		return 0;
	}

	struct VertexBufferElement
	{
		std::string Name;
		ShaderDataType Type;
		uint32_t Size;
		uint32_t Offset;
		bool Normalized;

		VertexBufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
			: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
		{
		}

		uint32_t GetComponentCount() const 
		{
			switch (Type)
			{
			case Hazel::ShaderDataType::None:
				HZ_CORE_ASSERT(false, "Invalide Type");
				break;
			case Hazel::ShaderDataType::Float:
				return 1;
				break;
			case Hazel::ShaderDataType::Float2:
				return 2;
				break;
			case Hazel::ShaderDataType::Float3:
				return 3;
				break;
			case Hazel::ShaderDataType::Float4:
				return 4;
				break;
			case Hazel::ShaderDataType::Mat3:
				return 3 * 3;
				break;
			case Hazel::ShaderDataType::Mat4:
				return 4 * 4;
				break;
			case Hazel::ShaderDataType::Int:
				return 1;
				break;
			case Hazel::ShaderDataType::Int2:
				return 2;
				break;
			case Hazel::ShaderDataType::Int3:
				return 3;
				break;
			case Hazel::ShaderDataType::Int4:
				return 4;
				break;
			case Hazel::ShaderDataType::Bool:
				return 1;
				break;
			}
			HZ_CORE_ASSERT(false, "Unknow ShaderDataType");
		}
	};

	class VertexBufferLayout 
	{
	public:
		VertexBufferLayout() = default;
		VertexBufferLayout(const std::initializer_list<VertexBufferElement>& elements)
			: m_Elements(elements)
			{
				CalculateOffsetAndStride();
			}
		inline const std::vector<VertexBufferElement>& GetElements() const { return m_Elements; }
		// forloop need this
		std::vector<VertexBufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<VertexBufferElement>::const_iterator end()  const { return m_Elements.end(); }
		uint32_t GetStride() const { return m_Stride; }
	private:
		void CalculateOffsetAndStride()
		{
			uint32_t offset = 0;
			for (auto& element : m_Elements)
			{
				element.Offset = offset;
				offset += element.Size;
				m_Stride += element.Size;
			}
		}
	private:
		std::vector<VertexBufferElement> m_Elements;
		uint32_t m_Stride = 0;
	};

	class VertexBuffer : public RefCounted
	{
	public:
		virtual ~VertexBuffer() {}

		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		virtual void SetData(const void* data, uint32_t dataSize) = 0;

		virtual void SetLayout(const VertexBufferLayout& layout) = 0;
		virtual const VertexBufferLayout& GetLayout() const = 0;

		static Ref<VertexBuffer> Create(uint32_t size);
		static Ref<VertexBuffer> Create(float* vertices, uint32_t size);
	};

	class IndexBuffer : public RefCounted
	{
	public:
		virtual ~IndexBuffer() {}

		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;
		virtual uint32_t GetCount() const = 0;

		static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t count);
	};
}
