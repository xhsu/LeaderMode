/*

Created Date: Mar 20 2020

Modern Warfare Dev Team
 - Luna the Reborn

*/

#include "precompiled.h"
#include "../external/GL/glext.h"

#pragma comment(lib, "opengl32.lib")

PFNGLCOMPRESSEDTEXIMAGE2DARBPROC glCompressedTexImage2DARB = NULL;

#pragma region DDS

#define DDPF_ALPHAPIXELS	0x000001
#define DDPF_ALPHA			0x000002
#define DDPF_FOURCC			0x000004
#define DDPF_RGB			0x000040
#define DDPF_YUV			0x000200
#define DDPF_LUMINANCE		0x020000

#define D3DFMT_DXT1		(('D'<<0)|('X'<<8)|('T'<<16)|('1'<<24))
#define D3DFMT_DXT3		(('D'<<0)|('X'<<8)|('T'<<16)|('3'<<24))
#define D3DFMT_DXT5		(('D'<<0)|('X'<<8)|('T'<<16)|('5'<<24))

typedef struct
{
	DWORD	dwSize;
	DWORD	dwFlags;
	DWORD	dwFourCC;
	DWORD	dwRGBBitCount;
	DWORD	dwRBitMask;
	DWORD	dwGBitMask;
	DWORD	dwBBitMask;
	DWORD	dwABitMask;
} DDS_PIXELFORMAT;

#define DDSD_CAPS			0x000001
#define DDSD_HEIGHT			0x000002
#define DDSD_WIDTH			0x000004
#define DDSD_PITCH			0x000008
#define DDSD_PIXELFORMAT	0x001000
#define DDSD_MIPMAPCOUNT	0x020000
#define DDSD_LINEARSIZE		0x080000
#define DDSD_DEPTH			0x800000

typedef struct
{
	DWORD			dwSize;
	DWORD			dwFlags;
	DWORD			dwHeight;
	DWORD			dwWidth;
	DWORD			dwPitchOrLinearSize;
	DWORD			dwDepth;
	DWORD			dwMipMapCount;
	DWORD			dwReserved1[11];
	DDS_PIXELFORMAT	ddspf;
	DWORD			dwCaps;
	DWORD			dwCaps2;
	DWORD			dwCaps3;
	DWORD			dwCaps4;
	DWORD			dwReserved2;
} DDS_HEADER;

typedef struct
{
	DWORD		dwMagic;
	DDS_HEADER	Header;
} DDS_FILEHEADER;

// For a compressed texture, the size of each mipmap level image is typically one-fourth the size of the previous, with a minimum of 8 (DXT1) or 16 (DXT2-5) bytes (for 
// square textures). Use the following formula to calculate the size of each level for a non-square texture:
#define SIZE_OF_DXT1(width, height)		( Q_max((unsigned long)1, ( (width + 3) >> 2 ) ) * Q_max((unsigned long)1, ( (height + 3) >> 2 ) ) * 8 )
#define SIZE_OF_DXT2(width, height)		( Q_max((unsigned long)1, ( (width + 3) >> 2 ) ) * Q_max((unsigned long)1, ( (height + 3) >> 2 ) ) * 16 )

#pragma endregion

void Dxt_Initialization(void)
{
	// GL_ARB_texture_compression
	// GL_EXT_texture_compression_s3tc
	glCompressedTexImage2DARB = (PFNGLCOMPRESSEDTEXIMAGE2DARBPROC)wglGetProcAddress("glCompressedTexImage2DARB");
}

GLuint gl_load_dds(GLvoid* pBuffer, int* iWidth, int* iHeight)
{
	DDS_FILEHEADER* header;
	DWORD			compressFormat;
	GLvoid* data;
	GLsizei			imageSize;

	header = (DDS_FILEHEADER*)pBuffer;

	if (header->dwMagic != 0x20534444)
	{
		printf("bad dds file\n");
		return 0;
	}

	if (header->Header.dwSize != 124)
	{
		printf("bad header size\n");
		return 0;
	}

	if (!(header->Header.dwFlags & DDSD_LINEARSIZE))
	{
		printf("bad file type\n");
		return 0;
	}

	if (!(header->Header.ddspf.dwFlags & DDPF_FOURCC))
	{
		printf("bad pixel format\n");
		return 0;
	}

	compressFormat = header->Header.ddspf.dwFourCC;

	if (compressFormat != D3DFMT_DXT1 &&
		compressFormat != D3DFMT_DXT3 &&
		compressFormat != D3DFMT_DXT5)
	{
		printf("bad compress format\n");
		return 0;
	}

	data = (GLvoid*)(header + 1);    // header data skipped

	//int iTextureID = VGUI_SURFACE->CreateNewTextureID();
	GLuint iTextureID;
	glGenTextures(1, &iTextureID);
	glBindTexture(GL_TEXTURE_2D, iTextureID);

	// LUNA: prevent coord > 1.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	switch (compressFormat)
	{
	case D3DFMT_DXT1:
		imageSize = SIZE_OF_DXT1(header->Header.dwWidth, header->Header.dwHeight);
		glCompressedTexImage2DARB(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB_S3TC_DXT1_EXT, header->Header.dwWidth, header->Header.dwHeight, 0, imageSize, data);
		break;
	case D3DFMT_DXT3:
		imageSize = SIZE_OF_DXT2(header->Header.dwWidth, header->Header.dwHeight);
		glCompressedTexImage2DARB(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, header->Header.dwWidth, header->Header.dwHeight, 0, imageSize, data);
		break;
	case D3DFMT_DXT5:
		imageSize = SIZE_OF_DXT2(header->Header.dwWidth, header->Header.dwHeight);
		glCompressedTexImage2DARB(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, header->Header.dwWidth, header->Header.dwHeight, 0, imageSize, data);
		break;
	}

	// LUNA: return the height and width data.
	if (iWidth)
		*iWidth = header->Header.dwWidth;
	if (iHeight)
		*iHeight = header->Header.dwHeight;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, NULL);	// reset current texture
	return iTextureID;
}

GLuint LoadDDS(const char* szFile, int* iWidth, int* iHeight)
{
	//FILE*	fp;
	FileHandle_t fp;
	int		size;
	void*	data;

	if (iWidth)
		*iWidth = 0;
	if (iHeight)
		*iHeight = 0;

	//fp = fopen(szFile, "rb");
	fp = FILE_SYSTEM->Open(szFile, "rb");
	if (!fp)
		return 0;

	FILE_SYSTEM->Seek(fp, 0, FILESYSTEM_SEEK_TAIL);
	size = FILE_SYSTEM->Tell(fp);
	FILE_SYSTEM->Seek(fp, 0, FILESYSTEM_SEEK_HEAD);
	//fseek(fp, 0, SEEK_END);
	//size = ftell(fp);
	//fseek(fp, 0, SEEK_SET);

	data = malloc(size);
	if (!data)
	{
		//fclose(fp);
		FILE_SYSTEM->Close(fp);
		return 0;
	}

	//if (fread(data, size, 1, fp) != 1)
	if (!FILE_SYSTEM->Read(data, size, fp))
	{
		free(data);
		//fclose(fp);
		FILE_SYSTEM->Close(fp);
		return 0;
	}

	//fclose(fp);
	FILE_SYSTEM->Close(fp);

	// Load DDS to GL texture
	unsigned int iResult = gl_load_dds(data, iWidth, iHeight);

	free(data);

	return iResult;
}
