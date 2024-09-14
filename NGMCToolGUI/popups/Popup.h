#pragma once

#include "stdafx.h"

#include "File.h"

namespace NGMC
{
	//	The available types of Popups
	enum PopupType
	{
		TypePopupBase,
		TypePopupBuildGT1G,
		TypePopupRenameFile,
		TypePopupFilePicker,
		TypePopupAbout,
		TypePopupSelectGame
	};

	//	Base class of popups, to be inherited from
	class Popup
	{
	public:
		//	Constructs a Popup object by giving it a name, visible in the window title.
		Popup(const char* popupName);

		//	The function rendering the Popup on every frame where it is opened.
		void OnRender();

		//	Opens the Popup window.
		void Open();

		//	Closes the Popup window.
		void Close();

		//	Returns whether the file referenced by the Popup object is the same as the one specified.
		bool IsSameFile(File* p_File) const;

		//	Returns whether the Popup window is opened.
		bool IsOpen() const;

		//	Returns the type of the Popup.
		PopupType GetType() const;

	protected:
		//	Returns the pointer to the file object referenced by the Popup.
		File* GetFile() const;

		//	Sets the pointer to the file object referenced by the Popup to the specified file object pointer.
		void SetFile(File* p_File);

		//	Marks the Popup object as ready for rendering.
		void MarkAsReady();

		//	The type of the Popup.
		PopupType m_Type;

		//	The minimum size of the Popup window in x- and y-direction.
		ImVec2 m_WindowMinSize;

		//	The maximum size of the Popup window in x- and y-direction.
		ImVec2 m_WindowMaxSize;

	private:
		//	The virtual function that renders the contents of the Popup window. To be implemented by child classes.
		virtual void OnRenderContent();

		//	The name of the Popup object, visible in the window title.
		std::string m_PopupName;

		//	Whether the Popup window is opened.
		bool m_Open;

		//	Whether the Popup window is ready for rendering.
		bool m_IsReady;

		//	The pointer to a file object that might be used by the Popup.
		File* m_File;
	};
}