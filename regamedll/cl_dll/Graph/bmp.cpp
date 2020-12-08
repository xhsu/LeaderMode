/*

Created Date: Dec 07 2020

Modern Warfare Dev Team
 - Luna the Reborn

*/


#include "precompiled.h"
#include "../external/GL/glext.h"

// ----------------------------------------------------------------
// Load BMP file
// ----------------------------------------------------------------

#define DIB_HEADER_MARKER ((WORD)('M' << 8) | 'B')

bool LoadBMP(const char* szFilename, byte* buffer, int bufferSize, int* width, int* height)
{
	FileHandle_t file = FILE_SYSTEM->Open(szFilename, "rb");

	if (!file)
		return false;

	BITMAPFILEHEADER bmfHeader;
	LPBITMAPINFO lpbmi;
	DWORD dwFileSize = FILE_SYSTEM->Size(file);

	if (!FILE_SYSTEM->Read(&bmfHeader, sizeof(bmfHeader), file))
	{
		*width = 0;
		*height = 0;

		FILE_SYSTEM->Close(file);
		return false;
	}

	if (bmfHeader.bfType == DIB_HEADER_MARKER)
	{
		DWORD dwBitsSize = dwFileSize - sizeof(bmfHeader);

		unsigned char* pDIB = (unsigned char*)malloc(dwBitsSize);

		if (!FILE_SYSTEM->Read(pDIB, dwBitsSize, file))
		{
			free(pDIB);

			*width = 0;
			*height = 0;

			FILE_SYSTEM->Close(file);
			return false;
		}

		lpbmi = (LPBITMAPINFO)pDIB;

		if (lpbmi->bmiHeader.biBitCount != 8)
			Sys_Error("[LoadBMP] Only 8 bit images supported.\nFILE: %s", szFilename);

		if (width)
			*width = lpbmi->bmiHeader.biWidth;

		if (height)
			*height = lpbmi->bmiHeader.biHeight;

		unsigned char* rgba = (unsigned char*)(pDIB + sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD));

		for (int j = 0; j < lpbmi->bmiHeader.biHeight; j++)
		{
			for (int i = 0; i < lpbmi->bmiHeader.biWidth; i++)
			{
				int y = (lpbmi->bmiHeader.biHeight - j - 1);

				int offs = (y * lpbmi->bmiHeader.biWidth + i);
				int offsdest = (j * lpbmi->bmiHeader.biWidth + i) * 4;
				unsigned char* src = rgba + offs;
				unsigned char* dst = buffer + offsdest;

				dst[0] = lpbmi->bmiColors[*src].rgbRed;
				dst[1] = lpbmi->bmiColors[*src].rgbGreen;
				dst[2] = lpbmi->bmiColors[*src].rgbBlue;

				// LUNA: leadermode only!!!
				if (dst[0] <= 10 && dst[2] <= 10)
					dst[3] = 255 - lpbmi->bmiColors[*src].rgbGreen;
				else
					dst[3] = 255;
			}
		}

		free(pDIB);
	}

	FILE_SYSTEM->Close(file);

	return true;
}

GLuint LoadBMP(const char* szPath, int* piWidth, int* piHeight)
{
	static byte buffer[2048 * 2048 * 4];
	int fmt = 0;
	int wide = 0, tall = 0;
	GLuint iTextureID = 0U;

	// makes sure that the load function actually get something.
	if (!piWidth)
		piWidth = &wide;

	if (!piHeight)
		piHeight = &tall;

	if (LoadBMP(szPath, buffer, sizeof(buffer), piWidth, piHeight))
	{
		glGenTextures(1, &iTextureID);
		glBindTexture(GL_TEXTURE_2D, iTextureID);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, *piWidth, *piHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// LUNA: prevent coord > 1.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		// unbind.
		glBindTexture(GL_TEXTURE_2D, NULL);
	}

	return iTextureID;

	//byte* datBuff[2] = { nullptr, nullptr }; // Header buffers

	//byte* pixels = nullptr; // Pixels

	//BITMAPFILEHEADER* bmpHeader = nullptr; // Header
	//BITMAPINFOHEADER* bmpInfo = nullptr; // Info

	//// The file... We open it with it's constructor
	//FileHandle_t file = FILE_SYSTEM->Open(szPath, "rb");
	//if (!file)
	//	return 0U;

	//// Allocate byte memory that will hold the two headers
	//datBuff[0] = new byte[sizeof(BITMAPFILEHEADER)];
	//datBuff[1] = new byte[sizeof(BITMAPINFOHEADER)];

	//FILE_SYSTEM->Read(datBuff[0], sizeof(BITMAPFILEHEADER), file);
	//FILE_SYSTEM->Read(datBuff[1], sizeof(BITMAPINFOHEADER), file);

	//// Construct the values from the buffers
	//bmpHeader = (BITMAPFILEHEADER*)datBuff[0];
	//bmpInfo = (BITMAPINFOHEADER*)datBuff[1];

	//// Check if the file is an actual BMP file
	//if (bmpHeader->bfType != 0x4D42)
	//{
	//	Sys_Error("%s is not a BMP file.", szPath);
	//	return 0U;
	//}

	//// First allocate pixel memory
	//pixels = new byte[bmpInfo->biSizeImage];

	//// Go to where image data starts, then read in image data
	//FILE_SYSTEM->Seek(file, bmpHeader->bfOffBits, FILESYSTEM_SEEK_CURRENT);
	//FILE_SYSTEM->Read(pixels, bmpInfo->biSizeImage, file);

	//// We're almost done. We have our image loaded, however it's not in the right format.
	//// .bmp files store image data in the BGR format, and we have to convert it to RGB.
	//// Since we have the value in bytes, this shouldn't be to hard to accomplish
	//byte tmpRGB = 0; // Swap buffer
	//for (unsigned long i = 0; i < bmpInfo->biSizeImage; i += 3)
	//{
	//	tmpRGB = pixels[i];
	//	pixels[i] = pixels[i + 2];
	//	pixels[i + 2] = tmpRGB;
	//}

	//// Set width and height to the values loaded from the file
	//*piWidth = bmpInfo->biWidth;
	//*piHeight = bmpInfo->biHeight;

	///*******************GENERATING TEXTURES*******************/

	//GLuint iTextureID = 0U;
	//glGenTextures(1, &iTextureID);             // Generate a texture
	//glBindTexture(GL_TEXTURE_2D, iTextureID); // Bind that texture temporarily

	//GLint mode = GL_RGB;                   // Set the mode

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//// Create the texture. We get the offsets from the image, then we use it with the image's
	//// pixel data to create it.
	//glTexImage2D(GL_TEXTURE_2D, 0, mode, *piWidth, *piHeight, 0, mode, GL_UNSIGNED_BYTE, pixels);

	//// Unbind the texture
	//glBindTexture(GL_TEXTURE_2D, NULL);

	//// Delete the two buffers.
	//delete[] datBuff[0];
	//delete[] datBuff[1];
	//delete[] pixels;

	//return iTextureID;
}
