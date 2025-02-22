#pragma once

#include "DataReader.h"

namespace NGMC
{
	namespace SaveFile
	{
		enum Type
		{
			unknown,
			SYSTEMSAVE_S1,	//	NG Sigma 1: System save
			GAMESAVE_S1,	//	NG Sigma 1: Game save
			SYSTEMSAVE_S2,	//	NG Sigma 2:	System save
			STORYSAVE_S2,	//	NG Sigma 2:	Story save
			SYSTEMSAVE_2B,	//	NG 2 Black:	System save
			STORYSAVE_2B,	//	NG 2 Black:	Story save
			SYSTEMSAVE_3RE	//	NG 3 RE:	System save
		};
	}

	class LoaderSave
	{
	public:
		LoaderSave(const wchar_t* filePath, GAME game);

		LoaderSave(MemoryBuffer& memBuf, GAME game);

		LoaderSave(File& file);

		~LoaderSave();

		SaveFile::Type DetermineSaveType();

	private:
		DataReader m_Reader;

		GAME m_Game;
	};
}