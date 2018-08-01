#ifndef __PGM_H__
#define __PGM_H__


typedef struct _cimg {
	int w;
	int h;
	int stride;
	uint8_t * pix;
} img;

int read_ppm(char * file_name, img * rgb);
int write_ppm(char * file_name, img * rgb);
void init_img_noalloc(img * img, uint8_t * pixbuf, int w, int h, int stride);
void init_img(img * img, int w, int h, int stride);
int read_pgm(char * file_name, img * rgb);
int write_pgm(char *filename, img * rgb);

#endif

