//
//  ImageFlow.cpp
//  ImageFlow
//
//  Created by Zhalgas Baibatyr on 7/1/15.
//  Copyright © 2016 Zhalgas Baibatyr. All rights reserved.
//

#include "ImageFlow.hpp"

#define ESC_KEY   keyCode == 27
#define SPACE_KEY keyCode == 32

#if defined _WIN32

    #define UP_KEY    keyCode == 2490368
/*  #define DOWN_KEY  keyCode == 2621440  */
    #define LEFT_KEY  keyCode == 2424832
    #define RIGHT_KEY keyCode == 2555904

#elif defined __unix__ || defined __APPLE__

    #define UP_KEY    keyCode == 63232
/*  #define DOWN_KEY  keyCode == 63233  */
    #define LEFT_KEY  keyCode == 63234
    #define RIGHT_KEY keyCode == 63235

#endif

/*
    Default constructor.
*/
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

/*
    Destructor.
*/
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
        bool isPaused = false;
        do
        {
            if (isPaused == true)
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
                    isPaused = true;
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
        while (isPaused == true);

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
    else /* if (files[fileIndex].type == IMAGE) */
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
    printf("[ space ] - pause video stream.\n");
    printf("  [ esc ] - terminate the program.\n");
    printf("    [ → ] - skip to the next image/video.\n");
    printf("    [ ← ] - back to the previous image/video.\n");
    printf("    [ ↑ ] - repeat video.\n\n");

    return;
}

/*
    "listDir()" function performs directory listing.
    Can handle case when "path" points to a target file, not a directory.
*/
void ImageFlow::listDir(const std::string &path)
{
    DIR *pDir = opendir(path.c_str());
    struct dirent *entry;
    std::string name;
    std::string fileExt;
    const std::vector<std::string> EXTENSIONS =
    {
        /* VIDEO: */ ".avi", ".mov", ".mp4",
        /* IMAGE: */ ".bmp", ".jpg", ".jpeg", ".png", ".tiff"
    };

#if defined _WIN32

    const char SLASH_CHAR = '\\';

#elif defined __unix__ || defined __APPLE__

    const char SLASH_CHAR = '/';

#endif

    if (pDir != NULL)
    {
        dirPath = path.back() == SLASH_CHAR ? path : path + SLASH_CHAR;

        while ((entry = readdir(pDir)))
        {
            /* Skip folders, hidden files: */
            if (entry->d_name[0] == '.' || entry->d_type != DT_REG)
            {
                continue;
            }

            name = entry->d_name;
            size_t npos = name.find_last_of(".");

            /* Skip files with no extension: */
            if (static_cast<int>(npos) < 0)
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

        (void)closedir(pDir);

        if (files.empty())
        {
            throw ImfExc(ImfExc::EXT_ERR1, path);
        }
    }
    else /* if "path" is a target file, not a directory */
    {
        size_t npos = path.find_last_of(SLASH_CHAR) + 1;
        dirPath = path.substr(0, npos);
        name = path.substr(npos);

#if defined _WIN32

        struct _stat buffer;

        /* Check file for existence: */
        if (_stat(path.c_str(), &buffer) != 0)
        {

#elif defined __unix__ || defined __APPLE__

        struct stat buffer;

        /* Check file for existence: */
        if (stat(path.c_str(), &buffer) != 0)
        {

#endif

            throw ImfExc(ImfExc::PATH_ERR2, path);
        }

        npos = name.find_last_of(".");

        /* If file has no extension: */
        if (static_cast<int>(npos) < 0)
        {
            throw ImfExc(ImfExc::EXT_ERR2, path);
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

        if (files.empty())
        {
            throw ImfExc(ImfExc::EXT_ERR2, path);
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

/*
    Private constructor of "ImageFlow Exception" class.
    It accepts exception code and assigns corresponding message.
*/
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

/*
    Private constructor of "ImageFlow Exception" class.
    It accepts exception code, string parameter and assigns corresponding message.
*/
ImfExc::ImfExc(const int &code, const std::string &extraInfo)
:   code(code)
{
    if (code == PATH_ERR2)
    {
        message = "\"" + extraInfo + "\" file does not exist!";
    }
    else if (code == EXT_ERR1)
    {
        message = "No preferred file formats in \"" + extraInfo + "\"!";
    }
    else if (code == EXT_ERR2)
    {
        message = "Incompatible file format in \"" + extraInfo + "\"!";
    }
    else if (code == INDX_ERR2)
    {
        message = "Invalid starting index! Choose within the range 0-" + extraInfo + ".";
    }
    else if (code == CAPT_ERR)
    {
        message = "Couldn't open \"" + extraInfo + "\" file.";
    }
    else if (code == TIME_ERR1)
    {
        message = "Can't set time for image file \"" + extraInfo + "\"!";
    }
}

/*
    "what()" function which returns "ImageFlow" exception message.
*/
const char* ImfExc::what() const _NOEXCEPT
{
    return message.c_str();
}
