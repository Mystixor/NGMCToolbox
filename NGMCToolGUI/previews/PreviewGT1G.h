#pragma once

#include "BasePreviewImage.h"

#include "loaders/LoaderGT1G.h"

namespace NGMC
{
    class PreviewGT1G final : public BasePreviewImage
    {
    public:
		PreviewGT1G(File& file);

        void OnRender() override;

    private:
        bool Prepare() override;

		bool PrepareGT1GTexture(unsigned int textureIndex);


		//	The struct containing information on a mipmap of a GT1G texture.
		struct GT1GMipMap
		{
			unsigned int Width, Height;
		};

		//	The struct containing information on a GT1G texture.
		struct GT1GTexture
		{
			unsigned int MipMapCount;
			GT1G::PixelFormat Format;
			uint32_t Flags;
			uint32_t ExtraFlags0;
			uint32_t ExtraFlags1;
			uint32_t ExtraFlags2;
			std::vector<GT1GMipMap> MipMaps;
		};

		//	The header of a GT1G file.
		GT1G::Header m_GT1GHeader;

		//	The list of structs with information on the textures found in a GT1G file.
		std::vector<GT1GTexture> m_GT1GTextures;

		//	The texture index of the texture in a GT1G file which is being currently previewed.
		unsigned int m_PreviewGT1GTexIdx;

		//	The mip level of the texture in a GT1G file which is being currently previewed.
		unsigned int m_PreviewGT1GMipMapIdx;
    };
}