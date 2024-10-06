#pragma once

#include "File.h"

namespace NGMC
{
	namespace Preview
	{
		//	Default flags for ImGui Tables used across all file previews.
		constexpr ImGuiTableFlags tableFlags =
			ImGuiTableFlags_RowBg |
			ImGuiTableFlags_SizingFixedFit |
			ImGuiTableFlags_NoHostExtendX |
			ImGuiTableFlags_BordersInnerV |
			ImGuiTableFlags_BordersOuter;
	}


	//	Base class for all File Previews.
	class BasePreview
	{
	public:
		//	Constructs a Preview object by assigning a File object to be previewed.
		BasePreview(File& file);

		//	Deconstructs the Preview object.
		virtual ~BasePreview() = default;

		//	Returns whether the Preview object was properly setup using the File object associated with it.
		bool IsSetup() const;

		//	The function drawing the UI specific to a file type once per frame.
		virtual void OnRender() = 0;

		//	The function called to prepare the preview data, returns whether preparation was completed successfully.
		virtual bool Prepare();

	protected:
		//	The reference to the file object to preview.
		File& m_File;

		//	Whether this Preview object is properly setup for previewing.
		bool m_IsSetup;
	};
}