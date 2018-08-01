#ifndef __FACEDET_H__
#define __FACEDET_H__
#include "pic_data.h"
//#include "classifier_float.h"

extern img orig ;
extern img scale_img;

#define WINX 19
#define WINY 19

typedef struct _scaler_dims {
	int16_t w;
	int16_t h;
} scaler_dims;

#if 0 //scaling factor = 0.75
#define MAX_SCALES 8
scaler_dims sdims[MAX_SCALES] = { 	
	{240,180},
	{180,135},
	{135,101},
	{101,76},
	{76,57},
	{57,43},
	{32,24},
	{18,13}
};
#endif


#define GET_PIX(img,x,y) (img->pix[((y)*img->w) + (x)])

typedef struct _scaled_obj_list {
	int scale;
	int16_t x;
	int16_t y;
} scaled_obj_list;

#define OBJ_LIST_MAX 512 

typedef struct _obj_list {
	scaled_obj_list obj_list[OBJ_LIST_MAX];
	int index;
} obj_list;




static void init_obj_list(obj_list * list);
static void add_to_obj_list(obj_list * list, int x, int y, int scale);
static void resize(const img* srcImage, img* dstImage);
static inline int16_t calc_mct(img * img, int x, int y, int nx, int ny);
void init_dummy_classifiers(void);
void facedet_init(void);
void facedet_run(char * file_name);
void facedet_run(void);
#endif
