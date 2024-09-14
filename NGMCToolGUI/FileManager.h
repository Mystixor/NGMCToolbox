#pragma once

#include "stdafx.h"

#include "File.h"

namespace NGMC
{
	class FileManager
	{
	public:
		//	Constructs a File Manager object with 0 files and 0 selected files.
		FileManager();

		//	Deconstructs a File Manager object.
		~FileManager();

		//	Registers a file by constructing a file object from disk using a wide char string.
		void RegisterFile(const wchar_t* filePath);

		//	Registers a file by constructing a file object from disk using a string which gets converted to a wide char string.
		void RegisterFile(const char* filePath);

		//	Registers a file by constructing a file object from memory, a name and optionally a type.
		void RegisterFile(MemoryBuffer& memoryBuffer, const char* name, FileType type = FileType());

		//	Returns the count of files registered on the top-level.
		size_t GetFileCount() const;

		//	Returns the count of currently selected files.
		size_t GetSelectionCount() const;

		//	Returns whether the file with the specified address could be found in the list of selected files.
		bool IsFileSelected(File* p_File) const;

		//	Returns the file at the specified index on the top-level, returns nullptr if index is out of bounds.
		File* GetFile(unsigned int index);

		//	Returns	the file at the specified index in the list of selected files, returns nullptr if index is out of bounds.
		File* GetSelectedFile(unsigned int index);

		//	Loads the file at the specified index on the top-level, returns whether the loading succeeded.
		bool LoadFile(unsigned int index);

		//	Select the file with the specified address, can be configured to clear the current selection.
		void SelectFile(File* p_File, bool clearPrevSelection = false);

		//	Selects all children of the file with the specified address, can be configured to clear the current selection.
		void SelectAllChilds(File* p_File, bool clearPrevSelection = false);

		//	Selects all descendants of the file with the specified address, can be configured to clear the current selection.
		void SelectAllDescendants(File* p_File, bool clearPrevSelection = false);

		//	Selects all files on the top-level and all their descendants.
		void SelectAllFiles();

		//	Deselects the file with the specified address, does nothing if the file wasn't selected anyway.
		void DeselectFile(File* p_File);

		//	Deselects all files.
		void DeselectAllFiles();
	private:
		//	Helper function which returns the iterator to a file with the specified address from the selected files
		std::vector<File*>::const_iterator FindFileInSelected(File* p_File) const;

		//	A list of all files loaded on the top-level, i.e. loaded from disk or generated in-memory
		std::vector<File> m_Files;

		//	A list which defines the selected files in the Content Viewer by holding their respective memory addresses
		std::vector<File*> m_SelectedFiles;
	};
}