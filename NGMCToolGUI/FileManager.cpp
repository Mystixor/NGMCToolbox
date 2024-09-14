#include "stdafx.h"

#include "FileManager.h"

namespace NGMC
{
	FileManager::FileManager()
		: m_Files(0), m_SelectedFiles(0)
	{
	}

	FileManager::~FileManager()
	{
	}

	void FileManager::RegisterFile(const wchar_t* filePath)
	{
		std::wstring tempWString = filePath;
		for (unsigned int i = 0; i < this->GetFileCount(); i++)
		{
			if (tempWString == m_Files[i].GetFilePath())
			{
				std::cout << "File \"" << m_Files[i].GetName() << "\", located at \"" << std::string(tempWString.begin(), tempWString.end()) << "\", was already registered." << std::endl;
				return;
			}
		}

		//if (GetSelectionCount())
		//{
		//	File* p_Files = &m_Files[0];
		//
		//	m_Files.emplace_back(filePath);
		//
		//	if (&m_Files[0] != p_Files)
		//	{
		//		for (unsigned int i = 0; i < GetSelectionCount(); i++)
		//		{
		//			m_SelectedFiles[i] = &m_Files[(m_SelectedFiles[i] - p_Files) / sizeof(File*)];
		//		}
		//	}
		//}
		//else
		//{
		//	m_Files.emplace_back(filePath);
		//}
		DeselectAllFiles();
		m_Files.emplace_back(filePath);

		m_Files.back().UpdateStatus();

		std::cout << "File \"" << m_Files[this->GetFileCount()-1].GetName() << "\", located at \"" << std::string(tempWString.begin(), tempWString.end()) << "\", is now registered." << std::endl;
	}

	void FileManager::RegisterFile(const char* filePath)
	{
		std::string tempString = filePath;
		std::wstring tempWString = std::wstring(tempString.begin(), tempString.end());
		RegisterFile(tempWString.c_str());
	}

	void FileManager::RegisterFile(MemoryBuffer& memoryBuffer, const char* name, FileType type)
	{
		//if (GetSelectionCount())
		//{
		//	File* p_Files = &m_Files[0];
		//
		//	m_Files.emplace_back(memoryBuffer, name, 0, nullptr, type);
		//
		//	if (&m_Files[0] != p_Files)
		//	{
		//		for (unsigned int i = 0; i < GetSelectionCount(); i++)
		//		{
		//			m_SelectedFiles[i] = &m_Files[(m_SelectedFiles[i] - p_Files) / sizeof(File*)];
		//		}
		//	}
		//}
		//else
		//{
		//	m_Files.emplace_back(memoryBuffer, name, 0, nullptr, type);
		//}
		DeselectAllFiles();
		m_Files.emplace_back(memoryBuffer, name, 0, nullptr, type);

		m_Files.back().UpdateStatus();

		std::cout << "File \"" << m_Files[this->GetFileCount() - 1].GetName() << "\", located in memory, is now registered." << std::endl;
	}

	size_t FileManager::GetFileCount() const
	{
		return m_Files.size();
	}

	size_t FileManager::GetSelectionCount() const
	{
		return m_SelectedFiles.size();
	}

	bool FileManager::IsFileSelected(File* p_File) const
	{
		return FindFileInSelected(p_File) != m_SelectedFiles.end();
	}

	File* FileManager::GetFile(unsigned int index)
	{
		return index < this->GetFileCount() ? &m_Files[index] : nullptr;
	}

	File* FileManager::GetSelectedFile(unsigned int index)
	{
		return index < this->GetSelectionCount() ? m_SelectedFiles[index] : nullptr;
	}

	bool FileManager::LoadFile(unsigned int index)
	{
		return index < this->GetFileCount() ? m_Files[index].Load() : false;
	}

	void FileManager::SelectFile(File* p_File, bool clearPrevSelection)
	{
		if (clearPrevSelection)
			DeselectAllFiles();
		m_SelectedFiles.push_back(p_File);
	}

	void FileManager::SelectAllChilds(File* p_File, bool clearPrevSelection)
	{
		if (clearPrevSelection)
			DeselectAllFiles();

		for (unsigned int i = 0; i < p_File->GetChildCount(); i++)
		{
			SelectFile(p_File->GetChild(i));
		}
	}

	void FileManager::SelectAllDescendants(File* p_File, bool clearPrevSelection)
	{
		if (clearPrevSelection)
			DeselectAllFiles();

		for (unsigned int i = 0; i < p_File->GetChildCount(); i++)
		{
			SelectFile(p_File->GetChild(i));
			SelectAllDescendants(p_File->GetChild(i));
		}
	}

	void FileManager::SelectAllFiles()
	{
		DeselectAllFiles();

		for (unsigned int i = 0; i < GetFileCount(); i++)
		{
			SelectFile(&m_Files[i]);
			SelectAllDescendants(&m_Files[i]);
		}
	}

	void FileManager::DeselectFile(File* p_File)
	{
		for (auto fileIt = FindFileInSelected(p_File); fileIt != m_SelectedFiles.end(); fileIt = FindFileInSelected(p_File))
		{
			m_SelectedFiles.erase(fileIt);
		}
	}

	void FileManager::DeselectAllFiles()
	{
		m_SelectedFiles.clear();
	}

	std::vector<File*>::const_iterator FileManager::FindFileInSelected(File* p_File) const
	{
		return std::find(m_SelectedFiles.begin(), m_SelectedFiles.end(), p_File);
	}
}