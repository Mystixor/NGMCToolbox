#pragma once

#include "stdafx.h"

#include "MemoryBuffer.h"
#include "File.h"

namespace NGMC
{
	//  DDS_PIXELFORMAT.dwFlags

	static DWORD DDPF_ALPHAPIXELS = 0x1;
	static DWORD DDPF_ALPHA = 0x2;
	static DWORD DDPF_FOURCC = 0x4;
	static DWORD DDPF_RGB = 0x40;
	static DWORD DDPF_YUV = 0x200;
	static DWORD DDPF_LUMINANCE = 0x20000;


	//	Struct defining the Pixel Format in a DDS texture.
	struct DDS_PIXELFORMAT {
		DWORD dwSize;
		DWORD dwFlags;
		DWORD dwFourCC;
		DWORD dwRGBBitCount;
		DWORD dwRBitMask;
		DWORD dwGBitMask;
		DWORD dwBBitMask;
		DWORD dwABitMask;
	};


	//  DDS_HEADER.dwFlags

	static DWORD DDSD_CAPS = 0x1;
	static DWORD DDSD_HEIGHT = 0x2;
	static DWORD DDSD_WIDTH = 0x4;
	static DWORD DDSD_PITCH = 0x8;
	static DWORD DDSD_PIXELFORMAT = 0x1000;
	static DWORD DDSD_MIPMAPCOUNT = 0x20000;
	static DWORD DDSD_LINEARSIZE = 0x80000;
	static DWORD DDSD_DEPTH = 0x800000;


	//  DDS_HEADER.dwCaps

	static DWORD DDSCAPS_COMPLEX = 0x8;
	static DWORD DDSCAPS_MIPMAP = 0x400000;
	static DWORD DDSCAPS_TEXTURE = 0x1000;


	//  DDS_HEADER.dwCaps2

	static DWORD DDSCAPS2_CUBEMAP = 0x200;
	static DWORD DDSCAPS2_CUBEMAP_POSITIVEX = 0x400;
	static DWORD DDSCAPS2_CUBEMAP_NEGATIVEX = 0x800;
	static DWORD DDSCAPS2_CUBEMAP_POSITIVEY = 0x1000;
	static DWORD DDSCAPS2_CUBEMAP_NEGATIVEY = 0x2000;
	static DWORD DDSCAPS2_CUBEMAP_POSITIVEZ = 0x4000;
	static DWORD DDSCAPS2_CUBEMAP_NEGATIVEZ = 0x8000;
	static DWORD DDSCAPS2_VOLUME = 0x200000;


	//	The DDS file header struct.
	typedef struct {
		DWORD           dwSize;
		DWORD           dwFlags;
		DWORD           dwHeight;
		DWORD           dwWidth;
		DWORD           dwPitchOrLinearSize;
		DWORD           dwDepth;
		DWORD           dwMipMapCount;
		DWORD           dwReserved1[11];
		DDS_PIXELFORMAT ddspf;
		DWORD           dwCaps;
		DWORD           dwCaps2;
		DWORD           dwCaps3;
		DWORD           dwCaps4;
		DWORD           dwReserved2;
	} DDS_HEADER;

	//	The different DDS texture formats.
	enum DDSFormat
	{
		BGRA8,
		RGBA8,
		DXT1,
		DXT5,
		LUMINANCE,
		unsupported
	};

	//	The magic/signature "DDS " of DDS files.
	static DWORD ddsMagic = ' SDD';

	//	Builds a DDS file and saves it at the specified path.
	static bool BuildDDS(const wchar_t* path, void* imageData, unsigned int width, unsigned int height, unsigned int mipMapCount, DDSFormat format, size_t* mipSizes)
	{
		bool isSuccess = false;

		std::ofstream stream(path, std::ios::binary);

		if (stream)
		{
			size_t blockSize = 0;
			DWORD fourCC = 0;
			bool isCompressed = false;
			bool isLuminance = false;
			switch (format)
			{
			case DDSFormat::BGRA8:
			case DDSFormat::RGBA8:
			{
				blockSize = 32;
				break;
			}
			case DDSFormat::DXT1:
			{
				fourCC = '1TXD';
				blockSize = 8;
				isCompressed = true;
				break;
			}
			case DDSFormat::DXT5:
			{
				fourCC = '5TXD';
				blockSize = 16;
				isCompressed = true;
				break;
			}
			case DDSFormat::LUMINANCE:
			{
				blockSize = 8;
				isLuminance = true;
				break;
			}
			}

			DWORD pitch = (width * blockSize + 7) / 8;

			size_t mip0DataSize = mipSizes[0];
			size_t imageDataSize = 0;
			for (unsigned int i = 0; i < mipMapCount; i++)
			{
				imageDataSize += mipSizes[i];
			}

			size_t fileSize = 4 + sizeof(DDS_HEADER) + imageDataSize;
			char* fileData = new char[fileSize];
			memset((void*)fileData, 0, fileSize);

			memcpy((void*)fileData, (void*)&ddsMagic, 4);

			DDS_HEADER* p_DDS_HEADER = (DDS_HEADER*)((uintptr_t)fileData + 4);
			p_DDS_HEADER->dwSize = sizeof(DDS_HEADER);
			p_DDS_HEADER->dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_MIPMAPCOUNT | (isCompressed ? DDSD_LINEARSIZE : DDSD_PITCH);
			p_DDS_HEADER->dwHeight = height;
			p_DDS_HEADER->dwWidth = width;
			p_DDS_HEADER->dwPitchOrLinearSize = isCompressed ? (DWORD)mip0DataSize : pitch;
			p_DDS_HEADER->dwDepth = 0;
			p_DDS_HEADER->dwMipMapCount = mipMapCount;

			const char* reserved1 = "NGMCTool-DDS";
			memcpy((void*)&p_DDS_HEADER->dwReserved1, (void*)reserved1, min(strlen(reserved1), sizeof(DDS_HEADER::dwReserved1)));

			p_DDS_HEADER->ddspf.dwSize = sizeof(DDS_PIXELFORMAT);
			p_DDS_HEADER->ddspf.dwFlags = isCompressed ? DDPF_FOURCC : (isLuminance ? DDPF_LUMINANCE : (DDPF_ALPHAPIXELS | DDPF_RGB));
			p_DDS_HEADER->ddspf.dwFourCC = fourCC;

			if (!isCompressed)
			{
				switch (format)
				{
				case DDSFormat::BGRA8:
					p_DDS_HEADER->ddspf.dwRGBBitCount = blockSize;
					p_DDS_HEADER->ddspf.dwRBitMask = 0x00FF0000;
					p_DDS_HEADER->ddspf.dwGBitMask = 0x0000FF00;
					p_DDS_HEADER->ddspf.dwBBitMask = 0x000000FF;
					p_DDS_HEADER->ddspf.dwABitMask = 0xFF000000;
					break;
				case DDSFormat::RGBA8:
					p_DDS_HEADER->ddspf.dwRGBBitCount = blockSize;
					p_DDS_HEADER->ddspf.dwRBitMask = 0x000000FF;
					p_DDS_HEADER->ddspf.dwGBitMask = 0x0000FF00;
					p_DDS_HEADER->ddspf.dwBBitMask = 0x00FF0000;
					p_DDS_HEADER->ddspf.dwABitMask = 0xFF000000;
					break;
				case DDSFormat::LUMINANCE:
					p_DDS_HEADER->ddspf.dwRGBBitCount = blockSize;
					p_DDS_HEADER->ddspf.dwRBitMask = 0x000000FF;
					p_DDS_HEADER->ddspf.dwGBitMask = 0x000000FF;
					p_DDS_HEADER->ddspf.dwBBitMask = 0x000000FF;
					p_DDS_HEADER->ddspf.dwABitMask = 0x000000FF;
				}
			}

			p_DDS_HEADER->dwCaps = DDSCAPS_TEXTURE | (mipMapCount ? DDSCAPS_COMPLEX | DDSCAPS_MIPMAP : 0);
			p_DDS_HEADER->dwCaps2 = 0;
			p_DDS_HEADER->dwCaps3 = 0;
			p_DDS_HEADER->dwCaps4 = 0;
			p_DDS_HEADER->dwReserved2 = 0;

			memcpy((void*)((uintptr_t)fileData + 4 + sizeof(DDS_HEADER)), (void*)imageData, imageDataSize);

			stream.write(fileData, fileSize);

			delete[] fileData;
		}

		return isSuccess;
	}

	//	Builds a DDS file and loads it into the specified outBuffer.
	static bool BuildDDS(MemoryBuffer& outBuffer, void* imageData, unsigned int width, unsigned int height, unsigned int mipMapCount, DDSFormat format, size_t* mipSizes)
	{
		bool isSuccess = false;

		size_t blockSize = 0;
		DWORD fourCC = 0;
		bool isCompressed = false;
		bool isLuminance = false;
		switch (format)
		{
		case DDSFormat::BGRA8:
		case DDSFormat::RGBA8:
		{
			blockSize = 32;
			break;
		}
		case DDSFormat::DXT1:
		{
			fourCC = '1TXD';
			blockSize = 8;
			isCompressed = true;
			break;
		}
		case DDSFormat::DXT5:
		{
			fourCC = '5TXD';
			blockSize = 16;
			isCompressed = true;
			break;
		}
		case DDSFormat::LUMINANCE:
		{
			blockSize = 8;
			isLuminance = true;
			break;
		}
		}

		DWORD pitch = (width * blockSize + 7) / 8;

		size_t mip0DataSize = mipSizes[0];
		size_t imageDataSize = 0;
		for (unsigned int i = 0; i < mipMapCount; i++)
		{
			imageDataSize += mipSizes[i];
		}

		size_t fileSize = 4 + sizeof(DDS_HEADER) + imageDataSize;
		char* fileData = new char[fileSize];
		memset((void*)fileData, 0, fileSize);

		memcpy((void*)fileData, (void*)&ddsMagic, 4);

		DDS_HEADER* p_DDS_HEADER = (DDS_HEADER*)((uintptr_t)fileData + 4);
		p_DDS_HEADER->dwSize = sizeof(DDS_HEADER);
		p_DDS_HEADER->dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_MIPMAPCOUNT | (isCompressed ? DDSD_LINEARSIZE : DDSD_PITCH);
		p_DDS_HEADER->dwHeight = height;
		p_DDS_HEADER->dwWidth = width;
		p_DDS_HEADER->dwPitchOrLinearSize = isCompressed ? (DWORD)mip0DataSize : pitch;
		p_DDS_HEADER->dwDepth = 0;
		p_DDS_HEADER->dwMipMapCount = mipMapCount;

		const char* reserved1 = "NGMCTool-DDS";
		memcpy((void*)&p_DDS_HEADER->dwReserved1, (void*)reserved1, min(strlen(reserved1), sizeof(DDS_HEADER::dwReserved1)));

		p_DDS_HEADER->ddspf.dwSize = sizeof(DDS_PIXELFORMAT);
		p_DDS_HEADER->ddspf.dwFlags = isCompressed ? DDPF_FOURCC : (isLuminance ? DDPF_LUMINANCE : (DDPF_ALPHAPIXELS | DDPF_RGB));
		p_DDS_HEADER->ddspf.dwFourCC = fourCC;

		if (!isCompressed)
		{
			switch (format)
			{
			case DDSFormat::BGRA8:
				p_DDS_HEADER->ddspf.dwRGBBitCount = blockSize;
				p_DDS_HEADER->ddspf.dwRBitMask = 0x00FF0000;
				p_DDS_HEADER->ddspf.dwGBitMask = 0x0000FF00;
				p_DDS_HEADER->ddspf.dwBBitMask = 0x000000FF;
				p_DDS_HEADER->ddspf.dwABitMask = 0xFF000000;
				break;
			case DDSFormat::RGBA8:
				p_DDS_HEADER->ddspf.dwRGBBitCount = blockSize;
				p_DDS_HEADER->ddspf.dwRBitMask = 0x000000FF;
				p_DDS_HEADER->ddspf.dwGBitMask = 0x0000FF00;
				p_DDS_HEADER->ddspf.dwBBitMask = 0x00FF0000;
				p_DDS_HEADER->ddspf.dwABitMask = 0xFF000000;
				break;
			case DDSFormat::LUMINANCE:
				p_DDS_HEADER->ddspf.dwRGBBitCount = blockSize;
				p_DDS_HEADER->ddspf.dwRBitMask = 0x000000FF;
				p_DDS_HEADER->ddspf.dwGBitMask = 0x000000FF;
				p_DDS_HEADER->ddspf.dwBBitMask = 0x000000FF;
				p_DDS_HEADER->ddspf.dwABitMask = 0x000000FF;
			}
		}

		p_DDS_HEADER->dwCaps = DDSCAPS_TEXTURE | (mipMapCount ? DDSCAPS_COMPLEX | DDSCAPS_MIPMAP : 0);
		p_DDS_HEADER->dwCaps2 = 0;
		p_DDS_HEADER->dwCaps3 = 0;
		p_DDS_HEADER->dwCaps4 = 0;
		p_DDS_HEADER->dwReserved2 = 0;

		memcpy((void*)((uintptr_t)fileData + 4 + sizeof(DDS_HEADER)), (void*)imageData, imageDataSize);

		outBuffer.PrepareBuffer(fileSize);
		outBuffer.Write(fileData, fileSize);

		delete[] fileData;

		return isSuccess;
	}

	//	Class for loading data from a DDS file.
	class LoaderDDS
	{
	public:
		//	Constructs a DDS Loader object by specifying a Memory Buffer to read the file from.
		LoaderDDS(MemoryBuffer& memoryBuffer);

		//	Constructs a DDS Loader object by specifying a wide char string path to read the file at.
		LoaderDDS(const wchar_t* path);

		//	Constructs a DDS Loader object by specifying a file object to read.
		LoaderDDS(File* p_File);

		//	Deconstructs the DDS Loader object.
		~LoaderDDS();

		//	Loads the header of the DDS file into the specified outHeader, returns whether the operation was successful.
		bool GetHeader(DDS_HEADER& outHeader);

		//	Loads the image data of the specified mip level of the DDS file into the specified outBuffer, returns whether the operation was successful.
		bool GetMipData(MemoryBuffer& outBuffer, unsigned int mipLevel);

		//	Loads the image data including all mip levels of the DDS file into the specified outBuffer, returns whether the operation was successful.
		bool GetImageData(MemoryBuffer& outBuffer);

		//	Returns the DDS Pixel Format of the DDS texture.
		DDSFormat GetPixelFormat();

	private:
		bool GetHeaderFromDisk(DDS_HEADER& outHeader);
		bool GetHeaderFromMemory(DDS_HEADER& outHeader);

		bool GetMipDataFromDisk(MemoryBuffer& outBuffer, unsigned int mipLevel);
		bool GetMipDataFromMemory(MemoryBuffer& outBuffer, unsigned int mipLevel);
		
		bool GetImageDataFromDisk(MemoryBuffer& outBuffer);
		bool GetImageDataFromMemory(MemoryBuffer& outBuffer);

		//	Whether the file is loaded from memory or disk.
		bool m_IsFileInMemory;

		//	The MemoryBuffer holding the memory to read the file from, unused if m_IsFileInMemory == false
		MemoryBuffer m_MemBuf;

		//	The wide char string to read the file from, unused if m_IsFileInMemory == true
		std::wstring m_FilePath;
	};
}