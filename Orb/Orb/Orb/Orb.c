#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>
#include <mbstring.h>

#define bool char
#define true 1
#define false 0
#define Height 768
#define Length 1024
#define Maxn 16			//选取多少对特征描述子
#define ScoreBound	13	//描述子成绩界限

float pic1[Height][Length][3];
float pic2[Height][Length][3];

float grayPic1[Height][Length];
float grayPic2[Height][Length];

int DotNumber1 = 0;
int DotNumber2 = 0;

struct FeatureDot {
	int Match;			//pic1的这个featureDot匹配pic2的第几个FeatureDot
	int use;
	int row;			//height
	int colum;			//width
	int Spec[Maxn];
	int score;
}fPic1[15000],fPic2[15000];

//FeatureDot fPic1[10000];

enum BOOL {
	FLASE,TRUE
};

typedef struct
{
	//unsigned short    bfType;  
	unsigned long    bfSize;
	unsigned short    bfReserved1;
	unsigned short    bfReserved2;
	unsigned long    bfOffBits;
} ClBitMapFileHeader;

typedef struct
{
	unsigned long  biSize;
	long   biWidth;
	long   biHeight;
	unsigned short   biPlanes;
	unsigned short   biBitCount;
	unsigned long  biCompression;
	unsigned long  biSizeImage;
	long   biXPelsPerMeter;
	long   biYPelsPerMeter;
	unsigned long   biClrUsed;
	unsigned long   biClrImportant;
} ClBitMapInfoHeader;

typedef struct
{
	unsigned char rgbBlue; //该颜色的蓝色分量  
	unsigned char rgbGreen; //该颜色的绿色分量  
	unsigned char rgbRed; //该颜色的红色分量  
	unsigned char rgbReserved; //保留值  
} ClRgbQuad;

typedef struct
{
	int width;
	int height;
	int channels;
	unsigned char* imageData;
}ClImage;
ClImage* clLoadImage(char* path,int id);
//ClImage* clLoadImage(char* path,float arr[][3]);
bool clSaveImage(char* path, ClImage* bmpImg);

ClImage* clLoadImage(char* path,int id)
//ClImage* clLoadImage(char* path,float arr[][3])
{
	int count = 0;
	ClImage* bmpImg;
	FILE* pFile;
	unsigned short fileType;
	ClBitMapFileHeader bmpFileHeader;
	ClBitMapInfoHeader bmpInfoHeader;
	int channels = 1;
	int width = 0;
	int height = 0;
	int step = 0;
	int offset = 0;
	float As = 0;
	unsigned char pixVal;
	ClRgbQuad* quad;
	int i, j, k;
	int grayAns = 0;
	bmpImg = (ClImage*)malloc(sizeof(ClImage));
	pFile = fopen(path, "rb");
	if (!pFile)
	{
		free(bmpImg);
		return NULL;
	}

	fread(&fileType, sizeof(unsigned short), 1, pFile);
	if (fileType == 0x4D42) //string "BM"
	{
		//printf("bmp file! \n");  

		fread(&bmpFileHeader, sizeof(ClBitMapFileHeader), 1, pFile);
		/*printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n");
		printf("bmp文件头信息：\n");
		printf("文件大小：%d \n", bmpFileHeader.bfSize);
		printf("保留字：%d \n", bmpFileHeader.bfReserved1);
		printf("保留字：%d \n", bmpFileHeader.bfReserved2);
		printf("位图数据偏移字节数：%d \n", bmpFileHeader.bfOffBits);*/

		fread(&bmpInfoHeader, sizeof(ClBitMapInfoHeader), 1, pFile);
		/*printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n");
		printf("bmp文件信息头\n");
		printf("结构体长度：%d \n", bmpInfoHeader.biSize);
		printf("位图宽度：%d \n", bmpInfoHeader.biWidth);
		printf("位图高度：%d \n", bmpInfoHeader.biHeight);
		printf("位图平面数：%d \n", bmpInfoHeader.biPlanes);
		printf("颜色位数：%d \n", bmpInfoHeader.biBitCount);
		printf("压缩方式：%d \n", bmpInfoHeader.biCompression);
		printf("实际位图数据占用的字节数：%d \n", bmpInfoHeader.biSizeImage);
		printf("X方向分辨率：%d \n", bmpInfoHeader.biXPelsPerMeter);
		printf("Y方向分辨率：%d \n", bmpInfoHeader.biYPelsPerMeter);
		printf("使用的颜色数：%d \n", bmpInfoHeader.biClrUsed);
		printf("重要颜色数：%d \n", bmpInfoHeader.biClrImportant);
		printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n");*/

		if (bmpInfoHeader.biBitCount == 8)
		{
			//printf("该文件有调色板，即该位图为非真彩色\n\n");  
			channels = 1;
			width = bmpInfoHeader.biWidth;
			height = bmpInfoHeader.biHeight;
			offset = (channels*width) % 4;
			if (offset != 0)
			{
				offset = 4 - offset;
			}
			//bmpImg->mat = kzCreateMat(height, width, 1, 0);  
			bmpImg->width = width;
			bmpImg->height = height;
			bmpImg->channels = 1;
			bmpImg->imageData = (unsigned char*)malloc(sizeof(unsigned char)*width*height);
			step = channels * width;

			quad = (ClRgbQuad*)malloc(sizeof(ClRgbQuad) * 256);
			fread(quad, sizeof(ClRgbQuad), 256, pFile);
			free(quad);

			for (i = 0; i < height; i++)
			{
				for (j = 0; j < width; j++)
				{
					fread(&pixVal, sizeof(unsigned char), 1, pFile);
					bmpImg->imageData[(height - 1 - i)*step + j] = pixVal;   //每次一个像素处理
				}
				if (offset != 0)
				{
					for (j = 0; j < offset; j++)
					{
						fread(&pixVal, sizeof(unsigned char), 1, pFile);
					}
				}
			}
		}
		else if (bmpInfoHeader.biBitCount == 24)
		{
			printf("该位图为位真彩色\n\n");  
			channels = 3;
			width = bmpInfoHeader.biWidth;
			height = bmpInfoHeader.biHeight;

			bmpImg->width = width;
			bmpImg->height = height;
			printf("%d %d\n", height, width);
			bmpImg->channels = 3;
			bmpImg->imageData = (unsigned char*)malloc(sizeof(unsigned char)*width * 3 * height);
			step = channels * width;

			offset = (channels*width) % 4;
			if (offset != 0)
			{
				offset = 4 - offset;
			}

			fseek(pFile, bmpFileHeader.bfOffBits - sizeof(bmpInfoHeader) - sizeof(bmpFileHeader), SEEK_CUR);  

			for (i = 0; i < height; i++)
			{
				for (j = 0; j < width; j++)
				{
					for (k = 0; k < 3; k++)
					{
						fread(&pixVal, sizeof(unsigned char), 1, pFile);
						count++;
						bmpImg->imageData[(height - 1 - i)*step + j * 3 + k] = pixVal;  //
						As = (float)pixVal;
						if (id == 1) {
							pic1[i][j][k] = As;
						}
						else {
							pic2[i][j][k] = As;
						}
					}
					if (id == 1) {
						grayPic1[i][j] = pic1[i][j][0]*0.299 + pic1[i][j][1]*0.587 + pic1[i][j][2]*0.114;
						//printf("%f\n", grayPic1[height][width]);
					}
					else {
						grayPic2[i][j] = pic2[i][j][0] * 0.299 + pic2[i][j][1] * 0.587 + pic2[i][j][2] * 0.114;
					}
					//kzSetMat(bmpImg->mat, height-1-i, j, kzScalar(pixVal[0], pixVal[1], pixVal[2]));  
				}
				if (offset != 0)
				{
					for (j = 0; j < offset; j++)
					{
						fread(&pixVal, sizeof(unsigned char), 1, pFile);
						//count++;
					}
				}
			}
		}
	}
	return bmpImg;
}

bool clSaveImage(char* path, ClImage* bmpImg)
{
	FILE *pFile;
	int ans;
	unsigned short fileType;
	ClBitMapFileHeader bmpFileHeader;
	ClBitMapInfoHeader bmpInfoHeader;
	int step;
	int offset;
	unsigned char pixVal = '\0';
	int i, j;
	ClRgbQuad* quad;

	pFile = fopen(path, "wb");
	if (!pFile)
	{
		return false;
	}

	fileType = 0x4D42;
	fwrite(&fileType, sizeof(unsigned short), 1, pFile);

	if (bmpImg->channels == 3)//24位，通道，彩图  
	{
		step = bmpImg->channels*bmpImg->width;
		offset = step % 4;
		if (offset != 4)
		{
			step += 4 - offset;
		}

		bmpFileHeader.bfSize = bmpImg->height*step + 54;
		bmpFileHeader.bfReserved1 = 0;
		bmpFileHeader.bfReserved2 = 0;
		bmpFileHeader.bfOffBits = 54;
		fwrite(&bmpFileHeader, sizeof(ClBitMapFileHeader), 1, pFile);

		bmpInfoHeader.biSize = 40;
		bmpInfoHeader.biWidth = bmpImg->width;
		bmpInfoHeader.biHeight = bmpImg->height;
		bmpInfoHeader.biPlanes = 1;
		bmpInfoHeader.biBitCount = 24;
		bmpInfoHeader.biCompression = 0;
		bmpInfoHeader.biSizeImage = bmpImg->height*step;
		bmpInfoHeader.biXPelsPerMeter = 0;
		bmpInfoHeader.biYPelsPerMeter = 0;
		bmpInfoHeader.biClrUsed = 0;
		bmpInfoHeader.biClrImportant = 0;
		fwrite(&bmpInfoHeader, sizeof(ClBitMapInfoHeader), 1, pFile);

		for (i = bmpImg->height - 1; i > -1; i--)
		{
			for (j = 0; j < bmpImg->width; j++)
			{
				pixVal = bmpImg->imageData[i*bmpImg->width * 3 + j * 3];
				ans = (int)pixVal;
				//printf("%d\n", ans);
				fwrite(&pixVal, sizeof(unsigned char), 1, pFile);
				pixVal = bmpImg->imageData[i*bmpImg->width * 3 + j * 3 + 1];
				ans = (int)pixVal;
				//printf("%d\n", ans);
				fwrite(&pixVal, sizeof(unsigned char), 1, pFile);
				pixVal = bmpImg->imageData[i*bmpImg->width * 3 + j * 3 + 2];
				ans = (int)pixVal;
				//printf("%d\n", ans);
				fwrite(&pixVal, sizeof(unsigned char), 1, pFile);
				//getchar();
			}
			if (offset != 0)
			{
				for (j = 0; j < offset; j++)
				{
					pixVal = 0;
					fwrite(&pixVal, sizeof(unsigned char), 1, pFile);
				}
			}
		}
	}
	else if (bmpImg->channels == 1)//8位，单通道，灰度图  
	{
		step = bmpImg->width;
		offset = step % 4;
		if (offset != 4)
		{
			step += 4 - offset;
		}

		bmpFileHeader.bfSize = 54 + 256 * 4 + bmpImg->width;
		bmpFileHeader.bfReserved1 = 0;
		bmpFileHeader.bfReserved2 = 0;
		bmpFileHeader.bfOffBits = 54 + 256 * 4;
		fwrite(&bmpFileHeader, sizeof(ClBitMapFileHeader), 1, pFile);

		bmpInfoHeader.biSize = 40;
		bmpInfoHeader.biWidth = bmpImg->width;
		bmpInfoHeader.biHeight = bmpImg->height;
		bmpInfoHeader.biPlanes = 1;
		bmpInfoHeader.biBitCount = 8;
		bmpInfoHeader.biCompression = 0;
		bmpInfoHeader.biSizeImage = bmpImg->height*step;
		bmpInfoHeader.biXPelsPerMeter = 0;
		bmpInfoHeader.biYPelsPerMeter = 0;
		bmpInfoHeader.biClrUsed = 256;
		bmpInfoHeader.biClrImportant = 256;
		fwrite(&bmpInfoHeader, sizeof(ClBitMapInfoHeader), 1, pFile);

		quad = (ClRgbQuad*)malloc(sizeof(ClRgbQuad) * 256);
		for (i = 0; i < 256; i++)
		{
			quad[i].rgbBlue = i;
			quad[i].rgbGreen = i;
			quad[i].rgbRed = i;
			quad[i].rgbReserved = 0;
		}
		fwrite(quad, sizeof(ClRgbQuad), 256, pFile);
		free(quad);

		for (i = bmpImg->height - 1; i > -1; i--)
		{
			for (j = 0; j < bmpImg->width; j++)
			{
				pixVal = bmpImg->imageData[i*bmpImg->width + j];
				fwrite(&pixVal, sizeof(unsigned char), 1, pFile);
			}
			if (offset != 0)
			{
				for (j = 0; j < offset; j++)
				{
					pixVal = 0;
					fwrite(&pixVal, sizeof(unsigned char), 1, pFile);
				}
			}
		}
	}
	fclose(pFile);

	return true;
}

void findFeatureDot()
{
	int i, j;
	int num = 0;
	int thresHold = 62;
	int score = 0;
	for(i=3;i <= 764;i++)
		for (j = 3; j <= 1020; j++)
		{
			if (abs(grayPic1[i - 3][j] - grayPic1[i][j]) > thresHold)score += 1;
			if (abs(grayPic1[i - 3][j - 1] - grayPic1[i][j]) > thresHold)score += 1;
			if (abs(grayPic1[i - 2][j - 2] - grayPic1[i][j]) > thresHold)score += 1;
			if (abs(grayPic1[i - 1][j - 3] - grayPic1[i][j]) > thresHold)score += 1;
			if (abs(grayPic1[i][j - 3] - grayPic1[i][j]) > thresHold)score += 1;
			if (abs(grayPic1[i + 1][j - 3] - grayPic1[i][j]) > thresHold)score += 1;
			if (abs(grayPic1[i + 2][j - 2] - grayPic1[i][j]) > thresHold)score += 1;
			if (abs(grayPic1[i + 3][j - 1] - grayPic1[i][j]) > thresHold)score += 1;
			if (abs(grayPic1[i + 3][j] - grayPic1[i][j]) > thresHold)score += 1;
			if (abs(grayPic1[i + 3][j + 1] - grayPic1[i][j]) > thresHold)score += 1;
			if (abs(grayPic1[i + 2][j + 2] - grayPic1[i][j]) > thresHold)score += 1;
			if (abs(grayPic1[i + 1][j + 3] - grayPic1[i][j]) > thresHold)score += 1;
			if (abs(grayPic1[i][j + 3] - grayPic1[i][j]) > thresHold)score += 1;
			if (abs(grayPic1[i - 1][j + 3] - grayPic1[i][j]) > thresHold)score += 1;
			if (abs(grayPic1[i - 2][j + 2] - grayPic1[i][j]) > thresHold)score += 1;
			if (abs(grayPic1[i - 3][j + 1] - grayPic1[i][j]) > thresHold)score += 1;
			//printf("%d", score);
			if (score >= 4) {				//FeatureDot
				num++;
				// 特征算子计算 16位特征算子，有12相同即为相同特征
				fPic1[num - 1].use = 1;
				fPic1[num - 1].row = i;
				fPic1[num - 1].colum = j;
				fPic1[num - 1].Match = -1;
				fPic1[num - 1].Spec[0] = grayPic1[i - 3][j] > grayPic1[i][j] ? 0 : 1;
				fPic1[num - 1].Spec[1] = grayPic1[i - 3][j - 1] > grayPic1[i][j] ? 0 : 1;
				fPic1[num - 1].Spec[2] = grayPic1[i - 2][j - 2] > grayPic1[i][j] ? 0 : 1;
				fPic1[num - 1].Spec[3] = grayPic1[i - 1][j - 3] > grayPic1[i][j] ? 0 : 1;
				fPic1[num - 1].Spec[4] = grayPic1[i][j - 3] > grayPic1[i][j] ? 0 : 1;
				fPic1[num - 1].Spec[5] = grayPic1[i + 1][j - 3] > grayPic1[i][j] ? 0 : 1;
				fPic1[num - 1].Spec[6] = grayPic1[i + 2][j - 2] > grayPic1[i][j] ? 0 : 1;
				fPic1[num - 1].Spec[7] = grayPic1[i + 3][j - 1] > grayPic1[i][j] ? 0 : 1;
				fPic1[num - 1].Spec[8] = grayPic1[i + 3][j] > grayPic1[i][j] ? 0 : 1;
				fPic1[num - 1].Spec[9] = grayPic1[i + 3][j + 1] > grayPic1[i][j] ? 0 : 1;
				fPic1[num - 1].Spec[10] = grayPic1[i + 2][j + 2] > grayPic1[i][j] ? 0 : 1;
				fPic1[num - 1].Spec[11] = grayPic1[i + 1][j + 3] > grayPic1[i][j] ? 0 : 1;
				fPic1[num - 1].Spec[12] = grayPic1[i][j + 3] > grayPic1[i][j] ? 0 : 1;
				fPic1[num - 1].Spec[13] = grayPic1[i - 1][j + 3] > grayPic1[i][j] ? 0 : 1;
				fPic1[num - 1].Spec[14] = grayPic1[i - 2][j + 2] > grayPic1[i][j] ? 0 : 1;
				fPic1[num - 1].Spec[15] = grayPic1[i - 3][j + 1] > grayPic1[i][j] ? 0 : 1;
			}
			score = 0;
		}
	printf("%d\n", num);
	DotNumber1 = num;
	num = 0;
	for (i = 3; i <= 764; i++)
		for (j = 3; j <= 1020; j++)
		{
			if (abs(grayPic2[i - 3][j] - grayPic2[i][j]) > thresHold)score += 1;
			if (abs(grayPic2[i - 3][j - 1] - grayPic2[i][j]) > thresHold)score += 1;
			if (abs(grayPic2[i - 2][j - 2] - grayPic2[i][j]) > thresHold)score += 1;
			if (abs(grayPic2[i - 1][j - 3] - grayPic2[i][j]) > thresHold)score += 1;
			if (abs(grayPic2[i][j - 3] - grayPic2[i][j]) > thresHold)score += 1;
			if (abs(grayPic2[i + 1][j - 3] - grayPic2[i][j]) > thresHold)score += 1;
			if (abs(grayPic2[i + 2][j - 2] - grayPic2[i][j]) > thresHold)score += 1;
			if (abs(grayPic2[i + 3][j - 1] - grayPic2[i][j]) > thresHold)score += 1;
			if (abs(grayPic2[i + 3][j] - grayPic2[i][j]) > thresHold)score += 1;
			if (abs(grayPic2[i + 3][j + 1] - grayPic2[i][j]) > thresHold)score += 1;
			if (abs(grayPic2[i + 2][j + 2] - grayPic2[i][j]) > thresHold)score += 1;
			if (abs(grayPic2[i + 1][j + 3] - grayPic2[i][j]) > thresHold)score += 1;
			if (abs(grayPic2[i][j + 3] - grayPic2[i][j]) > thresHold)score += 1;
			if (abs(grayPic2[i - 1][j + 3] - grayPic2[i][j]) > thresHold)score += 1;
			if (abs(grayPic2[i - 2][j + 2] - grayPic2[i][j]) > thresHold)score += 1;
			if (abs(grayPic2[i - 3][j + 1] - grayPic2[i][j]) > thresHold)score += 1;
			//printf("%d", score);
			if (score >= 9) {				//FeatureDot
				num++;
				fPic2[num - 1].use = 1;
				fPic2[num - 1].row = i;
				fPic2[num - 1].colum = j;
				fPic2[num - 1].Match = -1;
				fPic2[num - 1].Spec[0] = grayPic2[i - 3][j] > grayPic2[i][j] ? 0 : 1;
				fPic2[num - 1].Spec[1] = grayPic2[i - 3][j - 1] > grayPic2[i][j] ? 0 : 1;
				fPic2[num - 1].Spec[2] = grayPic2[i - 2][j - 2] > grayPic2[i][j] ? 0 : 1;
				fPic2[num - 1].Spec[3] = grayPic2[i - 1][j - 3] > grayPic2[i][j] ? 0 : 1;
				fPic2[num - 1].Spec[4] = grayPic2[i][j - 3] > grayPic2[i][j] ? 0 : 1;
				fPic2[num - 1].Spec[5] = grayPic2[i + 1][j - 3] > grayPic2[i][j] ? 0 : 1;
				fPic2[num - 1].Spec[6] = grayPic2[i + 2][j - 2] > grayPic2[i][j] ? 0 : 1;
				fPic2[num - 1].Spec[7] = grayPic2[i + 3][j - 1] > grayPic2[i][j] ? 0 : 1;
				fPic2[num - 1].Spec[8] = grayPic2[i + 3][j] > grayPic2[i][j] ? 0 : 1;
				fPic2[num - 1].Spec[9] = grayPic2[i + 3][j + 1] > grayPic2[i][j] ? 0 : 1;
				fPic2[num - 1].Spec[10] = grayPic2[i + 2][j + 2] > grayPic2[i][j] ? 0 : 1;
				fPic2[num - 1].Spec[11] = grayPic2[i + 1][j + 3] > grayPic2[i][j] ? 0 : 1;
				fPic2[num - 1].Spec[12] = grayPic2[i][j + 3] > grayPic2[i][j] ? 0 : 1;
				fPic2[num - 1].Spec[13] = grayPic2[i - 1][j + 3] > grayPic2[i][j] ? 0 : 1;
				fPic2[num - 1].Spec[14] = grayPic2[i - 2][j + 2] > grayPic2[i][j] ? 0 : 1;
				fPic2[num - 1].Spec[15] = grayPic2[i - 3][j + 1] > grayPic2[i][j] ? 0 : 1;
			}
			score = 0;
			
		}
	printf("%d\n", num);
	DotNumber2 = num;
}

void printGray()
{
	int i, j;
	for (i = 0; i < 768; i++)
		for (j = 0; j < 1024; j++)
			printf("%f\n", grayPic1[i][j]);
}

void MatchDot()
{
	int i, j, k;
	int ans = 0;
	int score = 0;
	printf("dot1:%d     dot2:%d\n", DotNumber1, DotNumber2);
	for(i = 0;i < DotNumber1;i++)
		for (j = 0; j < DotNumber2; j++)
		{	
			if (fPic2[j].Match != -1)continue;
			for (k = 0; k < 16; k++) {
				if (fPic1[i].Spec[k] == fPic2[j].Spec[k])score++;
			}
			if (score > ScoreBound) {
				score = 0;
				fPic1[i].Match = j;
				fPic2[j].Match = i;
				break;
			}
			score = 0;
		}
	for (i = 0; i < DotNumber1; i++) {
		if (fPic1[i].Match == -1)ans++;
	}
	printf("成功匹配数量: %d\n", ans);
}

ClImage* clSplice(ClImage* img1, ClImage* img2)
{
	int ai;
	int test;
	int i = 0;
	int j, k;
	int ans = 0;
	char pixVal;
	int step3, step1, step2;
	float As;
	ClImage* img3 = (ClImage*)malloc(sizeof(ClImage));
	step1 = img1->channels * img1->width;
	step2 = img2->channels * img2->width;
	img3->channels = 3;
	img3->height = img1->height;
	img3->width = img1->width + img2->width;
	img3->imageData = (unsigned char*)malloc(sizeof(unsigned char)*img3->width * 3 * img3->height);
	step3 = img3->channels * img3->width;
	printf("%d  %d\n", _mbslen(img1->imageData), _mbslen(img2->imageData));


	strcpy(img3->imageData, img1->imageData);
	strcat(img3->imageData, img2->imageData);
	for (i = 0; i < img3->height; i++)
	{
		for (j = 0; j < img3->width; j++)
		{
			for (k = 0; k < 3; k++)
			{
				if (j < img1->width) {
					pixVal = img1->imageData[(img3->height - 1 - i)*step1 + j * 3 + k];
					img3->imageData[(img3->height - 1 - i)*step3 + j * 3 + k] = pixVal;
				}
				else {
					pixVal = img2->imageData[(img3->height - 1 - i)*step2 + (j-img1->width) * 3 + k];
					img3->imageData[(img3->height - 1 - i)*step3 + j * 3 + k] = pixVal;
				}
			}
		}
	}
	ai = 28;
	pixVal = (char)ai;
	printf("Dot1:%d    Dot2:%d\n", DotNumber1, DotNumber2);
	for (i = 0; i < DotNumber1; i++) {
		if (fPic1[i].Match != -1) {
			//printf("%d\n", test);
			//getchar();
			img3->imageData[(img3->height - 1 - fPic1[i].row)*step3 + fPic1[i].colum * 3 + 0] = pixVal;
			img3->imageData[(img3->height - 1 - fPic1[i].row)*step3 + fPic1[i].colum * 3 + 1] = pixVal;
			img3->imageData[(img3->height - 1 - fPic1[i].row)*step3 + fPic1[i].colum * 3 + 2] = pixVal;
		}
	}

	for (i = 0; i < DotNumber2; i++) {
		if (fPic2[i].Match != -1) {
			img3->imageData[(img3->height - 1 - fPic2[i].row)*step3 + (img1->width + fPic2[i].colum) * 3 + 0] = pixVal;
			img3->imageData[(img3->height - 1 - fPic2[i].row)*step3 + (img1->width + fPic2[i].colum) * 3 + 1] = pixVal;
			img3->imageData[(img3->height - 1 - fPic2[i].row)*step3 + (img1->width + fPic2[i].colum) * 3 + 2] = pixVal;
		}
	}

	int l = _mbslen(img3->imageData);
	printf("l:%d\n", l);
	return img3;
}

int main(int argc, char* argv[])
{
	//float pic1[10][3];
	ClImage* img1 = clLoadImage("1.bmp",1);
	ClImage* img2 = clLoadImage("2.bmp", 2);
	//printGray();
	findFeatureDot();
	MatchDot();
	ClImage* img3 = clSplice(img1, img2);
	bool flag = clSaveImage("result.bmp", img3);
	if (flag)
	{
		printf("save ok... \n");
	}

	while (1);
	return 0;
}

//ClImage* clLoadImage   这个函数读入图片，通过结构体存储
//clSaveImage             这个函数存储图片，写入文件
//