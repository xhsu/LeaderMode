/*

Created Date: Dec 07 2020

Modern Warfare Dev Team
 - Luna the Reborn

*/


#include "precompiled.h"
#include "../external/GL/glext.h"

// ----------------------------------------------------------------
// Load TGA file
// ----------------------------------------------------------------

#pragma pack(1)

struct TargaHeader
{
	unsigned char id_length, colormap_type, image_type;
	unsigned short colormap_index, colormap_length;
	unsigned char colormap_size;
	unsigned short x_origin, y_origin, width, height;
	unsigned char pixel_size, attributes;
};

#pragma pack()

bool LoadTGA(const char* filename, unsigned char* buffer, int bufferSize, int* format, int* width, int* height)
{
	FileHandle_t fp = FILE_SYSTEM->Open(filename, "rb");

	if (!fp)
		return false;

	int columns, rows, numPixels;
	unsigned char* pixbuf;
	int row, column;
	TargaHeader header;

	if (!FILE_SYSTEM->Read(&header, sizeof(header), fp))
	{
		*width = 0;
		*height = 0;

		FILE_SYSTEM->Close(fp);
		return false;
	}

	if (header.image_type != 2 && header.image_type != 10)
		Sys_Error("[LoadTGA] Only type 2 and 10 targa RGB images supported.\nFILE: %s", filename);

	if (header.colormap_type != 0 || (header.pixel_size != 32 && header.pixel_size != 24))
		Sys_Error("[LoadTGA] Only 32 or 24 bit images supported (no colormaps).\nFILE: %s", filename);

	columns = header.width;
	rows = header.height;
	numPixels = columns * rows;

	if (header.pixel_size == 32)
		*format = GL_RGBA;
	else if (header.pixel_size == 24)
		*format = GL_RGB;

	*width = header.width;
	*height = header.height;

	if (header.id_length != 0)
		FILE_SYSTEM->Seek(fp, header.id_length, FILESYSTEM_SEEK_CURRENT);

	int savepos = FILE_SYSTEM->Tell(fp);
	int filesize = FILE_SYSTEM->Size(fp);

	int size = filesize - savepos;
	unsigned char* freebuf = (unsigned char*)malloc(size);
	unsigned char* fbuffer = freebuf;
	int readcount = FILE_SYSTEM->Read(fbuffer, size, fp);
	FILE_SYSTEM->Close(fp);

	if (!readcount)
	{
		free(freebuf);
		return FALSE;
	}

	if (header.image_type == 2)
	{
		for (row = rows - 1; row >= 0; row--)
		{
			pixbuf = buffer + row * columns * 4;

			for (column = 0; column < columns; column++)
			{
				register unsigned char red, green, blue, alphabyte;

				switch (header.pixel_size)
				{
				case 24:
				{
					blue = fbuffer[0];
					green = fbuffer[1];
					red = fbuffer[2];
					pixbuf[0] = red;
					pixbuf[1] = green;
					pixbuf[2] = blue;
					pixbuf[3] = 255;
					fbuffer += 3;
					pixbuf += 4;
					break;
				}

				case 32:
				{
					blue = fbuffer[0];
					green = fbuffer[1];
					red = fbuffer[2];
					alphabyte = fbuffer[3];
					pixbuf[0] = red;
					pixbuf[1] = green;
					pixbuf[2] = blue;
					pixbuf[3] = alphabyte;
					fbuffer += 4;
					pixbuf += 4;
					break;
				}
				}
			}
		}
	}
	else if (header.image_type == 10)
	{
		register unsigned char red, green, blue, alphabyte;
		unsigned char packetHeader, packetSize, j;

		for (row = rows - 1; row >= 0; row--)
		{
			pixbuf = buffer + row * columns * 4;

			for (column = 0; column < columns; )
			{
				packetHeader = *fbuffer++;
				packetSize = 1 + (packetHeader & 0x7F);

				if (packetHeader & 0x80)
				{
					switch (header.pixel_size)
					{
					case 24:
					{
						blue = fbuffer[0];
						green = fbuffer[1];
						red = fbuffer[2];
						alphabyte = 255;
						fbuffer += 3;
						break;
					}

					case 32:
					{
						blue = fbuffer[0];
						green = fbuffer[1];
						red = fbuffer[2];
						alphabyte = fbuffer[3];
						fbuffer += 4;
						break;
					}
					}

					for (j = 0; j < packetSize; j++)
					{
						pixbuf[0] = red;
						pixbuf[1] = green;
						pixbuf[2] = blue;
						pixbuf[3] = alphabyte;
						pixbuf += 4;
						column++;

						if (column == columns)
						{
							column = 0;

							if (row > 0)
								row--;
							else
								goto breakOut;

							pixbuf = buffer + row * columns * 4;
						}
					}
				}
				else
				{
					for (j = 0; j < packetSize; j++)
					{
						switch (header.pixel_size)
						{
						case 24:
						{
							blue = fbuffer[0];
							green = fbuffer[1];
							red = fbuffer[2];
							pixbuf[0] = red;
							pixbuf[1] = green;
							pixbuf[2] = blue;
							pixbuf[3] = 255;
							fbuffer += 3;
							pixbuf += 4;
							break;
						}

						case 32:
						{
							blue = fbuffer[0];
							green = fbuffer[1];
							red = fbuffer[2];
							alphabyte = fbuffer[3];
							pixbuf[0] = red;
							pixbuf[1] = green;
							pixbuf[2] = blue;
							pixbuf[3] = alphabyte;
							fbuffer += 4;
							pixbuf += 4;
							break;
						}
						}

						column++;

						if (column == columns)
						{
							column = 0;

							if (row > 0)
								row--;
							else
								goto breakOut;

							pixbuf = buffer + row * columns * 4;
						}
					}
				}
			}

		breakOut:;
		}
	}

	free(freebuf);

	return true;
}

GLuint LoadTGA(const char* szPath, int* piWidth, int* piHeight)
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

	if (LoadTGA(szPath, buffer, sizeof(buffer), &fmt, piWidth, piHeight))
	{
		glGenTextures(1, &iTextureID);
		glBindTexture(GL_TEXTURE_2D, iTextureID);

		glTexImage2D(GL_TEXTURE_2D, 0, fmt, *piWidth, *piHeight, 0, fmt, GL_UNSIGNED_BYTE, buffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// LUNA: prevent coord > 1.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		// unbind.
		glBindTexture(GL_TEXTURE_2D, NULL);
	}

	return iTextureID;
}
