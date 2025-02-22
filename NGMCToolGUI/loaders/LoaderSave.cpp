#include "stdafx.h"

#include "LoaderSave.h"

namespace NGMC
{
	LoaderSave::LoaderSave(const wchar_t* filePath, GAME game)
		: m_Game(game)
	{
		m_Reader.Init(filePath);
	}

	LoaderSave::LoaderSave(MemoryBuffer& memBuf, GAME game)
		: m_Game(game)
	{
		m_Reader.Init(memBuf);
	}

	LoaderSave::LoaderSave(File& file)
		: m_Game(file.GetType().GetGame())
	{
		m_Reader.Init(file);
	}

	LoaderSave::~LoaderSave()
	{
	}

	SaveFile::Type LoaderSave::DetermineSaveType()
	{
		std::streamoff pos = m_Reader.Tell();


		SaveFile::Type saveType = SaveFile::Type::unknown;

		switch (m_Game)
		{
		case SIGMA_1:
		{
			m_Reader.Seek(0, MemoryBuffer::beg);

			std::wstring saveName = m_Reader.ReadWString();

			if (wcsncmp(saveName.c_str(), L"System Save", 11) == 0)
			{
				saveType = SaveFile::Type::SYSTEMSAVE_S1;
			}
			else if (wcsncmp(saveName.c_str(), L"Game Save", 9) == 0)
			{
				saveType = SaveFile::Type::GAMESAVE_S1;
			}
			break;
		}
		case SIGMA_2:
		{
			m_Reader.Seek(0, MemoryBuffer::beg);

			if (m_Reader.ReadInt32() == 'SAVG')
			{
				//	NG 2 Black
				m_Reader.Seek(0x6AB, MemoryBuffer::beg);

				std::string saveName = m_Reader.ReadString();

				if (strncmp(saveName.c_str(), "System Preferences", 18) == 0)
				{
					saveType = SaveFile::Type::SYSTEMSAVE_2B;
				}
				else if (strncmp(saveName.c_str(), "Saved Game", 10) == 0)
				{
					saveType = SaveFile::Type::STORYSAVE_2B;
				}
			}
			else
			{
				//	NG Sigma 2
				m_Reader.Seek(0, MemoryBuffer::beg);

				std::wstring saveName = m_Reader.ReadWString();

				if (wcsncmp(saveName.c_str(), L"System Preferences", 18) == 0)
				{
					saveType = SaveFile::Type::SYSTEMSAVE_S2;
				}
				else if (wcsncmp(saveName.c_str(), L"Saved Game", 10) == 0)
				{
					saveType = SaveFile::Type::STORYSAVE_S2;
				}
			}
			break;
		}
		case RE_3:
		{
			m_Reader.Seek(0, MemoryBuffer::beg);

			std::wstring saveName = m_Reader.ReadWString();

			if (wcsncmp(saveName.c_str(), L"SYSTEM DATA", 11) == 0)
			{
				saveType = SaveFile::Type::SYSTEMSAVE_3RE;
			}
			break;
		}
		}


		m_Reader.Seek(pos, MemoryBuffer::beg);

		return saveType;
	}
}