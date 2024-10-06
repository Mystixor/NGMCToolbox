#pragma once

#include "BasePreview.h"

#include "utility/DDS.h"

namespace NGMC
{
    class BasePreviewImage : public BasePreview
    {
	public:
		BasePreviewImage(File& file);

		~BasePreviewImage();

		virtual void OnRender() override = 0;

	protected:
		//	The function used to upload the specified raw image data with the specified image width, image height and pixel format to the GPU.
		void UploadTexture(unsigned char* imageData, int imageWidth, int imageHeight, DDSFormat pixelFormat);

		//	The function used to free the texture that is currently uploaded on the GPU.
		void FreeTexture();

		//	The width/height of the canvas on which the image used by the previewer is drawn.
		int m_ImageDisplayWidth, m_ImageDisplayHeight;

		//	The value referring internally on the GPU to the image potentially used on the previewer window.
		void* m_Image;

    private:
		//	Whether an image is currently in use by the previewer.
		bool m_ImageInUse;

		//	The width/height of the image used by the previewer.
		int m_ImageWidth, m_ImageHeight;
    };
}