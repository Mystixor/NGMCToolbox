#pragma once

#include "Popup.h"

namespace NGMC
{
	//	Class for rendering and handling a popup window used to display information about the application.
	class PopupAbout : public Popup
	{
	public:
		//	Constructs an About Popup.
		PopupAbout();

		//	Sets up the About Popup.
		void Setup();

	private:
		//	The overridden function that renders the contents of the Popup window.
		void OnRenderContent() override;
	};
}