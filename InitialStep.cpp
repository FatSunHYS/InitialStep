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
void DrawLine( IplImage **image, int IsSlopeValid, double kslope, int FirstClusterOffset );

/*=====================================
 Implementation of functions
 =====================================*/

int main()
{
	//Load the image.
	IplImage * testimg = cvLoadImage( TEST_PHOTO_DIR, CV_LOAD_IMAGE_GRAYSCALE );
	IplImage * showimg = NULL;
	int counter;

	int TemporaryDegree;
	double TemporarySlope;
	int TemporaryData;
	int centerpoint_x;
	int centerpoint_y;

	int FirstSlope_Index = 0;
	int SecondSlope_Index = 0;
	double FirstSlope;
	double SecondSlope;
	int FirstOffset = 0;
	int SecondOffset = 0;
	int *FirstClusterOffset = NULL;
	int *SecondClusterOffset = NULL;
	int *FirstOffsetPointer = NULL;
	int *SecondOffsetPointer = NULL;
	CvPoint CrossCenter;

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
	cvReleaseImage( &testimg );
#endif

	cvThreshold( testimg, testimg, 200, 255, CV_THRESH_BINARY );

#if 0
	cvNamedWindow( "test1" );
	cvShowImage( "test1", testimg );
	cvWaitKey( 0 );
	cvDestroyWindow( "test1" );
	cvReleaseImage( &testimg );
#endif

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

#if 0
	//Draw the Lines.
	CvPoint* linepoint;
	showimg = cvLoadImage( TEST_PHOTO_DIR );
	linepoint = ( CvPoint* )cvGetSeqElem( lines, 0 );
	for( counter = 0; counter < lines->total; counter += 2 )
	{
		cvLine( showimg, linepoint[ counter ], linepoint[ counter + 1 ], CV_RGB( 0, 255, 0 ), 3, CV_AA, 0 );
		//cvCircle( showimg, linepoint[ 1 ], 3, CV_RGB( 0, 255, 0 ), 3 );
		//cout << linepoint[ 1 ].x << "\t" << linepoint[ 1 ].y << endl;
		//cvCircle( showimg, linepoint[ 1 ], 3, CV_RGB( 0, 255, 0 ), 3 );
	}

	cvShowImage( "test1", showimg );
	cvWaitKey( 0 );
	cvDestroyWindow( "test1" );
	cvReleaseImage( &showimg );
#endif

	//Draw the Histogram.
	int Histogram_bins[ 180 ];	// index 0 represent -90 degree, index 90 represent 0 degree, index 179 represent 89 degree

	for( counter = 0; counter < 180; ++counter )
	{
		Histogram_bins[ counter ] = 0;
	}

	CvPoint *endpoint = ( CvPoint* )cvGetSeqElem( lines, 0 );

	for( counter = 0; counter < lines->total; counter += 2 )
	{
		if( endpoint[ counter ].x == endpoint[ counter + 1 ].x )
		{
			++Histogram_bins[ 0 ];
			continue;
		}

		TemporarySlope = ( double )( endpoint[ counter + 1 ].y - endpoint[ counter ].y ) / ( endpoint[ counter + 1 ].x - endpoint[ counter ].x );
		TemporaryDegree = ( int )( atan( TemporarySlope ) * 180 / CV_PI );
		++Histogram_bins[ TemporaryDegree + 90 ];
	}

#if 1
	for( counter = 0; counter < 180; ++counter )
	{
		if( Histogram_bins[ counter ] )
		{
			cout << "Degree " << counter - 90 << " = " << Histogram_bins[ counter ] << endl;
		}
	}
	cout << endl;
#endif

	//select the first slope.
	FirstSlope_Index = 0;
	for( counter = 1; counter < 180; ++counter )
	{
		if( Histogram_bins[ counter ] > Histogram_bins[ FirstSlope_Index ] )
		{
			FirstSlope_Index = counter;
		}
	}

	if( Histogram_bins[ FirstSlope_Index ] == 0 )
	{
		cout << "There is no line here!" << endl;
		return 0;
	}

	cout << "First Slope Index = " << FirstSlope_Index << endl;
	cout << "First Slope Amount = " << Histogram_bins[ FirstSlope_Index ] << endl;
	cout << "First Degree = " << FirstSlope_Index - 90 << endl << endl;

	//select the second slope.
	SecondSlope_Index = ( FirstSlope_Index + 85 ) % 180;
	for( counter = 1; counter < 10; ++counter )
	{
		TemporaryData = ( FirstSlope_Index + 85 + counter ) % 180;
		if( Histogram_bins[ TemporaryData ] > Histogram_bins[ SecondSlope_Index ] )
		{
			SecondSlope_Index = TemporaryData;
		}
	}

	if( Histogram_bins[ SecondSlope_Index ] == 0 )
	{
		cout << "There is only one line here!" << endl;
		return 0;
	}

	cout << "Second Slope Index = " << SecondSlope_Index << endl;
	cout << "Second Slope Amount = " << Histogram_bins[ SecondSlope_Index ] << endl;
	cout << "Second Degree = " << SecondSlope_Index - 90 << endl << endl;

	//Cluster the offset.
	FirstClusterOffset = new int[ Histogram_bins[ FirstSlope_Index ] ];
	FirstOffsetPointer = FirstClusterOffset;
	SecondClusterOffset = new int[ Histogram_bins[ SecondSlope_Index ] ];
	SecondOffsetPointer = SecondClusterOffset;

	if( FirstSlope_Index )
	{
		FirstSlope = tan( ( double )( FirstSlope_Index - 90 ) / 180 * CV_PI );
	}
	if( SecondSlope_Index )
	{
		SecondSlope = tan( ( double )( SecondSlope_Index - 90 ) / 180 * CV_PI );
	}
	for( counter = 0; counter < lines->total; counter += 2 )
	{
		if( FirstSlope_Index == 0 )
		{
			//Slope does not exist.
			if( endpoint[ counter ].x == endpoint[ counter + 1 ].x )
			{
				*FirstOffsetPointer = endpoint[ counter ].x;
				++FirstOffsetPointer;
			}
			continue;
		}

		if( SecondSlope_Index == 0 )
		{
			//Slope does not exist.
			if( endpoint[ counter ].x == endpoint[ counter + 1 ].x )
			{
				*SecondOffsetPointer = endpoint[ counter ].x;
				++SecondOffsetPointer;
			}
			continue;
		}

		//Slope does exist;
		TemporarySlope = ( double )( endpoint[ counter + 1 ].y - endpoint[ counter ].y ) / ( endpoint[ counter + 1 ].x - endpoint[ counter ].x );
		TemporaryDegree = ( int )( atan( TemporarySlope ) * 180 / CV_PI );

		if( TemporaryDegree == FirstSlope_Index - 90 )
		{
			centerpoint_x = ( endpoint[ counter ].x + endpoint[ counter + 1 ].x ) / 2;
			centerpoint_y = ( endpoint[ counter ].y + endpoint[ counter + 1 ].y ) / 2;

			*FirstOffsetPointer = ( int )( centerpoint_y - FirstSlope * centerpoint_x );
			++FirstOffsetPointer;
			continue;
		}

		if( TemporaryDegree == SecondSlope_Index - 90 )
		{
			centerpoint_x = ( endpoint[ counter ].x + endpoint[ counter + 1 ].x ) / 2;
			centerpoint_y = ( endpoint[ counter ].y + endpoint[ counter + 1 ].y ) / 2;

			*SecondOffsetPointer = ( int )( centerpoint_y - SecondSlope * centerpoint_x );
			++SecondOffsetPointer;
			continue;
		}

	}

#if 1
	//printout the first cluster offset.
	for( counter = 0; counter < Histogram_bins[ FirstSlope_Index ]; ++counter )
	{
		cout << "FirstClusterOffset[" << counter << "] = " << FirstClusterOffset[ counter ] << endl;
	}
	cout << endl;

	//printout the second cluster offset.
	for( counter = 0; counter < Histogram_bins[ SecondSlope_Index ]; ++counter )
	{
		cout << "SecondClusterOffset[" << counter << "] = " << SecondClusterOffset[ counter ] << endl;
	}
	cout << endl;

#endif

	//temporary set the FirstClusterOffset to the average value.
	FirstOffset = 0;
	for( counter = 0; counter < Histogram_bins[ FirstSlope_Index ]; ++counter )
	{
		FirstOffset += FirstClusterOffset[ counter ];
	}
	FirstOffset /= Histogram_bins[ FirstSlope_Index ];

	//temporary set the SecondClusterOffset to the average value.
	SecondOffset = 0;
	for( counter = 0; counter < Histogram_bins[ SecondSlope_Index ]; ++counter )
	{
		SecondOffset += SecondClusterOffset[ counter ];
	}
	SecondOffset /= Histogram_bins[ SecondSlope_Index ];

#if 1
	cout << "FirstOffset = " << FirstOffset << endl;
	cout << "SecondOffset = " << SecondOffset << endl << endl;
#endif

#if 1
	//Print Out the formular.
	if( FirstSlope_Index )
	{
		cout << "Formular 1 : " << FirstSlope << " * x - y + " << FirstOffset << " = 0 " << endl << endl;
	}
	else
	{
		cout << "Formular 1 : x = " << -FirstOffset << endl << endl;
	}

	if( SecondSlope_Index )
	{
		cout << "Formular 2 : " << SecondSlope << " * x - y + " << SecondOffset << " = 0 " << endl << endl;
	}
	else
	{
		cout << "Formular 2 : x = " << -SecondOffset << endl << endl;
	}
#endif

	//calculate the cross center point.
	if( FirstSlope_Index )
	{
		if( SecondSlope_Index )
		{
			CrossCenter.x = ( SecondOffset - FirstOffset ) / ( FirstSlope - SecondSlope );
			CrossCenter.y = ( FirstSlope * SecondOffset - SecondSlope * FirstOffset ) / ( FirstSlope - SecondSlope );
		}
		else
		{
			//the second slope does not exist.
			CrossCenter.x = SecondOffset;
			CrossCenter.y = FirstSlope * CrossCenter.x + FirstOffset;
		}
	}
	else
	{
		//the first slope does not exist.
		CrossCenter.x = FirstOffset;
		CrossCenter.y = SecondSlope * CrossCenter.x + SecondOffset;
	}

#if 1
	//print out the cross center point.
	cout << "CrossCenterPoint = ( " << CrossCenter.x << ", " << CrossCenter.y << " )." << endl << endl;
#endif

#if 1
	//Draw the Lines.

	showimg = cvLoadImage( TEST_PHOTO_DIR );

	DrawLine( &showimg, FirstSlope_Index, FirstSlope, FirstOffset );
	DrawLine( &showimg, SecondSlope_Index, SecondSlope, SecondOffset );
	cvCircle( showimg, CrossCenter, 1, CV_RGB( 255, 0, 0 ) );

	cvShowImage( "test1", showimg );
	cvWaitKey( 0 );
	cvDestroyWindow( "test1" );
	cvReleaseImage( &showimg );
#endif

	delete FirstClusterOffset;
	FirstClusterOffset = NULL;

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

