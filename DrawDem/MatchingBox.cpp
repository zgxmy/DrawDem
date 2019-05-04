#include "MatchingBox.h"

MatchingBox::MatchingBox() {
}

MatchingBox::MatchingBox(float height, DemData* d)
{
	this->height = height;
	this->d = d;
	lineSize = 0;
	ncols = d->ncols;
	nrows = d->nrows;
	sign = new char *[nrows];
	for (int i = 0; i < nrows; i++)
		sign[i] = new char[ncols];
	for (int r = 0; r < nrows; r++)
		for (int c = 0; c < ncols; c++)
		{
			if (d->data[r][c] < height)
				sign[r][c] = '-';
			else
				sign[r][c] = '+';
		}
	for (int r = 0; r < nrows - 1; r++)
		for (int c = 0; c < ncols - 1; c++)
		{
			CountLineSize(Cell(sign[r][c], sign[r + 1][c], sign[r][c + 1], sign[r + 1][c + 1]));
		}
	lines = new float[lineSize * 3 * 2];
	linePtr = lines;
	for (int r = 0; r < nrows - 1; r++)
		for (int c = 0; c < ncols - 1; c++)
		{
			AddLine(Cell(sign[r][c], sign[r + 1][c], sign[r][c + 1], sign[r + 1][c + 1]), r, c);
		}
}

unsigned int MatchingBox::Cell(char a, char b, char c, char d)
{
	unsigned int n = 0;
	if (a == '+') n += 1;
	if (b == '+') n += 2;
	if (c == '+') n += 4;
	if (d == '+') n += 8;
	return n;
}

void MatchingBox::CountLineSize(unsigned int i)
{
	switch (i) {
	case 0: {lineSize += 0; break; }
	case 1:
	case 2:
	case 3:
	case 4:
	case 5: {lineSize += 1; break; }
	case 6: {lineSize += 2; break; }
	case 7:
	case 8: {lineSize += 1; break; }
	case 9: {lineSize += 2; break; }
	case 10:
	case 11:
	case 12:
	case 13:
	case 14: {lineSize += 1; break; }
	case 15: {lineSize += 0; break; }
	default:
		break;
	}
}

void MatchingBox::AddPoint(unsigned int i, int r, int c) {
	float aH = d->data[r][c];
	float bH = d->data[r + 1][c];
	float cH = d->data[r][c + 1];
	float dH = d->data[r + 1][c + 1];
	float offset = 0.01f;
	float afterScaleH = (height - d->averHeight)*offset*0.1f;
	switch (i) {
	case 0: {
		float scale = (height - aH) / (bH - aH);
		*linePtr = c * offset;
		*linePtr++;
		*linePtr = (r + scale)*offset;
		*linePtr++;
		break;
	}
	case 1: {
		float scale = (height - bH) / (dH - bH);
		*linePtr = (c + scale)*offset;
		*linePtr++;
		*linePtr = (r + 1.0f)*offset;
		*linePtr++;
		break;
	}
	case 2: {
		float scale = (height - cH) / (dH - cH);
		*linePtr = (c + 1.0f)*offset;
		*linePtr++;
		*linePtr = (r + scale)*offset;
		*linePtr++;
		break;
	}
	case 3: {
		float scale = (height - aH) / (cH - aH);
		*linePtr = (c + scale)*offset;
		*linePtr++;
		*linePtr = r * offset;
		*linePtr++;
		break;
	}
	default:break;
	}
	*linePtr = afterScaleH;
	*linePtr++;
}
void MatchingBox::AddLine(unsigned int i, int r, int c) {
	switch (i) {
	case 0: {break; }
	case 1: {
		AddPoint(0, r, c);
		AddPoint(3, r, c);
		break; }
	case 2: {
		AddPoint(0, r, c);
		AddPoint(1, r, c);
		break; }
	case 3: {
		AddPoint(1, r, c);
		AddPoint(3, r, c);
		break; }
	case 4: {
		AddPoint(2, r, c);
		AddPoint(3, r, c);
		break; }
	case 5: {
		AddPoint(0, r, c);
		AddPoint(2, r, c);
		break; }
	case 6: {
		AddPoint(0, r, c);
		AddPoint(3, r, c);
		AddPoint(1, r, c);
		AddPoint(2, r, c);
		break; }
	case 7: {
		AddPoint(1, r, c);
		AddPoint(2, r, c);
		break; }
	case 8: {
		AddPoint(1, r, c);
		AddPoint(2, r, c);
		break; }
	case 9: {
		AddPoint(0, r, c);
		AddPoint(1, r, c);
		AddPoint(2, r, c);
		AddPoint(3, r, c);
		break; }
	case 10: {
		AddPoint(0, r, c);
		AddPoint(2, r, c);
		break; }
	case 11: {
		AddPoint(2, r, c);
		AddPoint(3, r, c);
		break; }
	case 12: {
		AddPoint(1, r, c);
		AddPoint(3, r, c);
		break; }
	case 13: {
		AddPoint(0, r, c);
		AddPoint(1, r, c);
		break; }
	case 14: {
		AddPoint(0, r, c);
		AddPoint(3, r, c);
		break; }
	case 15: {break; }
	default:
		break;
	}
}

MatchingBox::~MatchingBox()
{
	for (int i = 0; i < nrows; i++)
		delete[] sign[i];
	delete[]sign;
}
