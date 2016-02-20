# ImageFlow
"ImageFlow" takes control of your image capturing with the help of OpenCV library. It provides control keys to traverse across video/image files.

Control keys:<br>
[ space ] - stop video stream.<br>
&nbsp;&nbsp;&nbsp;&nbsp;[ esc ] - terminate the program.<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[ → ] - skip to the next image/video.<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[ ← ] - back to the previous image/video.<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[ ↑ ] - repeat video.<br><br>
Provided control keys work only when you call `cv::imshow()` function. <b>main.cpp</b> file contains demo.<br><br>
Dependencies:<br>
* <b>OpenCV 3.1.0</b> library (link <b>core</b>, <b>videoio</b> and <b>highgui</b> dll files)
* <b>dirent.h</b> file (for Windows OS you will need to download it from the web, UNIX based OS comes with it by default)
