#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

using namespace std;

typedef unsigned __int8 BYTE;
typedef unsigned __int16 WORD;
typedef unsigned __int32 DWORD;
typedef __int32 LONG;

#pragma pack(push)
#pragma pack(1)
 
typedef struct tagBITMAPFILEHEADER
{
	WORD bfType;
	DWORD bfSize;
	WORD bfReserved1;
	WORD bfReserved2;
	DWORD bfOffBits;
} BitMapFileHeader;

typedef struct tagBITMAPINFOHEADER
{
	DWORD biSize;
	LONG biWidth;
	LONG biHeight;
	WORD biPlanes;
	WORD biBitCount;
	DWORD biCompression;
	DWORD biSizeImage;
	LONG biXPelsPerMeter;
	LONG biYPelsPerMeter;
	DWORD biClrUsed;
	DWORD biClrImportant;
} BitMapInfoHeader;

typedef struct tagRGBTRIPLE
{
	BYTE rgbBlue;
	BYTE rgbGreen;
	BYTE rgbRed;
} RGBtriple;

typedef struct
{
	RGBtriple color;
	int number;
} List;

#pragma pack(pop)

int bmp_reader();
int txt_reader();
void quickSort(List* numbers, int left, int right);
int bmp_writer();

long pixels_number;
BitMapFileHeader header;
BitMapInfoHeader info;
RGBtriple colors;
List* pixels;

int main()
{
	if (bmp_reader())
	{
		return 1;
	}
	if (txt_reader())
	{
		return 1;
	}

	quickSort(pixels, 0, pixels_number - 1);

	if (bmp_writer())
	{
		return 1;
	}

	delete(pixels);
	return 0;
}

int bmp_reader()
{
	BYTE storage;
	FILE* bmp_r;
	fopen_s(&bmp_r, "F:\\Install\\Visual Studio\\Проекты\\Дешифратор картинки\\Data\\24e.bmp", "rb");
	if (!bmp_r)
	{
		cout << "Could not open bmp file for reading" << endl;
		return 1;
	}

	fread_s(&header, sizeof(header), 1, sizeof(header), bmp_r);
	fread_s(&info, sizeof(info), 1, sizeof(info), bmp_r);
	fseek(bmp_r, header.bfOffBits, SEEK_SET);
	
	pixels_number = info.biWidth * info.biHeight;
	pixels = new (nothrow) List[pixels_number]; // динамическое выделение памяти под массив структур
	if (!pixels)
	{
		cout << "Failed to allocate memory" << endl;
		return 1;
	}

	for (int i = 0; i < pixels_number; i++)
	{
		fread_s(&colors, sizeof(colors), 1, sizeof(colors), bmp_r);
		storage = colors.rgbBlue;
		colors.rgbBlue = colors.rgbRed;
		colors.rgbRed = storage;
		pixels[i].color = colors;
	}
	fclose(bmp_r);

	return 0;
}

int txt_reader()
{
	FILE* txt;
	fopen_s(&txt, "F:\\Install\\Visual Studio\\Проекты\\Дешифратор картинки\\Data\\24.txt", "rb");
	if (!txt)
	{
		cout << "Could not open txt file for reading" << endl;
		return 1;
	}

	for (int i = 0; i < pixels_number; i++)
	{
		fscanf_s(txt, "%d", &pixels[i].number);
	}
	fclose(txt);

	return 0;
}

void quickSort(List* pixels, int left, int right)
{
	List pivot; // разрешающий элемент
	int l_hold = left; //левая граница
	int r_hold = right; // правая граница
	int new_edge; // новая граница
	pivot = pixels[left];
	while (left < right) // пока границы не сомкнутся
	{
		while ((pixels[right].number >= pivot.number) && (left < right))
		{
			right--; // сдвигаем правую границу пока элемент [right] больше [pivot]
		}
		if (left != right) // если границы не сомкнулись
		{
			pixels[left] = pixels[right]; // перемещаем элемент [right] на место разрешающего
			left++; // сдвигаем левую границу вправо
		}
		while ((pixels[left].number <= pivot.number) && (left < right))
		{
			left++; // сдвигаем левую границу пока элемент [left] меньше [pivot]
		}
		if (left != right) // если границы не сомкнулись
		{
			pixels[right] = pixels[left]; // перемещаем элемент [left] на место [right]
			right--; // сдвигаем правую границу влево
		}
	}
	pixels[left] = pivot; // ставим разрешающий элемент на место
	new_edge = left;
	left = l_hold;
	right = r_hold;
	if (left < new_edge) // Рекурсивно вызываем сортировку для левой и правой части массива
		quickSort(pixels, left, new_edge - 1);
	if (right > new_edge)
		quickSort(pixels, new_edge + 1, right);
}

int bmp_writer()
{
	FILE* bmp_w;

	fopen_s(&bmp_w, "F:\\Install\\Visual Studio\\Проекты\\Дешифратор картинки\\Data\\sorted.bmp", "wb");
	if (!bmp_w)
	{
		cout << "Could not open bmp file for writing" << endl;
		return 1;
	}

	fwrite(&header, sizeof(header), 1, bmp_w);
	fwrite(&info, sizeof(info), 1, bmp_w);
	fseek(bmp_w, header.bfOffBits, SEEK_SET);

	for (int i = 1; i <= info.biHeight; i++)
	{
		for (int j = 0; j < info.biWidth; j++)
		{
			fwrite(&pixels[pixels_number - (i * info.biWidth) + j].color, sizeof(RGBtriple), 1, bmp_w);
		}
	}

	return 0;
}