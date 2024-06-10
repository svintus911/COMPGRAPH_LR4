
#include "Texture.h"

#include <algorithm>
#include <array>


Texture::~Texture()
{
	deleteTexture();
}

void Texture::loadTextureFromFile(LPWSTR filename)
{
	//������ ����������� ���������  (R G B)
	RGBTRIPLE *texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char *texCharArray;
	int texW, texH;
	Texture::LoadBMP(filename, &texW, &texH, &texarray);
	Texture::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void Texture::loadFromBuffer(unsigned char * buf, int w, int h)
{
	glDeleteTextures(1, &texId);
	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//����������� ������ � �������� (������������ �������� �������)
	auto buf_re = new unsigned char[w * h * 4];
	for (int i = 0; i < h-1; ++i)
	{
		std::copy(buf + i * w * 4, buf + (i + 1) * w * 4, buf_re + (h - i - 1) * w * 4);
	}
	
	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, buf_re);
	delete[] buf_re;

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}



void Texture::deleteTexture()
{
	glDeleteTextures(1, &texId);
	texId = 0;
}

void Texture::bindTexture()
{
	glBindTexture(GL_TEXTURE_2D, texId);
}

int Texture::LoadBMP(__in LPWSTR  filename, __out int* Wigth, __out int *Height, __out RGBTRIPLE **arr)
{
	DWORD nBytesRead = 0;
	int read_size = 0;
	int i = 0;
	int width, height, size;
	BITMAPINFOHEADER infoh;
	BITMAPFILEHEADER fileh;

	HANDLE file = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);


	//��������� ��������� BMP �����
	ReadFile((HANDLE)file, &fileh, sizeof(BITMAPFILEHEADER), &nBytesRead, 0);
	ReadFile((HANDLE)file, &infoh, sizeof(BITMAPINFOHEADER), &nBytesRead, 0);

	/*
	��������� � ���������� https://msdn.microsoft.com/ru-ru/library/windows/desktop/dd183374(v=vs.85).aspx
	https://msdn.microsoft.com/ru-ru/library/windows/desktop/dd183376(v=vs.85).aspx

	*/

	width = infoh.biWidth;
	height = infoh.biHeight;
	*Wigth = width;
	*Height = height;


	//
	size = width * 3 + width % 4;
	size = size * height;
	nBytesRead = fileh.bfOffBits;
	*arr = (RGBTRIPLE *)malloc(size);

	while (read_size < size)
	{
		ReadFile(file, *arr + i, sizeof(RGBTRIPLE), &nBytesRead, 0);
		read_size += nBytesRead;
		i++;
	}
	CloseHandle(file);
	return 1;

}




//��������� BMP � ������ ������.  ���� ������� ���������� ��������������� 4�� ������� (R G B A)
//��� �� ��� �������� ����������������, � BMP ��� �������� ������������.
int Texture::RGBtoChar(__in RGBTRIPLE *arr, __in int width, __in int height, __out char **out)
{
	int size = height*width * 4;
	char *mas;
	if (width <= 0 || height <= 0)
	{
		return 0;
	}

	mas = (char *)malloc(size*sizeof(char));
	for (int i = height - 1; i >= 0; i--)
		for (int j = 0; j < width; j++)
		{
			*(mas + i*width * 4 + j * 4 + 0) = arr[(i)*width + j].rgbtRed;
			*(mas + i*width * 4 + j * 4 + 1) = arr[(i)*width + j].rgbtGreen;
			*(mas + i*width * 4 + j * 4 + 2) = arr[(i)*width + j].rgbtBlue;
			*(mas + i*width * 4 + j * 4 + 3) = 255;
		}
	*out = mas;
	return 1;

}