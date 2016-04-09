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
void DrawLine( IplImage **image, int IsSlopeValid, double kslope, int offset );

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
	cout << "depth = " << testimg->depth << endl << endl;
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
	cout << "total lines: " << lines->total << endl << endl;
#endif

#if 1
	//Draw the Lines.
	CvPoint* linepoint;
	IplImage * showimg = cvLoadImage( TEST_PHOTO_DIR );
	linepoint = ( CvPoint* )cvGetSeqElem( lines, 0 );
	for( int i = 0; i < lines->total; i += 2 )
	{
		cvLine( showimg, linepoint[ i ], linepoint[ i + 1 ], CV_RGB( 0, 255, 0 ), 3, CV_AA, 0 );
		//cvCircle( showimg, linepoint[ 1 ], 3, CV_RGB( 0, 255, 0 ), 3 );
		//cout << linepoint[ 1 ].x << "\t" << linepoint[ 1 ].y << endl;
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

		Slope = ( double )( endpoint[ i + 1 ].y - endpoint[ i ].y ) / ( endpoint[ i + 1 ].x - endpoint[ i ].x );
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
	cout << endl;
#endif

	//select the maximum number of the slope.
	int MaxSlope_Index = 0;
	for( int i = 1; i < 180; ++i )
	{
		if( Histogram_bins[ i ] > Histogram_bins[ MaxSlope_Index ] )
		{
			MaxSlope_Index = i;
		}
	}

	cout << "MaxSlope Index = " << MaxSlope_Index << endl;
	cout << "Maximum number of the slope = " << Histogram_bins[ MaxSlope_Index ] << endl;
	cout << "Degree = " << MaxSlope_Index - 90 << endl << endl;

	int *offset = new int[ Histogram_bins[ MaxSlope_Index ] ];
	int *offset_pointer = offset;
	int centerpoint_x;
	int centerpoint_y;
	double k_slope;
	if( MaxSlope_Index )
	{
		k_slope = tan( ( double )( MaxSlope_Index - 90 ) / 180 * CV_PI );
	}
	for( int i = 0; i < lines->total; i += 2 )
	{
		if( MaxSlope_Index == 0 )
		{
			//Slope does not exist.
			if( endpoint[ i ].x == endpoint[ i + 1 ].x )
			{
				*offset_pointer = endpoint[ i ].x;
				++offset_pointer;
			}
		}
		else
		{
			//Slope does exist.
			if( endpoint[ i ].x == endpoint[ i + 1 ].x )
			{
				continue;
			}

			Slope = ( double )( endpoint[ i + 1 ].y - endpoint[ i ].y ) / ( endpoint[ i + 1 ].x - endpoint[ i ].x );
			EndPoint_Degree = ( int )( atan( Slope ) * 180 / CV_PI );

			if( EndPoint_Degree == MaxSlope_Index - 90 )
			{
				centerpoint_x = ( endpoint[ i ].x + endpoint[ i + 1 ].x ) / 2;
				centerpoint_y = ( endpoint[ i ].y + endpoint[ i + 1 ].y ) / 2;

				*offset_pointer = ( int )( centerpoint_y - k_slope * centerpoint_x );
				++offset_pointer;
			}
		}
	}

#if 1
	for( int i = 0; i < Histogram_bins[ MaxSlope_Index ]; ++i )
	{
		cout << "offset[" << i << "] = " << offset[ i ] << endl;
	}
	cout << endl;
#endif

	//cluster the offset
	int ClusterOffset = 0;

	//temperatory set the offset is the average value.

	for( int i = 0; i < Histogram_bins[ MaxSlope_Index ]; ++i )
	{
		ClusterOffset += offset[ i ];
	}
	ClusterOffset /= Histogram_bins[ MaxSlope_Index ];

#if 1
	cout << "ClusterOffset = " << ClusterOffset << endl << endl;
#endif

	//Print Out the formular.
	if( MaxSlope_Index )
	{
		cout << "Formular : " << k_slope << " * x - y + " << ClusterOffset << " = 0 " << endl << endl;
	}
	else
	{
		cout << "Formular : x = " << -ClusterOffset << endl << endl;
	}

#if 1
	//Draw the Lines.

	IplImage * showimg2 = cvLoadImage( TEST_PHOTO_DIR );

	DrawLine( &showimg2, MaxSlope_Index, k_slope, ClusterOffset );

	cvShowImage( "test1", showimg2 );
	cvWaitKey( 0 );
	cvDestroyWindow( "test1" );
#endif

	delete offset;
	offset = NULL;

}

void DrawLine( IplImage **image, int IsSlopeValid, double kslope, int offset )
{
	int TemporaryData;
	CvPoint EndPoint[ 4 ];
	int indexcounter = 0;

	if( IsSlopeValid )
	{
		if( ( 0 <= offset ) && ( offset <= ( *image )->height ) )
		{
			EndPoint[ indexcounter ].x = 0;
			EndPoint[ indexcounter ].y = offset;
			++indexcounter;
		}

		TemporaryData = -offset / kslope;
		if( ( 0 < TemporaryData ) && ( TemporaryData <= ( *image )->width ) )
		{
			EndPoint[ indexcounter ].x = TemporaryData;
			EndPoint[ indexcounter ].y = 0;
			++indexcounter;
		}

		TemporaryData = kslope * ( *image )->width + offset;
		if( ( 0 < TemporaryData ) && ( TemporaryData <= ( *image )->height ) )
		{
			EndPoint[ indexcounter ].x = ( *image )->width;
			EndPoint[ indexcounter ].y = TemporaryData;
			++indexcounter;
		}

		TemporaryData = ( ( *image )->height - offset ) / kslope;
		if( ( 0 < TemporaryData ) && ( TemporaryData < ( *image )->width ) )
		{
			EndPoint[ indexcounter ].x = TemporaryData;
			EndPoint[ indexcounter ].y = ( *image )->height;
			++indexcounter;
		}

		if( indexcounter < 2 )
		{
			cout << "No suitable line to draw on the image." << endl << endl;
			return;
		}

	}
	else
	{
		if( ( 0 <= offset ) && ( offset <= ( *image )->height ) )
		{
			EndPoint[ indexcounter ].x = offset;
			EndPoint[ indexcounter ].y = 0;
			++indexcounter;
		}

		if( indexcounter < 1 )
		{
			cout << "No suitable line to draw on the image." << endl << endl;
			return;
		}

		EndPoint[ indexcounter ].x = offset;
		EndPoint[ indexcounter ].y = ( *image )->height;
		++indexcounter;
	}

	//cout << "indexcounter = " << indexcounter << endl << endl;
	cvLine( *image, EndPoint[ 0 ], EndPoint[ 1 ], CV_RGB( 0, 255, 0 ), 1, CV_AA, 0 );

	return;
}

