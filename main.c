#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include <string.h>
#include <dirent.h>

//#include "pic_data.h"
#include "facedet.h"
//#include "classifier.h"
//img orig;
//img scale_img;


#include <string.h>
#include <assert.h>

#include <ctype.h>

#include <getopt.h>             /* getopt_long() */

#include<opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>


int main(int argc, char* argv[]){

    if(argc>2){
            printf("usage: %s filename \n",argv[0]);
            return -1;
    }
    else
    	if(argc==2){
    		img input;
    		img scale_input;
    		int x,y;
    		uint8_t p0;


    		read_pgm(argv[1],&orig);
    		facedet_init();
    		facedet_run(argv[1]);
    		return 0;
    	}
    	else{

    	    cvNamedWindow("Window", CV_WINDOW_AUTOSIZE);
    	    CvCapture* capture =cvCreateCameraCapture(-1);
    	    if (!capture){
    	        printf("Error. Cannot capture.");
    	    }
    	    else{
    	    	IplImage* frame = cvQueryFrame(capture);
    	        cvNamedWindow("Window", CV_WINDOW_AUTOSIZE);
    	        IplImage* gray = cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
    	        IplImage* new_img = cvCreateImage(cvSize(320,240), IPL_DEPTH_8U, 1);
    	        IplImage* img = cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
    	        while (1){
    	            frame = cvQueryFrame(capture);
    	            if(!frame){
    	                printf("Error. Cannot get the frame.");
    	                //break;
    	            }
    	            gray = cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);

    	            cvCvtColor(frame, gray, CV_RGB2GRAY);

    	            cvResize(gray,new_img);

    	            orig.h = new_img->height;
    	            orig.w = new_img->depth;
    	            orig.stride = 1;
    	            orig.pix = malloc(new_img->height * new_img->depth * 1);
    	            orig.pix = new_img->imageData;

    	            //img = cvCreateImage(cvSize(320,240),IPL_DEPTH_8U,1);

    	            facedet_init();
    	            facedet_run();

    	            //img->imageData = orig.pix;

    	        cvShowImage("Window",new_img);
    	        cvWaitKey(30);
    	        }
    	        cvReleaseCapture(&capture);
    	        cvDestroyWindow("Window");

    	    }
    	}


    return 0;
}

/*int main(int argc, char ** argv){
	img input;
	img scale_input;
	//int x,y;
	//uint8_t p0;
	
	
	//read_pgm(argv[1],&orig);
	//facedet_init();
	//facedet_run(argv[1]);
	return 0;
}*/
