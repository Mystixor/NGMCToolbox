#pragma once

#include "stdafx.h"

#include "Utility.h"
#include "MemoryBuffer.h"

namespace NGMC
{
	class File
	{
	public:
		//	Default constructor for a File object.
		File();

		//	Constructs a File object by copying another File object.
		File(const File& other);

		//	Constructs a File object by assigning its file path.
		File(const wchar_t* filePath, size_t dataOffset = 0ULL);
		
		//	Constructs a File object by assigning its file path and its type.
		File(const wchar_t* filePath, const char* name, FileType type = FileType(), size_t dataOffset = 0ULL);
		
		//	Constructs a File object by assigning its file path, its size and its type.
		File(const wchar_t* filePath, const char* name, std::uintmax_t size, FileType type = FileType(), size_t dataOffset = 0ULL);
		
		//	Constructs a File object by assigning its file path, its size, its parent and its type.
		File(const wchar_t* filePath, const char* name, std::uintmax_t size, unsigned int indexInParent, File* p_Parent, FileType type = FileType(), size_t dataOffset = 0ULL);
		
		//	Constructs a File object by assigning its memory, its parent and its type.
		File(MemoryBuffer& memBuf, const char* name, unsigned int indexInParent, File* p_Parent, FileType type = FileType());

		//	Deconstructs the File object by calling the Unload() member function.
		~File();


		//	Gets a boolean on whether the file is loaded.
		bool IsLoaded() const;

		//	Gets a boolean on whether the file is purely in memory or on disk.
		bool IsFileInMemory() const;

		//	Gets the file path of the file. Empty wide string if the file is in memory.
		const wchar_t* GetFilePath() const;

		//	Gets the name of the file.
		const char* GetName() const;

		//	Gets the type of the file.
		FileType GetType() const;

		//	Gets the count of the file's children.
		unsigned int GetChildCount() const;

		//	Gets the count of the file's children added to the count of all its children's descendants.
		unsigned int GetDescendantCount() const;

		//	Gets the name of the child at the specified index in the file's list of children.
		const char* GetChildName(unsigned int index) const;

		//	Gets the child at the specified index in the file's list of children.
		File* GetChild(unsigned int index);

		//	Gets the size of the file.
		std::uintmax_t GetSize() const;

		//	Gets the Memory Buffer object.
		MemoryBuffer& GetMemoryBuffer();

		//	Gets the index of the file in its parent's list of children.
		unsigned int GetIndexInParent() const;


		//	Updates the status of the file, that being its size and whether it is on disk or purely in memory.
		void UpdateStatus();

		//	Sets the name of the file.
		void SetName(const char* name);

		//	Sets the type of the file.
		void SetType(FileType type);

		//	Sets the parent of the file.
		void SetParent(File* p_Parent);


		//	Loads the file, meaning its contents will be made accessible through its children.
		bool Load();

		//	Unloads the file, given it was loaded before. Useful to free memory occupied by the file's children.
		bool Unload();

		//	Saves the file at the given path, given it is registered in memory as opposed to on disk (in the latter case, consider just copying the file in your file explorer). If isPrintResult == true, a message will be logged to the console.
		bool Save(const wchar_t* filePath, bool isPrintResult = true);

		//	Extracts all contents of the file to the given directory, equal to loading the file and saving all children to disk manually.
		bool Extract(const wchar_t* directory);



		//	Content Viewer

		//	Returns whether the node is opened.
		bool IsNodeOpened() const;
		
		//	Sets the openness of the node. Only opens the node (if true), or keeps it closed (if false) if called after it was opened and before the node gets rendered the next time.
		void SetNodeOpenness(bool isOpen);

	private:
		//	Tries to detect and returns the FileType of the File by parsing its file extension and confirming it (if possible) by parsing its magic.
		FileType DetectType();

		//	Loads a databin.
		bool LoadDatabin(GAME game);
		
		//	Loads a databinItem.
		bool LoadDatabinItem(GAME game);

		//	Extracts all files from a databin, returns the amount of extracted files.
		unsigned int ExtractDatabin(GAME game, const wchar_t* directory);

		//	Whether the file is loaded into memory, as opposed to being read from disk.
		bool m_IsFileInMemory;

		//	Whether the file is loaded. If true, then all data of the file should be accessible through its children.
		bool m_IsLoaded;

		//	The path wide string to the file on disk, from which this object reads its data. Empty, if the file is loaded into memory.
		std::wstring m_FilePath;

		//	The offset of the file in the databin which is its parent. To be obsoleted in the future.
		size_t m_DataOffset;

		//	The name string which shows in the Content Viewer and on other UI elements. It is NOT a unique identifier.
		std::string m_Name;

		//	The file type or format associated with this file. Currently only supporting NGS1 file formats as well as selected general purpose formats.
		FileType m_Type;

		//	The children of this file, which represent the data segments in which this file may be split up, and which show in the Content Viewer and on other UI elements.
		std::vector<File> m_Childs;

		//	The size of the file's contents, either in memory or on disk.
		std::uintmax_t m_Size;

		//	The buffer holding the file's contents, if the file is not being read from disk.
		MemoryBuffer m_MemBuf;

		//	The index of this file in its parent's list of children.
		unsigned int m_IndexInParent;

		//	The pointer to this file's parent. nullptr if the file has no parent because it is on the top level in the FileManager object (for example by being loaded from disk or being generated during runtime).
		File* m_Parent;



		//	Content Viewer

		//	Whether the node is opened.
		bool m_IsNodeOpened;
	};
}