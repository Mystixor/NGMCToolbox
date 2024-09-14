#pragma once

#include "Popup.h"

namespace NGMC
{
	//	Class for rendering and handling a popup window used to rename a file object.
	class PopupRenameFile : public Popup
	{
	public:
		//	Constructs a File Renaming Popup.
		PopupRenameFile();

		//	Sets up the File Renaming Popup by specifying the file object pointer of the file object to be renamed.
		void Setup(File* p_File);

	private:
		//	The overridden function that renders the contents of the Popup window.
		void OnRenderContent() override;

		//	The string holding the new name of the file object.
		std::string m_NewName;
	};
}