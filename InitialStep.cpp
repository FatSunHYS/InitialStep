/**
 ******************************************************************************
 * @project	
 *		InitialStep
 * @author  			
 *		
 * @creationdate   
 *		
 * @lastmodifydate
 *		Apr 4, 2016
 * @verify
 *		
 * @verifydate
 * 
 * @company
 *		
 ******************************************************************************
 * @brief   
 *		
 * @attention
 *		
 ******************************************************************************
 */

/*=====================================
 Include headers
 =====================================*/
#include "InitialStep.h"

/*=====================================
 Variables definition
 =====================================*/

/*=====================================
 Functions definition
 =====================================*/

/*=====================================
 Implementation of functions
 =====================================*/

int main()
{
	//Load the image.
	IplImage * testimg = cvLoadImage( TEST_PHOTO_DIR, CV_LOAD_IMAGE_GRAYSCALE );

#if 1
	//show some messages of the test photo.
	cout << "nChannels = " << testimg->nChannels << endl;
	cout << "width = " << testimg->width << endl;
	cout << "height = " << testimg->height << endl;
	cout << "widthStep = " << testimg->widthStep << endl;
	cout << "depth = " << testimg->depth << endl;
#endif

#if LOCAL_DEBUG
	cvNamedWindow( "test1" );
	cvShowImage( "test1", testimg );
	cvWaitKey( 0 );
	cvDestroyWindow( "test1" );
#endif

	cvThreshold( testimg, testimg, 200, 255, CV_THRESH_BINARY );

	//Find the lines.
	CvMemStorage *storage = cvCreateMemStorage( 0 );
	cvClearMemStorage( storage );
	cvCanny( testimg, testimg, 50, 100 );
	CvPoint* line;
	CvSeq* lines;
	lines = cvHoughLines2( testimg, storage, CV_HOUGH_PROBABILISTIC, 1,
	CV_PI / 180, 50, 50, 10 );
	testimg = cvLoadImage( TEST_PHOTO_DIR );

#if 1
	cout << "total lines: " << lines->total << endl;
#endif

#if LOCAL_DEBUG
	//Draw the Lines.
	line = ( CvPoint* )cvGetSeqElem( lines, 0 );
	for( int i = 0; i < lines->total; i += 2 )
	{
		cvLine( testimg, line[ i ], line[ i + 1 ], CV_RGB( 0, 0, 0 ), 3, CV_AA, 0 );
	}

	cvShowImage( "test1", testimg );
	cvWaitKey( 0 );
	cvDestroyWindow( "test1" );
#endif



}
