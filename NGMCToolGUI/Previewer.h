#pragma once

#include "utility/Utility.h"
#include "File.h"

#include "previews/PreviewDatabin.h"
#include "previews/PreviewDatabinItem.h"
#include "previews/PreviewGT1G.h"
#include "previews/PreviewDDS.h"
#include "previews/PreviewTMC.h"

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
		//	The polymorphic Preview object pointer used to handle previews of different file types.
		BasePreview* m_Preview;

		//	The pointer to the file object which is currently being previewed.
		File* m_File;

		//	Whether the the file behind the pointer m_File is currently setup for previewing.
		bool m_IsSetup;
	};
}