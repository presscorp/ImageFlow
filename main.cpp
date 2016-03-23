//
//  main.cpp
//  ImageFlow
//
//  Created by Zhalgas Baibatyr on 7/1/15.
//  Copyright © 2016 Zhalgas Baibatyr. All rights reserved.
//

#include "ImageFlow.hpp"

int main()
{
    try
    {
        ImageFlow imflow("directory_path", 0);

        /*
            Do not place "cv::Mat" declaration inside the loop.
            "ImageFlow" may not read frame if you paused a video or trying
            to read image files until you press proper control key.
        */
        cv::Mat image;

        while (true)
        {
            imflow >> image;
            cv::imshow("ImageFlow", image);
        }
    }
    catch (const ImfExc &imfExc)
    {
        printf("%s\n", imfExc.what());
    }

    return EXIT_SUCCESS;
}
