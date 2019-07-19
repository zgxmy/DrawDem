#include "D8.h"
#include <windows.h>

D8::D8(const DemData& dem)
{
	nrows = dem.nrows;
	ncols = dem.ncols;

	NIP = new int *[nrows];
	for (int i = 0; i < nrows; i++) {
		NIP[i] = new int[ncols];
		for (int j = 0; j < ncols; j++) {
			NIP[i][j] = 0;
		}
	}

	Water = new int *[nrows];
	for (int i = 0; i < nrows; i++) {
		Water[i] = new int[ncols];
		for (int j = 0; j < ncols; j++) {
			Water[i][j] = 0;
		}
	}

	dirData = new unsigned char *[nrows];
	for (int i = 0; i < nrows; i++)
		dirData[i] = new unsigned char[ncols];
	int* slope = new int[8];
	for (int r = 0; r < nrows; r++) {
		for (int c = 0; c < ncols; c++) {
			//set zero : nop = 0
			dirData[r][c] = 0;
			if (r == 0 || r == nrows - 1 || c == 0 || c == ncols - 1) {
				//skip border points
				continue;
			}	
			slope[0] = dem.data[r][c] - dem.data[r][c + 1];
			slope[1] = (dem.data[r][c] - dem.data[r - 1][c + 1]) / sqrt2;
			slope[2] = dem.data[r][c] - dem.data[r - 1][c];
			slope[3] = (dem.data[r][c] - dem.data[r - 1][c - 1]) / sqrt2;
			slope[4] = dem.data[r][c] - dem.data[r][c - 1];
			slope[5] = (dem.data[r][c] - dem.data[r + 1][c - 1]) / sqrt2;
			slope[6] = dem.data[r][c] - dem.data[r + 1][c];
			slope[7] = (dem.data[r][c] - dem.data[r + 1][c + 1]) / sqrt2;
			int maxId = -1;
			float maxVal = 0;
			for (int i = 0; i < 8; i++) {
				if (slope[i] > maxVal) {
					maxId = i;
					maxVal = slope[i];
				}			
			}
			if (maxId == -1)
				continue;
			dirData[r][c] = 1 << maxId;
			switch (maxId)
			{
			case 0: {
				NIP[r][c + 1]++;
				break;
			}
			case 1: {
				NIP[r - 1][c + 1]++;
				break;
			}
			case 2: {
				NIP[r - 1][c]++;
				break;
			}
			case 3: {
				NIP[r - 1][c - 1]++;
				break;
			}
			case 4: {
				NIP[r][c - 1]++;
				break;
			}
			case 5: {
				NIP[r + 1][c - 1]++;
				break;
			}
			case 6: {
				NIP[r + 1][c]++;
				break;
			}
			case 7: {
				NIP[r + 1][c + 1]++;
				break;
			}
			default:
				break;
			}
		}
	}
	int r1 = 0, c1 = 0;
	bool flag = true;
	for (int r = 0; r < nrows; r++) {
		for (int c = 0; c < ncols; c++) {
			flag = true;
			r1 = r;
			c1 = c;
			//CalWater(r, c,*this);
			while (flag) {
				if (r1 == 0 || r1 == nrows - 1 || c1 == 0 || c1 == ncols - 1)
					break;
				switch (dirData[r1][c1])
				{
				case 0: {
					flag = false;
					break;
				}
				case 1: {
					Water[r1, c1 + 1]++;
					c1 ++;
					break;
				}
				case 2: {
					Water[r1 - 1, c1 + 1]++;
					r1 --; c1 ++;
					break;
				}
				case 4: {
					Water[r1 - 1][c1]++;
					r1 --;
					
					break;
				}
				case 8: {
					Water[r1 - 1, c1 - 1]++;
					r1 --; c1 --;
					break;
				}
				case 16: {
					Water[r1, c1 - 1]++;
					c1 --;
					break;
				}
				case 32: {
					Water[r1 + 1, c1 - 1]++;
					r1 ++; c1 --;
					break;
				}
				case 64: {
					Water[r1 + 1, c1]++;
					r1 ++; 
					break;
				}
				case 128: {
					Water[r1 + 1, c1 + 1]++;
					r1 ++; c1 ++;
					break;
				}
				default:
					break;
				}
			}
		}
	}
	delete[] slope;
}


void D8::CalWater(int r,int c,D8 &d) {
	if (r == 0 || r == nrows - 1 || c == 0 || c == ncols - 1)
		return;
	switch (dirData[r][c])
	{
	case 0: {
		break;
	}
	case 1: {
		d.Water[r, c + 1]++;
		CalWater(r, c+1,d);
		break;
	}
	case 2: {
		d.Water[r - 1, c + 1]++;
		CalWater(r - 1, c + 1,d);
		break;
	}
	case 4: {
		d.Water[r - 1][c]++;
		CalWater(r - 1, c,d);
		break;
	}
	case 8: {
		d.Water[r - 1, c - 1]++;
		CalWater(r - 1, c - 1, d);
		break;
	}
	case 16: {
		d.Water[r, c - 1]++;
		CalWater(r, c - 1, d);
		break;
	}
	case 32: {
		d.Water[r + 1, c - 1]++;
		CalWater(r + 1, c - 1, d);
		break;
	}
	case 64: {
		d.Water[r + 1, c ]++;
		CalWater(r + 1, c, d);
		break;
	}
	case 128: {
		d.Water[r + 1, c + 1]++;
		CalWater(r + 1, c + 1, d);
		break;
	}
	default:
		break;
	}
}

void D8::PrintData(int t) {
	//struct CONSOLE_FONT
	//{
	//	DWORD index;
	//	COORD dim;
	//};

	//typedef BOOL(WINAPI *PROCSETCONSOLEFONT)(HANDLE, DWORD);
	//typedef BOOL(WINAPI *PROCGETCONSOLEFONTINFO)(HANDLE, BOOL, DWORD, CONSOLE_FONT*);
	//typedef COORD(WINAPI *PROCGETCONSOLEFONTSIZE)(HANDLE, DWORD);
	//typedef DWORD(WINAPI *PROCGETNUMBEROFCONSOLEFONTS)();
	//typedef BOOL(WINAPI *PROCGETCURRENTCONSOLEFONT)(HANDLE, BOOL, CONSOLE_FONT*);

	//PROCSETCONSOLEFONT SetConsoleFont;
	//PROCGETCONSOLEFONTINFO GetConsoleFontInfo;
	//PROCGETCONSOLEFONTSIZE GConsoleFontSize;
	//PROCGETNUMBEROFCONSOLEFONTS GetNumberOfConsoleFonts;
	//PROCGETCURRENTCONSOLEFONT GCurrentConsoleFont;

	//int nNumFont;

	//HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	//HMODULE hKernel32 = GetModuleHandle("kernel32");

	//SetConsoleFont = (PROCSETCONSOLEFONT)GetProcAddress(hKernel32, "SetConsoleFont");
	//GetConsoleFontInfo = (PROCGETCONSOLEFONTINFO)GetProcAddress(hKernel32, "GetConsoleFontInfo");
	//GConsoleFontSize = (PROCGETCONSOLEFONTSIZE)GetProcAddress(hKernel32, "GetConsoleFontSize");
	//GetNumberOfConsoleFonts = (PROCGETNUMBEROFCONSOLEFONTS)GetProcAddress(hKernel32, "GetNumberOfConsoleFonts");
	//GCurrentConsoleFont = (PROCGETCURRENTCONSOLEFONT)GetProcAddress(hKernel32, "GetCurrentConsoleFont");

	//nNumFont = GetNumberOfConsoleFonts();
	//CONSOLE_FONT *pFonts = new CONSOLE_FONT[nNumFont];
	//GetConsoleFontInfo(hConsole, 0, nNumFont, pFonts);

	//SetConsoleFont(hConsole,0);
	

	//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), );
	for (int r = 1; r < nrows-1; r++) {
		for (int c = 1; c < ncols-1; c++) {

			if (Water[r][c] > t) {
				//if (NIP[r-1][c+1]>t|| NIP[r + 1][c] > t || NIP[r - 1][c] > t || NIP[r][c+1] > t || NIP[r][c-1] > t || NIP[r+1][c+1] > t || NIP[r-1][c-1] > t || NIP[r+1][c-1] > t) {
					cout << "¡ö";
				//}
				//else
					//cout << "  ";
			}
				
			else
				cout << "  ";
		}
		cout << endl;
	}
}

void D8::PrintDataNIP(int t) {
	//struct CONSOLE_FONT
	//{
	//	DWORD index;
	//	COORD dim;
	//};

	//typedef BOOL(WINAPI *PROCSETCONSOLEFONT)(HANDLE, DWORD);
	//typedef BOOL(WINAPI *PROCGETCONSOLEFONTINFO)(HANDLE, BOOL, DWORD, CONSOLE_FONT*);
	//typedef COORD(WINAPI *PROCGETCONSOLEFONTSIZE)(HANDLE, DWORD);
	//typedef DWORD(WINAPI *PROCGETNUMBEROFCONSOLEFONTS)();
	//typedef BOOL(WINAPI *PROCGETCURRENTCONSOLEFONT)(HANDLE, BOOL, CONSOLE_FONT*);

	//PROCSETCONSOLEFONT SetConsoleFont;
	//PROCGETCONSOLEFONTINFO GetConsoleFontInfo;
	//PROCGETCONSOLEFONTSIZE GConsoleFontSize;
	//PROCGETNUMBEROFCONSOLEFONTS GetNumberOfConsoleFonts;
	//PROCGETCURRENTCONSOLEFONT GCurrentConsoleFont;

	//int nNumFont;

	//HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	//HMODULE hKernel32 = GetModuleHandle("kernel32");

	//SetConsoleFont = (PROCSETCONSOLEFONT)GetProcAddress(hKernel32, "SetConsoleFont");
	//GetConsoleFontInfo = (PROCGETCONSOLEFONTINFO)GetProcAddress(hKernel32, "GetConsoleFontInfo");
	//GConsoleFontSize = (PROCGETCONSOLEFONTSIZE)GetProcAddress(hKernel32, "GetConsoleFontSize");
	//GetNumberOfConsoleFonts = (PROCGETNUMBEROFCONSOLEFONTS)GetProcAddress(hKernel32, "GetNumberOfConsoleFonts");
	//GCurrentConsoleFont = (PROCGETCURRENTCONSOLEFONT)GetProcAddress(hKernel32, "GetCurrentConsoleFont");

	//nNumFont = GetNumberOfConsoleFonts();
	//CONSOLE_FONT *pFonts = new CONSOLE_FONT[nNumFont];
	//GetConsoleFontInfo(hConsole, 0, nNumFont, pFonts);

	//SetConsoleFont(hConsole,0);


	//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), );
	for (int r = 1; r < nrows - 1; r++) {
		for (int c = 1; c < ncols - 1; c++) {

			if (NIP[r][c] > t) {
				if (NIP[r-1][c+1]>t|| NIP[r + 1][c] > t || NIP[r - 1][c] > t || NIP[r][c+1] > t || NIP[r][c-1] > t || NIP[r+1][c+1] > t || NIP[r-1][c-1] > t || NIP[r+1][c-1] > t) {
				cout << "¡ö";
				}
				else
				cout << "  ";
			}

			else
				cout << "  ";
		}
		cout << endl;
	}
}

D8::~D8()
{
	for (int i = 0; i < nrows; i++)
		delete[] dirData[i];
	delete[]dirData;
	for (int i = 0; i < nrows; i++)
		delete[] Water[i];
	delete[]Water;
	for (int i = 0; i < nrows; i++)
		delete[] NIP[i];
	delete[]NIP;
}

