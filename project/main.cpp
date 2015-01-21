//
//  main.cpp
//  Cell Tracking for Tyler Nelson at OSU
//
//  Using OpenCV to track cell movement from video
//
//  Created by Jonathan Korty on 1/12/15.
//  Copyright (c) 2015 Jonathan Korty. All rights reserved.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

RNG rng(12345);

int main(int argc, const char * argv[]) {
    
    cv::VideoCapture cap;
    if(argc > 1) cap.open(string(argv[1]));
    else cap.open(0);

    cv::Mat frame_img[1000];
    cv::Mat frame;
    cap >> frame;
    if(frame.empty()) return -1;
    
    cv::namedWindow("Video");
    int count = 1;
    
    int noise_frame = 9;
    
    while(true){
        cap >> frame;
        if(frame.empty()) break;
        
        // Skip the to frame 9, due to excessive noise
        // Obviously, this is a case by case value
        
        if( count > noise_frame ){
            cv::Mat cont, threshold;
            cont = frame.clone();
            cv::cvtColor(cont, cont, cv::COLOR_BGR2GRAY);
            
            cv::vector<cv::Vec4i> hierarchy;
            cv::vector<cv::vector<cv::Point> > contours;
            
            cv::threshold(cont, cont, 90, 255, cv::THRESH_BINARY);
            threshold = cont.clone();
            
            cv::findContours(cont, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0,0));
            
            // Get the moments
            vector<Moments> mu(contours.size() );
            for( int i = 0; i < contours.size(); i++ )
            {
                mu[i] = moments( contours[i], false );
            }
            
            // Output frame and number of contours
            std::cout << "FRAME: " << count << "\tCOUNT: " << contours.size() << std::endl;
            
            //  Get the mass centers:
            vector<Point2f> mc( contours.size() );
            std::cout << "CENTROID: \n";
            for( int i = 0; i < contours.size(); i++ )
            {

/* TODO ===============================================================================
 
                // Check for nearest previous location of a center
                // But only if it is not the first iteration
                if( count > noise_frame+1 ){
                    
                }
 
===================================================================================== */
                
                mc[i] = Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 );
                std::cout << mc[i] << "\n";
            }
            
            Mat drawing = Mat::zeros( cont.size(), CV_8UC3 );
            for(size_t i=0; i<contours.size(); i++){
                cv::drawContours(frame, contours, (int)i, cv::Scalar(0,0,255), 1, 8, hierarchy, 0, Point());
                circle( frame, mc[i], 1, cv::Scalar(100,100,255), -1, 8, 0 );
                int contourSize = (int)contours.size();
                cv::putText( frame, std::to_string(contourSize), Point2d(5,30), FONT_HERSHEY_PLAIN, 2, Scalar(255,255,255), 1, 8, false);
                
            }
            frame_img[count] = frame;
            
            // Output frame_x.png
            std::ostringstream name;
            name << "/Users/jkorty/Desktop/output/frame_" << count << ".png";
            imwrite( name.str(), frame );
            
            // Show video
            cv::imshow("Video", frame);
            for(;;)
            {
                if(waitKey(30) >= 0) break;
            }
        }
        count++;
        if(cv::waitKey(10)==27 ) {
            break;
        }
    }
    for(;;)
    {
        if(waitKey(30) >= 0) break;
    }
    
    return 0;
}

