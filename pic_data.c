#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#include "pic_data.h"
#if 0
static void rgb2y(unsigned char * rgb, unsigned char * ybuf, int w, int h)
{
	int i,j;
	unsigned char r,g,b;
	unsigned int y,k;
	unsigned char a;
	unsigned char *p;
	float yy;
	k  = 0;
	for (i = 0; i < h; i++) 
			for (j = 0; j < w; j++) {
				p = rgb + (i * w * 3) + (j * 3);
				r = *p;
				g = *(p + 1);
				b = *(p + 2);
  				//y = av_clip_uint8((RY * r + GY * g + BY * b + ( 33 << (RGB2YUV_SHIFT - 1))) >> RGB2YUV_SHIFT);
            //u = av_clip_uint8((RU * r + GU * g + BU * b + (257 << (RGB2YUV_SHIFT - 1))) >> RGB2YUV_SHIFT);
            //v = av_clip_uint8((RV * r + GV * g + BV * b + (257 << (RGB2YUV_SHIFT - 1))) >> RGB2YUV_SHIFT);
				yy = 0.212671 * r + 0.715160 * g + 0.072169 * b;
            	ybuf[k] = __min(255, __max(0, yy));
			
				k++;
			}
}
#endif
/*for reading:*/
#define HI(num)	(((num) & 0x0000FF00) >> 8)
#define LO(num)	((num) & 0x000000FF)


void init_img(img * img, int w, int h, int stride)
{
	img->w = w;
	img->h = h;
	img->stride = stride;
	img->pix = malloc(w * h * stride);
	//printf("img init w=%d h=%d pix=0x%x\n",w,h,img->pix);
}

void init_img_noalloc(img * img, uint8_t * pixbuf, int w, int h, int stride)
{
	img->w = w;
	img->h = h;
	img->stride = stride;
	img->pix = pixbuf;
	//printf("img init w=%d h=%d pix=0x%x\n",w,h,img->pix);
}


void rgb2yuv(img * rgb, img * yuv)
{
	int i,j;
	float r,g,b;
	for (i=0;i<rgb->h;i++)
		for (j=0; j<rgb->w * 3; j+=3) {
			r = (float)rgb->pix[(i * rgb->w*3) + j];
			g = (float)rgb->pix[(i * rgb->w*3) + j + 1]; 
			b = (float)rgb->pix[(i * rgb->w*3) + j + 2]; 
			yuv->pix[(i * rgb->w*3) + j] 	  = (uint8_t)(0.299 * (float)r + 0.587 * (float)g + 0.114 * (float)b)   ; 
			yuv->pix[(i * rgb->w*3) + j + 1]  = (uint8_t)(128 + (-0.168736 * (float)r - 0.331264 * (float)g + 0.500 * (float)b)); 
			yuv->pix[(i * rgb->w*3) + j + 2]  = (uint8_t)(128 + (0.5 * (float)r - 0.418688 * (float)g - 0.081312 * (float)b)); 
		}
}

void rgb2y(img * rgb, img * y)
{
	int i,j,k;
	float r,g,b;
	for (i=0;i<rgb->h;i++)
		for (j=0,k=0; j<rgb->w * 3; j+=3,k++) {
			r = (float)rgb->pix[(i * rgb->w*3) + j];
			g = (float)rgb->pix[(i * rgb->w*3) + j + 1]; 
			b = (float)rgb->pix[(i * rgb->w*3) + j + 2]; 
			y->pix[(i * rgb->w) + k] = (uint8_t)(0.299 * (float)r + 0.587 * (float)g + 0.114 * (float)b)   ; 
		}
}



void SkipComments(FILE *fp)
{
	int ch;
	char line[100];

	while ((ch = fgetc(fp)) != EOF && isspace(ch))
		;
	if (ch == '#') {
		fgets(line, sizeof(line), fp);
		SkipComments(fp);
	} else
		fseek(fp, -1, SEEK_CUR);
}

int read_ppm(char * file_name, img * rgb)
{
	FILE *ppmFile;
	char version[3];
	int i, j,dummy;
	char lo;

	ppmFile = fopen(file_name, "rb");
	if (ppmFile == NULL) {
		perror("cannot open file to read");
		return 0;
	}

	fgets(version, sizeof(version), ppmFile);
	if (strcmp(version, "P6")) {
		fprintf(stderr, "Wrong file type!\n");
		return 0;
	}

	SkipComments(ppmFile);
	fscanf(ppmFile, "%d", &rgb->w);
	SkipComments(ppmFile);
	fscanf(ppmFile, "%d", &rgb->h);
	SkipComments(ppmFile);
	fscanf(ppmFile, "%d", &dummy);
	fgetc(ppmFile);

	//fprintf(stderr,"maxgrey = %d c=%d r=%d\n",data->max_gray, data->col, data->row);
	rgb->pix = malloc(rgb->w * rgb->h*3);
	rgb->stride = 3;
		
	for (i = 0; i < rgb->h; ++i)
			for (j = 0; j < rgb->w*3; j+=3) {
				lo = fgetc(ppmFile);
				rgb->pix[i * rgb->w*3 + j] = lo;
				lo = fgetc(ppmFile);
				rgb->pix[i * rgb->w*3 + j + 1] = lo;
				lo = fgetc(ppmFile);
				rgb->pix[i * rgb->w*3 + j + 2] = lo;
			}

	fclose(ppmFile);

	return 1;

	
}

int write_ppm(char *filename, img * rgb)
{
	FILE *ppmFile;
	int i, j;
	unsigned char lo;
	int color;
	ppmFile = fopen(filename, "wb");

	if (ppmFile == NULL) {
		perror("cannot open file to write");
		return 0;
	}

	fprintf(ppmFile, "P6 ");
	fprintf(ppmFile, "%d %d ", rgb->w, rgb->h);
	fprintf(ppmFile, "%d ", 255);

	for (i = 0; i < rgb->h; ++i) {
			for (j = 0; j < rgb->w*3 ; j+=3) {
				lo = rgb->pix[i * rgb->w*3 + j];
				fputc(lo, ppmFile);
				lo = rgb->pix[i * rgb->w*3 + j + 1];
				fputc(lo, ppmFile);
				lo = rgb->pix[i * rgb->w*3 + j + 2];
				fputc(lo, ppmFile);
			}
	}
	fclose(ppmFile);
}

int write_pgm(char *filename, img * rgb)
{
	FILE *pgmFile;
	int i, j;
	unsigned char lo;
	int color;
	pgmFile = fopen(filename, "wb");

	if (pgmFile == NULL) {
		perror("cannot open file to write");
		return 0;
	}

	fprintf(pgmFile, "P5 ");
	fprintf(pgmFile, "%d %d ", rgb->w, rgb->h);
	fprintf(pgmFile, "%d ", 255);

	for (i = 0; i < rgb->h; ++i) {
			for (j = 0; j < rgb->w ; j++) {
				lo = rgb->pix[(i * rgb->w) + j];
				fputc(lo, pgmFile);
			}
	}
	fclose(pgmFile);
}

int read_pgm(char * file_name, img * rgb)
{
	FILE *ppmFile;
	char version[3];
	int i, j,dummy;
	char lo;

	ppmFile = fopen(file_name, "rb");
	if (ppmFile == NULL) {
		perror("cannot open file to read");
		return 0;
	}

	fgets(version, sizeof(version), ppmFile);
	if (strcmp(version, "P5")) {
		fprintf(stderr, "Wrong file type!\n");
		return 0;
	}

	SkipComments(ppmFile);
	fscanf(ppmFile, "%d", &rgb->w);
	SkipComments(ppmFile);
	fscanf(ppmFile, "%d", &rgb->h);
	SkipComments(ppmFile);
	fscanf(ppmFile, "%d", &dummy);
	fgetc(ppmFile);

	//fprintf(stderr,"maxgrey = %d c=%d r=%d\n",data->max_gray, data->col, data->row);
	rgb->pix = malloc(rgb->w * rgb->h);
	rgb->stride = 1;
	
	for (i = 0; i < rgb->h; ++i) {
		for (j = 0; j < rgb->w ; j++) {
				lo = fgetc(ppmFile);
				rgb->pix[(i * rgb->w) + j]=lo;
			}
	}
	
	fclose(ppmFile);

	return 1;

	
}




