#pragma once

#include "Popup.h"

#include "loaders/LoaderGT1G.h"

#include "PopupFilePicker.h"

namespace NGMC
{
	//	Class for rendering and handling a popup window used to build custom GT1G files.
	class PopupBuildGT1G : public Popup
	{
	public:
		//	Constructs a GT1G Builder Popup object.
		PopupBuildGT1G();

		//	Sets up the popup by reserving memory and creating some buffers, then sets the popup as ready.
		void Setup();

	private:
		//	The overridden function that renders the contents of the Popup window.
		void OnRenderContent() override;

		//	The function that updates the sizes of various buffers to properly reflect the count of textures.
		void UpdateBufferSizes();

		//	The name of the final GT1G file.
		std::string m_OutputName;

		//	The count of textures to be put into the final GT1G file.
		unsigned int m_TextureCount;


		//	List of a bool for every texture on whether it is using file paths, i.e. it's loaded from disk, as opposed to being loaded from the file list in memory.
		std::vector<bool> m_IsUsingTextureFilePaths;

		//	List of a wide char string file path to every texture which is to be loaded from disk, respective element is unused if the texture is to be loaded from memory.
		std::vector<std::wstring> m_TextureFilePaths;

		//	List of a string file path of every texture which is to be loaded from disk, respective element is unused if the texture is to be loaded from memory. This serves only the purpose of displaying the path in ImGui and it DOES NOT support UTF-16 strings (i.e. the Sigma-symbol in the game files), if you select a UTF-16 file path with the Browse button it won't display correctly but it will still work!
		std::vector<std::string> m_TextureFilePathDisplays;

		//	List of a DDS file object pointer for every texture which is to e loaded from memory, respective element is unused if the texture is to be loaded from disk.
		std::vector<File*> m_TextureFilePointers;

		//	List of the image width of every texture.
		std::vector<unsigned int> m_Widths;

		//	List of the image height of every texture.
		std::vector<unsigned int> m_Heights;

		//	List of the count of mipmaps of every texture.
		std::vector<unsigned int> m_MipMapCounts;

		//	List of the pixel format ID of every texture, used only to display the correct format name in the UI.
		std::vector<unsigned int> m_FormatIDs;

		//	List of the PixelFormat of every texture.
		std::vector<GT1G::PixelFormat> m_Formats;

		//	The File Picker Popup object which handles the selection of files loaded in memory.
		PopupFilePicker m_FilePicker;
	};
}