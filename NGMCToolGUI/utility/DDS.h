#pragma once

#include "minwindef.h"

namespace NGMC
{
	//  DDS_PIXELFORMAT.dwFlags

	constexpr inline DWORD DDPF_ALPHAPIXELS = 0x1;
	constexpr inline DWORD DDPF_ALPHA = 0x2;
	constexpr inline DWORD DDPF_FOURCC = 0x4;
	constexpr inline DWORD DDPF_RGB = 0x40;
	constexpr inline DWORD DDPF_YUV = 0x200;
	constexpr inline DWORD DDPF_LUMINANCE = 0x20000;


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

	constexpr inline DWORD DDSD_CAPS = 0x1;
	constexpr inline DWORD DDSD_HEIGHT = 0x2;
	constexpr inline DWORD DDSD_WIDTH = 0x4;
	constexpr inline DWORD DDSD_PITCH = 0x8;
	constexpr inline DWORD DDSD_PIXELFORMAT = 0x1000;
	constexpr inline DWORD DDSD_MIPMAPCOUNT = 0x20000;
	constexpr inline DWORD DDSD_LINEARSIZE = 0x80000;
	constexpr inline DWORD DDSD_DEPTH = 0x800000;


	//  DDS_HEADER.dwCaps

	constexpr inline DWORD DDSCAPS_COMPLEX = 0x8;
	constexpr inline DWORD DDSCAPS_MIPMAP = 0x400000;
	constexpr inline DWORD DDSCAPS_TEXTURE = 0x1000;


	//  DDS_HEADER.dwCaps2

	constexpr inline DWORD DDSCAPS2_CUBEMAP = 0x200;
	constexpr inline DWORD DDSCAPS2_CUBEMAP_POSITIVEX = 0x400;
	constexpr inline DWORD DDSCAPS2_CUBEMAP_NEGATIVEX = 0x800;
	constexpr inline DWORD DDSCAPS2_CUBEMAP_POSITIVEY = 0x1000;
	constexpr inline DWORD DDSCAPS2_CUBEMAP_NEGATIVEY = 0x2000;
	constexpr inline DWORD DDSCAPS2_CUBEMAP_POSITIVEZ = 0x4000;
	constexpr inline DWORD DDSCAPS2_CUBEMAP_NEGATIVEZ = 0x8000;
	constexpr inline DWORD DDSCAPS2_VOLUME = 0x200000;


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
	constexpr inline DWORD ddsMagic = ' SDD';
}