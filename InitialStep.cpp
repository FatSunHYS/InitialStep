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

#if 0
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
	CvSeq* lines;

	lines = cvHoughLines2( testimg, storage, CV_HOUGH_PROBABILISTIC, 1,
	CV_PI / 180, 50, 50, 10 );

#if 1
	cout << "total lines: " << lines->total << endl;
#endif

#if 1
	//Draw the Lines.
	CvPoint* linepoint;
	IplImage * showimg = cvLoadImage( TEST_PHOTO_DIR );
	linepoint = ( CvPoint* )cvGetSeqElem( lines, 0 );
	for( int i = 0; i < lines->total; i += 2 )
	{
		//cvLine( showimg, linepoint[ i ], linepoint[ i + 1 ], CV_RGB( 0, 255, 0 ), 3, CV_AA, 0 );
		cvCircle( showimg, linepoint[ 1 ], 3, CV_RGB( 0, 255, 0 ), 3 );
		cout << linepoint[ 1 ].x << "\t" << linepoint[ 1 ].y << endl;
		//cvCircle( showimg, linepoint[ 1 ], 3, CV_RGB( 0, 255, 0 ), 3 );
	}

	cvShowImage( "test1", showimg );
	cvWaitKey( 0 );
	cvDestroyWindow( "test1" );
#endif

	//Draw the Histogram.
	int Histogram_bins[ 180 ];	// index 0 represent -90 degree, index 90 represent 0 degree, index 179 represent 89 degree

	for( int i = 0; i < 180; ++i )
	{
		Histogram_bins[ i ] = 0;
	}

	CvPoint *endpoint = ( CvPoint* )cvGetSeqElem( lines, 0 );
	int EndPoint_Degree;
	double Slope;
	for( int i = 0; i < lines->total; i += 2 )
	{
		if( endpoint[ i ].x == endpoint[ i + 1 ].x )
		{
			++Histogram_bins[ 0 ];
			continue;
		}

#if 1
 		cout << "ep[" << i << "].x=" << endpoint[ i ].x << endl;
		cout << "ep[" << i << "].y=" << endpoint[ i ].y << endl;
		cout << "ep[" << i + 1 << "].x=" << endpoint[ i + 1 ].x << endl;
		cout << "ep[" << i + 1 << "].y=" << endpoint[ i + 1 ].y << endl;
#endif

		Slope = ( double )( endpoint[ i + 1 ].y - endpoint[ i ].y ) / ( endpoint[ i + 1 ].x - endpoint[ i ].x );
		cout << atan( Slope ) << endl;
		EndPoint_Degree = ( int )( atan( Slope ) * 180 / CV_PI );
		++Histogram_bins[ EndPoint_Degree + 90 ];
	}

#if 1
	for( int i = 0; i < 180; ++i )
	{
		if( Histogram_bins[ i ] )
		{
			cout << "Degree " << i - 90 << " = " << Histogram_bins[ i ] << endl;
		}
	}
#endif

}
