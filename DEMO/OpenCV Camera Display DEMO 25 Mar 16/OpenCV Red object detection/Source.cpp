
// OpenCV ref: https://www.youtube.com/watch?v=wYl4MX0Wwlk
//http://opencv-srf.blogspot.com/2010/09/object-detection-using-color-seperation.html

#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

//void detect_hand_and_fingers(cv::Mat& src);
//void detect_hand_silhoutte(cv::Mat& src);

int main(int argc, char** argv)
{
	VideoCapture cap(0); //capture the video from web cam

	if (!cap.isOpened())  // if not success, exit program
	{
		cout << "Cannot open the web cam" << endl;
		return -1;
	}

	namedWindow("Control", CV_WINDOW_AUTOSIZE); //create a window called "Control"
		
	int iLowH = 0;
	int iHighH = 179;

	int iLowS = 0;
	int iHighS = 255;

	int iLowV = 0;
	int iHighV = 255;

	//Create trackbars in "Control" window
	cvCreateTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
	cvCreateTrackbar("HighH", "Control", &iHighH, 179);

	cvCreateTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
	cvCreateTrackbar("HighS", "Control", &iHighS, 255);

	cvCreateTrackbar("LowV", "Control", &iLowV, 255); //Value (0 - 255)
	cvCreateTrackbar("HighV", "Control", &iHighV, 255);

	while (true)
	{
		Mat imgOriginal;

		bool bSuccess = cap.read(imgOriginal); // read a new frame from video

		if (!bSuccess) //if not success, break loop
		{
			cout << "Cannot read a frame from video stream" << endl;
			break;
		}

		Mat imgHSV;

		cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

		Mat imgThresholded;

		inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image

																									  //morphological opening (remove small objects from the foreground)
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		//morphological closing (fill small holes in the foreground)
		dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		imshow("Thresholded Image", imgThresholded); //show the thresholded image

		namedWindow("Original", CV_WINDOW_NORMAL);
		//setWindowProperty("Original", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
		setWindowProperty("Original", CV_WND_PROP_AUTOSIZE, CV_WINDOW_FULLSCREEN);
		imshow("Original", imgOriginal);
	
		/* Xray like image

				cv::Mat gray;
				cv::cvtColor(imgOriginal, gray, CV_BGR2GRAY);
				cv::Mat gray_silhouette = gray.clone();

				// Isolate Hand + Fingers 

				detect_hand_and_fingers(gray);
				cv::imshow("Hand+Fingers", gray);
				cv::imwrite("hand_fingers.png", gray);

				// Isolate Hand Sillhoute and subtract it from the other image (Hand+Fingers) 

				detect_hand_silhoutte(gray_silhouette);
				cv::imshow("Hand", gray_silhouette);
				cv::imwrite("hand_silhoutte.png", gray_silhouette);

				// Subtract Hand Silhoutte from Hand+Fingers so we get only Fingers 

				cv::Mat fingers = gray - gray_silhouette;
				cv::imshow("Fingers", fingers);
				cv::imwrite("fingers_only.png", fingers);

		//xary like image ends		*/
		
		
		if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}
	}

	return 0;

}

/*xary like image  functions//

void detect_hand_and_fingers(cv::Mat& src)
{
	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3), cv::Point(1, 1));
	cv::morphologyEx(src, src, cv::MORPH_ELLIPSE, kernel);

	int adaptiveMethod = CV_ADAPTIVE_THRESH_GAUSSIAN_C; // CV_ADAPTIVE_THRESH_MEAN_C, CV_ADAPTIVE_THRESH_GAUSSIAN_C
	cv::adaptiveThreshold(src, src, 255,
		adaptiveMethod, CV_THRESH_BINARY,
		9, -5);

	int dilate_sz = 1;
	cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE,
		cv::Size(2 * dilate_sz, 2 * dilate_sz),
		cv::Point(dilate_sz, dilate_sz));
	cv::dilate(src, src, element);
}

void detect_hand_silhoutte(cv::Mat& src)
{
	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(7, 7), cv::Point(3, 3));
	cv::morphologyEx(src, src, cv::MORPH_ELLIPSE, kernel);

	int adaptiveMethod = CV_ADAPTIVE_THRESH_MEAN_C; // CV_ADAPTIVE_THRESH_MEAN_C, CV_ADAPTIVE_THRESH_GAUSSIAN_C
	cv::adaptiveThreshold(src, src, 255,
		adaptiveMethod, CV_THRESH_BINARY,
		251, 5); // 251, 5

	int erode_sz = 5;
	cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE,
		cv::Size(2 * erode_sz + 1, 2 * erode_sz + 1),
		cv::Point(erode_sz, erode_sz));
	cv::erode(src, src, element);

	int dilate_sz = 1;
	element = cv::getStructuringElement(cv::MORPH_ELLIPSE,
		cv::Size(2 * dilate_sz + 1, 2 * dilate_sz + 1),
		cv::Point(dilate_sz, dilate_sz));
	cv::dilate(src, src, element);

	cv::bitwise_not(src, src);
}

*/