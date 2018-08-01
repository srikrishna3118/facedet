#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

//#include "pic_data.h"

#include "facedet.h"
#include "classifier_float.h"
//#include "reisc_intrinsics.h"

img orig ;
img scale_img;

obj_list objlist;

#define VALUE
#define PYRAMID
#ifdef PYRAMID //scaling factor = 0.85
#define MAX_SCALES 16
scaler_dims sdims[MAX_SCALES] = {
 {320,240},
 {272,204},
 {231,172},
 {196,146},
 {167,124},
 {142,105},
 {121,89},
 {103,76},
 {88,65},
 {75,55},
 {64,47},
 {54,40},
 {46,34},
 {39,29},
 {33,25},
 {28,21}
};
#else
#define MAX_SCALES 1
scaler_dims sdims[MAX_SCALES] = {
	{21,21}
};
#endif

void dump_image(img* image,int x,int y){
	static int cnt=0;
	//static int cnt1=0;
	img a;
	int i,j,k;
	k=0;
	char str[256];
	int write;
	init_img(&a,19,19,1);
	for(i=y;i<y+19;i++){
		for(j=x;j<x+19;j++){
			a.pix[k]=image->pix[i*image->w+j];
			k++;
		}
	}
	sprintf(str,"scaledimages/output_%d.pgm",cnt++);
	write=write_pgm(str,&a);	
	//sprintf(str,"scaledimages/scaled_%d.pgm",cnt1++);
	//write=write_pgm(str,image);
}

char *binary (int16_t v) {
	static char binstr[10] ;
	int i ;

	binstr[9] = '\0' ;
	for (i=0; i<9; i++) {
		binstr[8-i] = v & 1 ? '1' : '0' ;
		v = v / 2 ;
	}

	return binstr ;
}

void draw_bounding_box(obj_list * list,img * img,int Maxindex,char * file_name){
	static int cnt=0;
	int xt,yt,scale,w,h;	
	uint8_t * ptr;
	int index,i;
	char str[256];
	float sfac;
	int length;
	int write;
	for(index=0;index<Maxindex; index++){
		scale = list->obj_list[index].scale;
		xt = list->obj_list[index].x;
		yt = list->obj_list[index].y;
		

		w = img->w;
		h = img->h;
		
		printf("sdims 0 =%d , scale= %d sdims[scale]=%d \n",sdims[0].w,scale,sdims[scale].w);
		sfac=(float)sdims[0].w/(float)sdims[scale].w;
		//sfac=1;
		printf("sfac = %f\n",sfac);
		xt=xt*sfac;
		yt=yt*sfac;
		length=19*sfac;
		//printf("length =%d \n",length);
		for(i=xt;i<(xt+length-1);i++)
			img->pix[(yt*w)+i]=255;

		for(i=xt;i<(xt+length-1);i++)
			img->pix[(yt +length-1)*w + i]=255;	
		

		for(i=yt;i<(yt+length-1);i++)
			img->pix[(i*w)+xt]=255;
	

		for(i=yt;i<(yt+length-1);i++)
			img->pix[(i*w)+xt+length-1]=255;	
	}
	
	sprintf(str,"face%s",file_name);
	write=write_pgm(str,img);

}

void draw_bounding_box(obj_list * list,img * img,int Maxindex){
	static int cnt=0;
	int xt,yt,scale,w,h;
	uint8_t * ptr;
	int index,i;
	char str[256];
	float sfac;
	int length;
	int write;
	for(index=0;index<Maxindex; index++){
		scale = list->obj_list[index].scale;
		xt = list->obj_list[index].x;
		yt = list->obj_list[index].y;


		w = img->w;
		h = img->h;

		printf("sdims 0 =%d , scale= %d sdims[scale]=%d \n",sdims[0].w,scale,sdims[scale].w);
		sfac=(float)sdims[0].w/(float)sdims[scale].w;
		//sfac=1;
		printf("sfac = %f\n",sfac);
		xt=xt*sfac;
		yt=yt*sfac;
		length=19*sfac;
		//printf("length =%d \n",length);
		for(i=xt;i<(xt+length-1);i++)
			img->pix[(yt*w)+i]=255;

		for(i=xt;i<(xt+length-1);i++)
			img->pix[(yt +length-1)*w + i]=255;


		for(i=yt;i<(yt+length-1);i++)
			img->pix[(i*w)+xt]=255;


		for(i=yt;i<(yt+length-1);i++)
			img->pix[(i*w)+xt+length-1]=255;
	}

	//sprintf(str,"%s.pgm",file_name);
	//write=write_pgm(str,img);

}

static void init_obj_list(obj_list * list)
{
	list->index = 0;
}

static void add_to_obj_list(obj_list * list, int x, int y, int scale)
{
	
	if (list->index < OBJ_LIST_MAX) {
		list->obj_list[list->index].scale = scale;
		list->obj_list[list->index].x = x;
		list->obj_list[list->index].y = y;
		printf("Obj added\n");
		list->index++;
	}
	else {
		printf("add_to_obj_list: max exceeded\n");
	}
}

static void resize(const img* srcImage, img* dstImage)
{
   	uint8_t* src;
	int32_t srcWidth;
	int32_t srcHeight;
	int32_t srcStride;
	int32_t pixelSize;

	uint8_t* dst;
	int32_t dstWidth;
	int32_t dstHeight;

	int32_t widthRatio;
	int32_t heightRatio;

	int32_t maxWidth;
	int32_t maxHeight;

	int32_t srcX, srcY, dX1, dY1, dX2, dY2;
	int32_t dstX1, srcY1, dstX2, srcY2;

	uint8_t *tmp1, *tmp2;
	uint8_t *p1, *p2, *p3, *p4;

	int32_t offset1;
	int32_t offset2;

	int32_t x;
	int32_t y;
	int32_t ch;

	int32_t srcRoiX;
	int32_t srcRoiY;
	
	src		= srcImage->pix;
	srcWidth	= srcImage->w;
	srcHeight	= srcImage->h;
	srcStride 	= srcWidth;
	pixelSize 	= 1;
	
	dst		= dstImage->pix;
	dstWidth	= dstImage->w;
	dstHeight	= dstImage->h;
	
	srcRoiX = 0;
	srcRoiY = 0;
	widthRatio	= (srcWidth << 12) / dstWidth;
	heightRatio	= ( srcHeight << 12) / dstHeight;

	/* Get horizontal and vertical limits. */
	maxWidth	= (srcWidth) - 1;
	maxHeight	= (srcHeight) - 1;
	
   	for (y = 0; y < dstHeight; y++ ) {

				/* Get Y from source. */ 
				srcY	= y * heightRatio + srcRoiY;
				srcY1	= srcY >> 12;
				srcY2	= (srcY1 == maxHeight) ? srcY1 : srcY1 + 1;
				dY1		= srcY - (srcY1 << 12);
				dY2		= (1 << 12) - dY1;

				/* Calculates the pointers to the two needed lines of the source. */
				tmp1 = src + srcY1 * srcStride;
				tmp2 = src + srcY2 * srcStride;

				// for each pixel
				for (x = 0; x < dstWidth; x++) {

					/* Get X from source. */ 
					srcX	= x * widthRatio + srcRoiX;
					dstX1	= srcX >> 12;
					dstX2	= (dstX1 == maxWidth) ? dstX1 : dstX1 + 1;
					dX1		= srcX - (dstX1 << 12);
					dX2		= (1 << 12) - dX1;

					/* Calculates the four points (p1,p2, p3, p4) of the source. */
					offset1 = dstX1 * pixelSize;
					offset2 = dstX2 * pixelSize;
					p1 = tmp1 + offset1;
					p2 = tmp1 + offset2;
					p3 = tmp2 + offset1;
					p4 = tmp2 + offset2;

					/* For each channel, interpolate the four points. */
					for (ch = 0; ch < pixelSize; ch++, dst++, p1++, p2++, p3++, p4++)
						*dst = (uint8_t)((dY2 * (dX2 * (*p1) + dX1 * (*p2)) 
						+ dY1 * (dX2 * (*p3) + dX1 * (*p4))) >> 24);
				}
			}
}

#if 0
static inline int16_t calc_mct(img * img, int x, int y, int nx, int ny)
{
	uint8_t p0, p1, p2, p3, p4, p5, p6, p7, p8;
	uint32_t sum;
	uint16_t mct=0;

	/* pixel positions..
	   p0 p1 p2
	   p3 p4 p5
	   p6 p7 p8
	*/

	p0 = GET_PIX(img,x+nx-1,y+ny-1);
	p1 = GET_PIX(img,x+nx,y+ny-1);
	p2 = GET_PIX(img,x+nx+1,y+ny-1);
	p3 = GET_PIX(img,x+nx-1,y+ny);
	p4 = GET_PIX(img,x+nx,y+ny); //center
	p5 = GET_PIX(img,x+nx+1,y+ny);
	p6 = GET_PIX(img,x+nx-1,y+ny+1);
	p7 = GET_PIX(img,x+nx,y+ny+1);
	p8 = GET_PIX(img,x+nx+1,y+ny+1);
	sum = p0 + p1 + p2 + p3 + p4 + p5 + p6 + p7 + p8;
	//compute average
	//sum = ((sum * 7282) + (1U<<15))>>16; //1/9 scaled by 2^16, with rounding
	sum = (sum * 7282)>>16; //1/9 scaled by 2^16, without rounding
	if (p0<sum)
		mct=1;
	mct=mct<<1;
	if (p1<sum) 
		mct++;
	mct=mct<<1;

	if (p2<sum)
		mct++;
	mct=mct<<1;

	if (p3<sum) 
		mct++;
	mct=mct<<1;
	
	if (p4<sum)
		mct++;
	mct=mct<<1;
	
	if (p5<sum) 
		mct++;
	mct=mct<<1;
	
	if (p6<sum)
		mct++;
	mct=mct<<1;

	if (p7<sum)
		mct++;
	mct=mct<<1;
	
	if (p8<sum)
		mct++;

	return mct;
}
#endif
#if 1
static inline int16_t calc_mct(img * img, int x, int y, int nx, int ny)
{
	uint32_t p0, p1, p2, p3, p4, p5, p6, p7, p8;
	//uint32_t sum;
	uint32_t sum;
	uint32_t mct=0;
	uint32_t temp=0;
	uint8_t * ptr;
	/* pixel positions..
	   p0 p1 p2
	   p3 p4 p5
	   p6 p7 p8
	*/

	//calc ptr to top row
	ptr = img->pix + ((y+ny-1)*img->w) + (x + nx - 1);
	p0 = ptr[0];
	sum=p0;
	p1 = ptr[1];
	sum+=p1;
	p2 = ptr[2];
	sum+=p2;

	//calc ptr to mid row
	ptr = img->pix + ((y+ny)*img->w) + (x + nx - 1);
	//ptr = ptr + img->w;
	p3 = ptr[0];
	sum+=p3;
	p4 = ptr[1];
	sum+=p4;
	p5 = ptr[2];
	sum+=p5;

	//calc ptr to bottom row
	//ptr = ptr + img->w;
	ptr = img->pix + ((y+ny+1)*img->w) + (x + nx - 1);
	p6 = ptr[0];
	sum+=p6;
	p7 = ptr[1];
	sum+=p7;
	p8 = ptr[2];
	sum+=p8;
	//printf("ber sum = %d \n",sum);
	//sum = p0 + p1 + p2 + p3 + p4 + p5 + p6 + p7 + p8;
	//compute average
	//sum = ((sum * 7282) + (1U<<15))>>16; //1/9 scaled by 2^16, with rounding
	sum = (sum * 7282)>>16; //1/9 scaled by 2^16, without rounding
	//printf("mct final sum = %d \n",sum);
#if 0
	mct = reisc_selectw_lt(p0,sum,1,0);
	mct = mct<<1;

	temp = mct + 1;
	mct = reisc_selectw_lt(p1,sum,temp,mct);
	mct = mct<<1;


	temp = mct + 1;
	mct = reisc_selectw_lt(p2,sum,temp,mct);
	mct = mct<<1;

	temp = mct + 1;
	mct = reisc_selectw_lt(p3,sum,temp,mct);
	mct = mct<<1;

	temp = mct + 1;
	mct = reisc_selectw_lt(p4,sum,temp,mct);
	mct = mct<<1;

	temp = mct + 1;
	mct = reisc_selectw_lt(p5,sum,temp,mct);
	mct = mct<<1;

	temp = mct + 1;
	mct = reisc_selectw_lt(p6,sum,temp,mct);
	mct = mct<<1;

	temp = mct + 1;
	mct = reisc_selectw_lt(p7,sum,temp,mct);
	mct = mct<<1;

	temp = mct + 1;
	mct = reisc_selectw_lt(p8,sum,temp,mct);
#endif


#if 0
	if (p0<sum)
		mct=1<<8;
	//mct=mct<<1;
	if (p1<sum) 
		mct|=(1<<7);

	if (p2<sum)
		mct|=(1<<6);

	if (p3<sum) 
		mct|=(1<<5);
	
	if (p4<sum)
		mct|=(1<<4);
	
	if (p5<sum) 
		mct|=(1<<3);
	
	if (p6<sum)
		mct|=(1<<2);

	if (p7<sum)
		mct|=(1<<1);
	
	if (p8<sum)
		mct|=1;


#endif

#if 0
//my logic of mct
	mct<<=1;
	if(p0<sum)
		temp=1;
	else
		temp=0;
	mct+=temp;
	
	mct<<=1;
	if(p1<sum)
		temp=1;
	else
		temp=0;
	mct+=temp;

	mct<<=1;
	if(p2<sum)
		temp=1;
	else
		temp=0;
	mct+=temp;

	mct<<=1;
	if(p3<sum)
		temp=1;
	else
		temp=0;
	mct+=temp;

	mct<<=1;
	if(p4<sum)
		temp=1;
	else
		temp=0;
	mct+=temp;

	mct<<=1;
	if(p5<sum)
		temp=1;
	else
		temp=0;
	mct+=temp;

	mct<<=1;
	if(p6<sum)
		temp=1;
	else
		temp=0;
	mct+=temp;

	mct<<=1;
	if(p7<sum)
		temp=1;
	else
		temp=0;
	mct+=temp;

	mct<<=1;
	if(p8<sum)
		temp=1;
	else
		temp=0;
	mct+=temp;

#endif

#if 1
//
	//printf("%d %d %d \n %d %d %d \n %d %d %d\n",p0,p1,p2,p3,p4,p5,p6,p7,p8);
	if (p0<sum)
		mct=1;
	//printf("%d\n",mct);
	mct=mct<<1;
	if (p1<sum) 
		mct++;
	//printf("%d\n",mct);
	mct=mct<<1;

	if (p2<sum)
		mct++;
	//printf("%d pix= %d sum = %d\n",mct,p2,sum);
	mct=mct<<1;

	if (p3<sum) 
		mct++;
	//printf("%d\n",mct);
	mct=mct<<1;
	
	if (p4<sum)
		mct++;
	//printf("%d\n",mct);
	mct=mct<<1;
	
	if (p5<sum) 
		mct++;
	//printf("%d\n",mct);
	mct=mct<<1;
	
	if (p6<sum)
		mct++;
	//printf("%d\n",mct);
	mct=mct<<1;

	if (p7<sum)
		mct++;
	//printf("%d\n",mct);
	mct=mct<<1;
	
	if (p8<sum)
		mct++;
	//printf("%d\n",mct);

#endif

	return mct;
}
#endif


void init_dummy_classifiers(void) 
{
	int i,j;
	for (i=0;i<18;i++) {
		classifier_pos_stage1[i].x = i;
		classifier_pos_stage1[i].y = i;
		for (j=0;j<512;j++)
			classifier_pos_stage1[i].mct_table[j] = 1;

	}
	
}

void facedet_init(void)
{
//	init_img_noalloc(&orig, cf, 320, 240, 1);
	//init_img_noalloc(&orig, cf, 19, 19, 1);
	//init_img_noalloc(&scale_img,scaled,sdims[0].w, sdims[0].h,1);
	init_img(&scale_img, sdims[0].w, sdims[0].h, 1);
	//init_img(&scale_img, 19, 19, 1);
	//init_dummy_classifiers();
}

void facedet_run(char * file_name){
	int scale,x,y,n,nx,ny;
	int16_t mct;
	//int32_t weight_sum;
	float weight_sum;
//FIXME remove	
	uint32_t p0;
	uint32_t sum;
	char str[128];
#if 0
	read_ppm(argv[1],&rgb);
	init_img(&gray,rgb.w,rgb.h,1);
	rgb2y(&rgb,&gray);
	write_pgm(argv[2],&gray);
#endif
	//write_ppm(argv[2],&rgb);
	//read_pgm(argv[1],&orig);
	//init_img_noalloc(&scale_img,cf,sdims[0].w, sdims[0].h,1);
#if 0 //test resize
	resize(&orig, &scale_img);
	write_pgm(argv[2],&scale_img);
#endif
	//scale=0;
	init_obj_list(&objlist);
	//for all scales
	for (scale = MAX_SCALES-1; scale >=0; scale--){
		//printf("inside scale for loop\n");
		scale_img.w = sdims[scale].w;
		scale_img.h = sdims[scale].h;
		//printf("scale = %d w=%d h=%d\n",scale,	scale_img.w, scale_img.h);
		if (scale_img.w != orig.w && scale_img.h!=orig.h)
			resize(&orig, &scale_img);
		else {
			//printf("no resize\n");

			scale_img.pix = orig.pix;
		}
		//sprintf(str,"resized/%d_%d_%d.pgm",scale,scale_img.w, scale_img.h);
		//write_pgm(str,&scale_img);
		//printf("scale dimensions = %d %d\n",scale_img.w, scale_img.h);
		for (y=1; y<=scale_img.h-WINY-1;y++) {
		//printf("inside y for loop y=%d\n",y);
			for (x=1;x<=scale_img.w-WINX-1;x++) {
		//printf("X = %d\n",x);
		//printf("pix = %d\n",scale_img.pix[y*scale_img.w + x]);

			//first stage	
				//x=y=0;
				weight_sum  = 0;
		//		printf("first stage x=%d y=%d\n",x,y);
#ifdef VALUE
				for (n=0;n<STAGE1_CLASSIFIER_NUM;n++) {
					nx = classifier_pos_stage1[n].x;
					ny = classifier_pos_stage1[n].y;
		//			printf("%d %d\n ",nx,ny); 
					//compute mct for (nx,ny)
					mct = calc_mct(&scale_img,x,y,nx,ny);
		//			printf("mct=%d\n",mct);
					//p0=scale_img.pix[((y+ny)*scale_img.w) + (x + nx)];
					weight_sum += classifier_pos_stage1[n].mct_table[mct];
		//			printf("weight_sum = %f\n",weight_sum);
				}
				
#else
				for (ny=0;ny<WINY;ny++) {
					for (nx=0;nx<WINX;nx++) {
						mct = calc_mct(&scale_img,x,y,nx,ny);
						weight_sum += classifier_pos_stage1[mct][ny][nx];
						printf("%d ",mct);
					}
					printf("\n");
				}
#endif
		//		printf("sum S1 %f \n",weight_sum);
				//second stage
				if (weight_sum < STAGE1_THRESHOLD) {
		//		printf("second stage\n");
					weight_sum  = 0;
#ifdef VALUE
					for (n=0;n<STAGE2_CLASSIFIER_NUM;n++) {
						nx = classifier_pos_stage2[n].x;
						ny = classifier_pos_stage2[n].y;
						//compute mct for (nx,ny)
						mct = calc_mct(&scale_img,x,y,nx,ny);
						weight_sum += classifier_pos_stage2[n].mct_table[mct];
					}
#else
					for (ny=0;ny<WINY;ny++) {
						for (nx=0;nx<WINX;nx++) {
							mct = calc_mct(&scale_img,x,y,nx,ny);
							weight_sum += classifier_pos_stage2[mct][ny][nx];
						}
					}
#endif
					
					//printf("sum S2 %f \n",weight_sum);
					//third stage
					if (weight_sum < STAGE2_THRESHOLD) {
		//		printf("3 stage\n");
						weight_sum  = 0;
#ifdef VALUE
						for (n=0;n<STAGE3_CLASSIFIER_NUM;n++) {
							nx = classifier_pos_stage3[n].x;
							ny = classifier_pos_stage3[n].y;
							//compute mct for (nx,ny)
							mct = calc_mct(&scale_img,x,y,nx,ny);
							weight_sum += classifier_pos_stage3[n].mct_table[mct];
						}
#else 
						for (ny=0;ny<WINY;ny++) {
							for (nx=0;nx<WINX;nx++) {
								mct = calc_mct(&scale_img,x,y,nx,ny);
								weight_sum += classifier_pos_stage3[mct][ny][nx];
							}
						}
#endif
						
						//printf("sum S3 %f \n",weight_sum);
						if (weight_sum < STAGE3_THRESHOLD) {
		//					printf("stage 4\n");
							weight_sum  = 0;
							for (ny=0;ny<WINY;ny++) {
								for (nx=0;nx<WINX;nx++) {
									mct = calc_mct(&scale_img,x,y,nx,ny);
									weight_sum += classifier_pos_stage4[mct][ny][nx];
								}
							}
							//printf("sum S4 %f \n",weight_sum);
							if (weight_sum < STAGE4_THRESHOLD) {
							//found face
								printf("facefound\n");
								add_to_obj_list(&objlist,x,y,scale);
								//dump_image(&scale_img,x,y);
								//printf("index =%d, scale = %d, x=%d y=%d\n",objlist.index-1,scale,x,y);
							}
						}

					}
				}
			
			}//x
		} //y
	} //scales

	draw_bounding_box(&objlist,&orig,objlist.index,file_name);
	init_obj_list(&objlist);
		

}

void facedet_run(void){
	int scale,x,y,n,nx,ny;
	int16_t mct;
	//int32_t weight_sum;
	float weight_sum;
//FIXME remove
	uint32_t p0;
	uint32_t sum;
	char str[128];
#if 0
	read_ppm(argv[1],&rgb);
	init_img(&gray,rgb.w,rgb.h,1);
	rgb2y(&rgb,&gray);
	write_pgm(argv[2],&gray);
#endif
	//write_ppm(argv[2],&rgb);
	//read_pgm(argv[1],&orig);
	//init_img_noalloc(&scale_img,cf,sdims[0].w, sdims[0].h,1);
#if 0 //test resize
	resize(&orig, &scale_img);
	write_pgm(argv[2],&scale_img);
#endif
	//scale=0;
	init_obj_list(&objlist);
	//for all scales
	for (scale = MAX_SCALES-1; scale >=0; scale--){
		//printf("inside scale for loop\n");
		scale_img.w = sdims[scale].w;
		scale_img.h = sdims[scale].h;
		//printf("scale = %d w=%d h=%d\n",scale,	scale_img.w, scale_img.h);
		if (scale_img.w != orig.w && scale_img.h!=orig.h)
			resize(&orig, &scale_img);
		else {
			//printf("no resize\n");

			scale_img.pix = orig.pix;
		}
		//sprintf(str,"resized/%d_%d_%d.pgm",scale,scale_img.w, scale_img.h);
		//write_pgm(str,&scale_img);
		//printf("scale dimensions = %d %d\n",scale_img.w, scale_img.h);
		for (y=1; y<=scale_img.h-WINY-1;y++) {
		//printf("inside y for loop y=%d\n",y);
			for (x=1;x<=scale_img.w-WINX-1;x++) {
		//printf("X = %d\n",x);
		//printf("pix = %d\n",scale_img.pix[y*scale_img.w + x]);

			//first stage
				//x=y=0;
				weight_sum  = 0;
		//		printf("first stage x=%d y=%d\n",x,y);
#ifdef VALUE
				for (n=0;n<STAGE1_CLASSIFIER_NUM;n++) {
					nx = classifier_pos_stage1[n].x;
					ny = classifier_pos_stage1[n].y;
		//			printf("%d %d\n ",nx,ny);
					//compute mct for (nx,ny)
					mct = calc_mct(&scale_img,x,y,nx,ny);
		//			printf("mct=%d\n",mct);
					//p0=scale_img.pix[((y+ny)*scale_img.w) + (x + nx)];
					weight_sum += classifier_pos_stage1[n].mct_table[mct];
		//			printf("weight_sum = %f\n",weight_sum);
				}

#else
				for (ny=0;ny<WINY;ny++) {
					for (nx=0;nx<WINX;nx++) {
						mct = calc_mct(&scale_img,x,y,nx,ny);
						weight_sum += classifier_pos_stage1[mct][ny][nx];
						printf("%d ",mct);
					}
					printf("\n");
				}
#endif
		//		printf("sum S1 %f \n",weight_sum);
				//second stage
				if (weight_sum < STAGE1_THRESHOLD) {
		//		printf("second stage\n");
					weight_sum  = 0;
#ifdef VALUE
					for (n=0;n<STAGE2_CLASSIFIER_NUM;n++) {
						nx = classifier_pos_stage2[n].x;
						ny = classifier_pos_stage2[n].y;
						//compute mct for (nx,ny)
						mct = calc_mct(&scale_img,x,y,nx,ny);
						weight_sum += classifier_pos_stage2[n].mct_table[mct];
					}
#else
					for (ny=0;ny<WINY;ny++) {
						for (nx=0;nx<WINX;nx++) {
							mct = calc_mct(&scale_img,x,y,nx,ny);
							weight_sum += classifier_pos_stage2[mct][ny][nx];
						}
					}
#endif

					//printf("sum S2 %f \n",weight_sum);
					//third stage
					if (weight_sum < STAGE2_THRESHOLD) {
		//		printf("3 stage\n");
						weight_sum  = 0;
#ifdef VALUE
						for (n=0;n<STAGE3_CLASSIFIER_NUM;n++) {
							nx = classifier_pos_stage3[n].x;
							ny = classifier_pos_stage3[n].y;
							//compute mct for (nx,ny)
							mct = calc_mct(&scale_img,x,y,nx,ny);
							weight_sum += classifier_pos_stage3[n].mct_table[mct];
						}
#else
						for (ny=0;ny<WINY;ny++) {
							for (nx=0;nx<WINX;nx++) {
								mct = calc_mct(&scale_img,x,y,nx,ny);
								weight_sum += classifier_pos_stage3[mct][ny][nx];
							}
						}
#endif

						//printf("sum S3 %f \n",weight_sum);
						if (weight_sum < STAGE3_THRESHOLD) {
		//					printf("stage 4\n");
							weight_sum  = 0;
							for (ny=0;ny<WINY;ny++) {
								for (nx=0;nx<WINX;nx++) {
									mct = calc_mct(&scale_img,x,y,nx,ny);
									weight_sum += classifier_pos_stage4[mct][ny][nx];
								}
							}
							//printf("sum S4 %f \n",weight_sum);
							if (weight_sum < STAGE4_THRESHOLD) {
							//found face
								printf("facefound\n");
								add_to_obj_list(&objlist,x,y,scale);
								//dump_image(&scale_img,x,y);
								//printf("index =%d, scale = %d, x=%d y=%d\n",objlist.index-1,scale,x,y);
							}
						}

					}
				}

			}//x
		} //y
	} //scales

	draw_bounding_box(&objlist,&orig,objlist.index);
}
