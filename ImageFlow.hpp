//
//  ImageFlow.hpp
//  ImageFlow
//
//  Created by Zhalgas Baibatyr on 7/1/15.
//  Copyright © 2016 Zhalgas Baibatyr. All rights reserved.
//

#ifndef ImageFlow_hpp
#define ImageFlow_hpp

#include <opencv2/highgui/highgui.hpp>
#include <dirent.h>
#include <sys/stat.h>

/*
    Class for video capturing from video files or image sequences.
    It contains functionality for traversing files.
*/
class ImageFlow
{
public:

    /*
        ImageFlow works with image/video files.
    */
    enum file_type { IMAGE, VIDEO };

    /*
        Default constructor.
    */
    ImageFlow();

    /*
        Constructor accepts directory path and appoints (optional) file index to begin with.
    */
    ImageFlow(const std::string &path, const int &fileIndex = 0);

    /*
        Brackets operator assigns a new directory path and appoints (optional) file index to begin with.
    */
    void operator() (const std::string &path, const int &fileIndex = 0);

    /*
        Function sets current file index.
    */
    void setFileIndex(const int &fileIndex);

    /*
        Function sets starting time position in the video.
    */
    void setTime(const std::string &strTime);

    /*
        Function acquires image or video frame.
    */
    bool getImage(cv::Mat &image);

    /*
        Analog of "getImage()" function.
    */
    inline bool operator>> (cv::Mat &image)
    {
        return getImage(image);
    }

    /*
        "getFileData()" function passes type and name of the currently opened file.
        File type parameter is "ImageFlow::file_type".
    */
    inline void getFileInfo(file_type &fileType, std::string &fileName)
    {
        fileType = files[fileIndex].type;
        fileName = files[fileIndex].name;

        return;
    }

    /*
        "getKeyCode()" function returns ASCII code value of the pressed keyboard key.
    */
    inline int getKeyCode() const
    {
        return keyCode;
    }

private:
    struct File
    {
        std::string name;
        file_type type;
    };

    bool pathIsSet;
    int keyCode;
    int fileIndex;
    std::string dirPath;
    std::vector<File> files;
    cv::VideoCapture capture;

    void intro() const;
    void listDir(const std::string &path);
    void strTimeToMsec(const std::string &strTime, double &msec);
};


/*
    Exception class for "ImageFlow".
*/
class ImfExc : private std::exception
{
    friend ImageFlow;

public:

    /*
        Exception code types.
    */
    enum
    {
        PATH_ERR1 = 101, PATH_ERR2 = 102,
        EXT_ERR1  = 201, EXT_ERR2  = 202,
        INDX_ERR1 = 301, INDX_ERR2 = 302,
        CAPT_ERR  = 400,
        EXIT      = 500,
        END       = 600,
        TIME_ERR1 = 701, TIME_ERR2 = 702
    };

    /*
        Exception code.
    */
    const int code;

    /*
        Exception message.
    */
    virtual const char* what() const _NOEXCEPT;

private:
    std::string message;

    explicit ImfExc(const int &code);
    explicit ImfExc(const int &code, const std::string &extraInfo);
};

#endif /* ImageFlow_hpp */
