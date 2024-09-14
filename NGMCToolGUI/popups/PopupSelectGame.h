#pragma once

#include "Popup.h"

namespace NGMC
{
	//	Class for rendering and handling a popup window used to rename a file object.
	class PopupSelectGame : public Popup
	{
	public:
		//	Constructs a Game Select Popup.
		PopupSelectGame();

		//	Sets up the Game Select Popup by specifying the file object pointer of the file object to be assigned a game.
		void Setup(File* p_File);

	private:
		//	The overridden function that renders the contents of the Popup window.
		void OnRenderContent() override;
	};
}