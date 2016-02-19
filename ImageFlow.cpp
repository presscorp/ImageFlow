//
//  ImageFlow.cpp
//  ImageFlow
//
//  Created by Zhalgas Baibatyr on 7/1/15.
//  Copyright © 2016 Zhalgas Baibatyr. All rights reserved.
//

#include "ImageFlow.hpp"

#define SPACE_KEY keyCode == 32
#define ESC_KEY   keyCode == 27

#ifdef _WIN32

    const char SLASH = '\\';
    #define UP_KEY    keyCode == 2490368
/*  #define DOWN_KEY  keyCode == 2621440  */
    #define LEFT_KEY  keyCode == 2424832
    #define RIGHT_KEY keyCode == 2555904

#elif defined __unix__ || defined __APPLE__

    const char SLASH = '/';
    #define UP_KEY    keyCode == 63232
/*  #define DOWN_KEY  keyCode == 63233  */
    #define LEFT_KEY  keyCode == 63234
    #define RIGHT_KEY keyCode == 63235

#endif

ImageFlow::ImageFlow()
:  pathIsSet(false),
   keyCode(0)
{
    intro();
}

/*
    "ImageFlow()" constructor lists the directory and appoints (optional) file index to start with.
    It also prints introductory message.
*/
ImageFlow::ImageFlow(const std::string &path, const int &fileIndex)
:  pathIsSet(false),
   keyCode(0)
{
    intro();
    (*this)(path, fileIndex);
}

ImageFlow::~ImageFlow() {}

/*
    "operator()" function lists the directory and appoints (optional) file index to start with.
*/
void ImageFlow::operator() (const std::string &path, const int &fileIndex)
{
    if (pathIsSet == true)
    {
        keyCode = -1;
        dirPath.clear();
        files.clear();
        capture.release();
    }
    else
    {
        pathIsSet = true;
    }

    listDir(path);
    setFileIndex(fileIndex);

    return;
}

/*
    "setFileIndex()" function appoints specific file index.
*/
void ImageFlow::setFileIndex(const int &fileIndex)
{
    if (pathIsSet == false)
    {
        throw ImfExc(ImfExc::PATH_ERR1);
    }

    if (fileIndex < 0 || fileIndex > files.size() - 1)
    {
        if (files.size() == 1)
        {
            throw ImfExc(ImfExc::INDX_ERR1);
        }
        else
        {
            throw ImfExc(ImfExc::INDX_ERR2, std::to_string(files.size() - 1));
        }
    }

    this->fileIndex = fileIndex;
    capture.open((dirPath + files[fileIndex].name).c_str());
    if (!capture.isOpened())
    {
        throw ImfExc(ImfExc::CAPT_ERR, dirPath + files[fileIndex].name);
    }

    printf("[%i] %s: ", fileIndex, files[fileIndex].name.c_str());

    return;
}

/*
    "setTime()" function sets starting time for video. The string format is "HH:MM:SS".
*/
void ImageFlow::setTime(const std::string &strTime)
{
    if (pathIsSet == false)
    {
        throw ImfExc(ImfExc::PATH_ERR1);
    }

    if (files[fileIndex].type != VIDEO)
    {
        throw ImfExc(ImfExc::TIME_ERR1, dirPath + files[fileIndex].name);
    }

    double msec;
    strTimeToMsec(strTime, msec);
    capture.set(CV_CAP_PROP_POS_MSEC, msec);

    return;
}

/*
    "getImage()" function puts acquired image to the passed parameter.
    It also handles pressed control keys.
*/
void ImageFlow::getImage(cv::Mat &image)
{
    if (pathIsSet == false)
    {
        throw ImfExc(ImfExc::PATH_ERR1);
    }

    if (files[fileIndex].type == VIDEO)
    {
        bool stopStream = false;
        do
        {
            if (stopStream == true)
            {
                keyCode = cv::waitKey(0);
                if (SPACE_KEY)
                {
                    keyCode = -1;
                    break;
                }
            }
            else
            {
                keyCode = cv::waitKey(1);
                if (SPACE_KEY)
                {
                    stopStream = true;
                    continue;
                }
            }

            if (ESC_KEY)
            {
                throw ImfExc(ImfExc::EXIT);
            }
            else if (RIGHT_KEY)
            {
                if (fileIndex + 1 == files.size())
                {
                    throw ImfExc(ImfExc::END);
                }

                printf("Video is skipped to the next.\n");
                setFileIndex(fileIndex + 1);
                break;
            }
            else if (LEFT_KEY)
            {
                if (fileIndex == 0)
                {
                    printf("You are at the file with [0] index!\n");
                    setFileIndex(fileIndex);
                }
                else
                {
                    printf("Video is skipped to the previous.\n");
                    setFileIndex(fileIndex - 1);
                }

                capture.read(image);
                return;
            }
            else if (UP_KEY)
            {
                printf("Video is repeated.\n");
                setFileIndex(fileIndex);
                capture.read(image);
                return;
            }
        }
        while (stopStream == true);

        capture.read(image);
        while (image.empty())
        {
            printf("Video is ended.\n");

            if (fileIndex + 1 == files.size())
            {
                throw ImfExc(ImfExc::END);
            }

            setFileIndex(fileIndex + 1);
            capture.read(image);
        }
    }
    else // (files[fileIndex].type == IMAGE)
    {
        while (true)
        {
            if (keyCode == 0)
            {
                keyCode = -1;
                break;
            }

            keyCode = cv::waitKey(0);

            if (ESC_KEY)
            {
                throw ImfExc(ImfExc::EXIT);
            }
            else if (RIGHT_KEY)
            {
                if (fileIndex + 1 == files.size())
                {
                    throw ImfExc(ImfExc::END);
                }
                else
                {
                    printf("Image is skipped to the next.\n");
                    setFileIndex(fileIndex + 1);
                }

                break;
            }
            else if (LEFT_KEY)
            {
                if (fileIndex == 0)
                {
                    printf("You are at the file with [0] index!\n");
                    setFileIndex(fileIndex);
                }
                else
                {
                    printf("Image is skipped to the previous.\n");
                    setFileIndex(fileIndex - 1);
                }

                capture.read(image);
                return;
            }
        }

        capture.read(image);
        while (image.empty())
        {
            if (fileIndex + 1 == files.size())
            {
                throw ImfExc(ImfExc::END);
            }

            setFileIndex(fileIndex + 1);
            capture.read(image);
        }
    }

    return;
}

/*
    "getImage()" function puts acquired image and its file type to the passed parameters.
    File type parameter is "ImageFlow::file_type".
*/
inline void ImageFlow::getImage(cv::Mat &image, file_type &fileType, std::string &fileName)
{
    getImage(image);

    fileType = files[fileIndex].type;
    fileName = files[fileIndex].name;

    return;
}

inline void ImageFlow::operator>> (cv::Mat &image)
{
    getImage(image);

    return;
}

/*
    "getKeyCode()" function puts ASCII code value of the pressed keyboard key to the passed parameter.
*/
void ImageFlow::getKeyCode(int &keyCode) const
{
    if (pathIsSet == false)
    {
        throw ImfExc(ImfExc::PATH_ERR1);
    }

    keyCode = this->keyCode;

    return;
}

/*
    "intro()" function prints introductory message with the list of control keys.
*/
void ImageFlow::intro() const
{
    printf("ImageFlow\n\n");

    printf("Control keys:\n");
    printf("[ space ] - stop video stream.\n");
    printf("  [ esc ] - terminate the program.\n");
    printf("    [ → ] - skip to the next image/video.\n");
    printf("    [ ← ] - back to the previous image/video.\n");
    printf("    [ ↑ ] - repeat video.\n\n");

    return;
}

/*
    "listDir()" function performs directory listing.
*/
void ImageFlow::listDir(const std::string &path)
{
    DIR *dir = opendir(path.c_str());
    struct dirent *entry;
    std::string name;
    std::string fileExt;
    const std::vector<std::string> EXTENSIONS =
    {
        /* VIDEO: */ ".avi", ".mov", ".mp4",
        /* IMAGE: */ ".bmp", ".jpg", ".jpeg", ".png"
    };

    if (dir != NULL)
    {
        dirPath = path.back() == SLASH ? path : path + SLASH;
        while ((entry = readdir(dir)))
        {
            name = entry->d_name;
            if (name.front() != '.') // Hidden files in UNIX which start from dot character
            {
                size_t npos = name.find_last_of(".");
                if (static_cast<int>(npos) < 1) // Not preferred extension
                {
                    continue;
                }

                fileExt = name.substr(npos);
                std::transform(fileExt.begin(), fileExt.end(), fileExt.begin(), ::tolower);
                for (size_t i = 0; i < EXTENSIONS.size(); ++i)
                {
                    if (fileExt == EXTENSIONS[i])
                    {
                        File file;
                        file.name = name;
                        file.type = i > 2 ? IMAGE : VIDEO;
                        files.push_back(file);
                        break;
                    }
                }
            }
        }

        (void)closedir(dir);

        if (files.empty())
        {
            throw ImfExc(ImfExc::EXT_ERR, path);
        }
    }
    else
    {
        size_t npos = path.find_last_of(SLASH) + 1;
        dirPath = path.substr(0, npos);
        dir = opendir(dirPath.c_str());
        std::string tmprName = path.substr(npos);
        if (dir != NULL)
        {
            while ((entry = readdir(dir)))
            {
                name = entry->d_name;
                if (name == tmprName)
                {
                    size_t npos = name.find_last_of(".");
                    if (static_cast<int>(npos) < 1)
                    {
                        break;
                    }

                    fileExt = name.substr(npos);
                    std::transform(fileExt.begin(), fileExt.end(), fileExt.begin(), ::tolower);
                    for (size_t i = 0; i < EXTENSIONS.size(); ++i)
                    {
                        if (fileExt == EXTENSIONS[i])
                        {
                            File file;
                            file.name = name;
                            file.type = i > 2 ? IMAGE : VIDEO;
                            files.push_back(file);
                            break;
                        }
                    }

                    break;
                }
            }

            (void)closedir(dir);
        }

        if (files.empty())
        {
            throw ImfExc(ImfExc::PATH_ERR2, path);
        }
    }

    return;
}

/*
    "strTimeToMsec()" function converts time from string (HH:MM:SS) to milliseconds.
*/
void ImageFlow::strTimeToMsec(const std::string &strTime, double &msec)
{
    size_t npos1 = strTime.find_first_of(':');
    size_t npos2 = strTime.find_last_of(':');

    if (npos1 == 2 && npos2 == 5 && strTime.length() == 8)
    {
        std::string hours = strTime.substr(0, npos1);
        std::string minutes = strTime.substr(npos1 + 1, npos2 - (npos1 + 1));
        std::string seconds = strTime.substr(npos2 + 1, strTime.length() - npos2 + 1);

        if (((hours.substr(0, 1).find_first_not_of("01") == -1 && hours.substr(1, 1).find_first_not_of("0123456789") == -1)
            || (hours.substr(0, 1).find_first_not_of("2") == -1 && hours.substr(1, 1).find_first_not_of("0123") == -1))
            && (minutes.substr(0, 1).find_first_not_of("012435") == -1 && minutes.substr(1, 1).find_first_not_of("0123456789") == -1)
            && (seconds.substr(0, 1).find_first_not_of("012435") == -1 && seconds.substr(1, 1).find_first_not_of("0123456789") == -1))
        {
            msec = 3600000 * atoi(hours.c_str()) + 60000 * atoi(minutes.c_str()) + 1000 * atoi(seconds.c_str());
        }
        else
        {
            throw ImfExc(ImfExc::TIME_ERR2);
        }
    }
    else
    {
        throw ImfExc(ImfExc::TIME_ERR2);
    }

    return;
}

ImfExc::ImfExc(const int &code)
:   code(code)
{
    if (code == PATH_ERR1)
    {
        message = "Path is unset!";
    }
    else if (code == INDX_ERR1)
    {
        message = "Invalid starting index! 0 index is only valid.";
    }
    else if (code == EXIT)
    {
        message = "\"ImageFlow\" has been stopped.";
    }
    else if (code == END)
    {
        message = "The last file is reached.";
    }
    else if (code == TIME_ERR2)
    {
        message = "Time format is incorrect! Please use following format: HH:MM:SS.";
    }
}

ImfExc::ImfExc(const int &code, const std::string &strVal)
:   code(code)
{
    if (code == PATH_ERR2)
    {
        message = "\"" + strVal + "\" file does not exist!";
    }
    else if (code == EXT_ERR)
    {
        message = "No preferred file formats in \"" + strVal + "\"!";
    }
    else if (code == INDX_ERR2)
    {
        message = "Invalid starting index! Choose within the range 0-" + strVal + ".";
    }
    else if (code == CAPT_ERR)
    {
        message = "Couldn't open \"" + strVal + "\" file.";
    }
    else if (code == TIME_ERR1)
    {
        message = "Can't set time for image file \"" + strVal + "\"!";
    }
}

/*
    Function which returns ImageFlow Exception message.
*/
const char* ImfExc::what() const _NOEXCEPT
{
    return message.c_str();
}