#pragma once

#include "utility/Utility.h"
#include "File.h"
#include "loaders/LoaderDatabin.h"
#include "loaders/LoaderGT1G.h"
#include "loaders/LoaderDDS.h"

namespace NGMC
{
	//	The class used to handle the contents of the Preview window.
	class Previewer
	{
	public:
		//	Constructs a Previewer object by setting all members to default values.
		Previewer();

		//	Deconstructs the Previewer object.
		~Previewer();

		//	Sets the file shown by the Previewer object to the specified file pointer. Frees the preview texture and reinitializes the Previewer object.
		void SetFile(File* p_File);

		//	The function called every frame that the Previewer window is visible, draws all UI.
		void OnRender();

		//	The function called to prepare a file object for previewing on the next frame where the Previewer window is visible after the preview file changed, returns whether preparation was completed successfully.
		bool Prepare();
		
	private:
		//	The function called to prepare a databin file object for previewing, returns whether preparation was completed successfully.
		bool PrepareDatabin();

		//	The function called to prepare a compressed databin item for previewing, returns whether preparation was completed successfully.
		bool PrepareDatabinItem();

		//	The function called to prepare a GT1G file object for previewing, returns whether preparation was completed successfully.
		bool PrepareGT1G();

		//	The function called to prepare the GT1G texture of the currently previewed GT1G file object at the specified textuer index for previewing, returns whether preparation was completed successfully.
		bool PrepareGT1GTexture(unsigned int textureIndex);

		//	The function called to prepare a DDS file object for previewing, returns whether preparation was completed successfully.
		bool PrepareDDS();

		//	The function used to upload the specified raw image data with the specified image width, image height and pixel format to the GPU.
		void UploadTexture(unsigned char* imageData, int imageWidth, int imageHeight, DDSFormat pixelFormat);

		//	The function used to free the texture that is currently uploaded on the GPU.
		void FreeTexture();

		//	The function called every frame to draw the UI of a databin file object.
		void OnRenderDatabin();

		//	The function called every frame to draw the UI of a compressed databin item.
		void OnRenderDatabinItem();

		//	The function called every frame to draw the UI of a GT1G file object.
		void OnRenderGT1G();

		//	The function called every frame to draw the UI of a DDS file object.
		void OnRenderDDS();

		//	The pointer to the file object which is currently being previewed.
		File* m_File;

		//	The type of the file which was previewed on the previous frame.
		FileType m_PrevType;

		//	Whether the the file behind the pointer m_File is currently setup for previewing.
		bool m_IsSetup;

		//	The value referring internally on the GPU to the image potentially used on the previewer window.
		void* m_Image;

		//	Whether an image is currently in use by the previewer.
		bool m_ImageInUse;

		//	The width/height of the image used by the previewer.
		int m_ImageWidth, m_ImageHeight;

		//	The width/height of the canvas on which the image used by the previewer is drawn.
		int m_ImageDisplayWidth, m_ImageDisplayHeight;



		//	databin	///////////////////////////////////////

		//	The header of a databin file.
		Databin::Header m_DatabinHeader;



		//	databinItem	///////////////////////////////////

		//	The header of a compressed databin item from Ninja Gaiden Sigma 1.
		Databin::S1::ItemHeader m_DatabinItemHeaderS1;
		
		//	The header of a compressed databin item from Ninja Gaiden Sigma 2.
		Databin::S2::ItemHeader m_DatabinItemHeaderS2;



		//	GT1G	///////////////////////////////////////

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



		//	DDS	///////////////////////////////////////////

		//	The header of a DDS file.
		DDS_HEADER m_DDS_HEADER;

		//	The mip level of a DDS texture which is being currently previewed.
		unsigned int m_PreviewDDSMipMapIdx;
	};
}