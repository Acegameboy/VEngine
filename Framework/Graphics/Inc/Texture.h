#pragma once

namespace VEngine::Graphics
{
	class Texture
	{
	public:
		enum class Format
		{
			RGBA_U8,
			RGBA_U32,
		};

		static void UnBindPS(uint32_t slot);

		Texture() = default;
		virtual ~Texture();

		//allow copies
		Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;

		//allow moves
		Texture(Texture&&) noexcept;
		Texture& operator=(Texture&&) noexcept;

		virtual void Initialize(const std::filesystem::path& fileName);
		virtual void Terminate();

		void BindVS(uint32_t slot) const;
		void BindPS(uint32_t slot) const;

		void* GetRawData() const;
	protected:
		ID3D11ShaderResourceView* mShaderResourceView = nullptr;
	};
}
