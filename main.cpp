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
        cv::Mat image;

        while (true)
        {
            if (imflow >> image)
            {
                cv::imshow("ImageFlow", image);
            }
        }
    }
    catch (const ImfExc &imfExc)
    {
        printf("%s\n", imfExc.what());
    }

    return EXIT_SUCCESS;
}
