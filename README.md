# Gummibears - object counter in OpenCV and C++

_Gummibears_ is a project in OpenCV and C++ that can be used to count the number of colorful gummibears placed on a white sheet of paper. 
 <p align="center">
  <img src="https://github.com/PawelTypiak/Gummibears/blob/master/OpenCVProject2/images/scena_004_ujecie_002.jpg" width="800" />
  </p>

#### Rules 
- There are 5 colors of objects: dark red, light red, orange, green, yellow and white
- There are 5 scenes and every scene have several images of every scene
- There are 3 types of markers used to limit the area of counting bears. 
- We can count only objects which are placed inside the markers. Markers' patterns can be found in [markers.pdf](https://github.com/PawelTypiak/Gummibears/blob/master/markers.pdf)
- Names of pictures have to be downloaded from txt file placed in  [OpenCVProject2/images_names/images_names.txt](https://github.com/PawelTypiak/Gummibears/blob/master/OpenCVProject2/images_names/images_names.txt)
- Images are placed in [OpenCVProject2/images/](https://github.com/PawelTypiak/Gummibears/tree/master/OpenCVProject2/images)
- Results are saved in txt file placed in [OpenCVProject2/results/](https://github.com/PawelTypiak/Gummibears/tree/master/OpenCVProject2/results)
- Number of objects in every scene:

 Scene/Color | Dark red | Light Red | Green | Orange | White | Yellow  
:------------: | :------------: | :------------:| :------------: | :------------: | :------------: | :------------:
**Scene 1** | 3 | 3 | 3 | 3 | 3 | 3
**Scene 2** | 8 | 9 | 0 | 0 | 0 | 0
**Scene 3** | 0 | 0 | 5 | 19 | 13 | 14 
**Scene 4** | 7 | 6 | 3 | 12 | 8 | 10
**Scene 5** | 2 | 6 | 6 | 6 | 7 | 7

#### How does it work?
1. Creating markers' patterns:
  * Drawing every marker
  * Finding edges of markers using **Canny**
  * Finding contours of every marker using **findContours**
  * Calculating moments and centers of gravity using **moments**
2. Loading images:
  * Downloading name of every file from images_names.txt
  * Loading files using **imread**
3. Processing every image in a loop:
  * checking if current image is first or last image of the scene
  * resizing images for better performance
  * image binarization - dark objects become white, every objects that has r>50, g>50, b>50 become black
  * converting images into grayscale using **cvtColor**
  * finding contours of every white object
  * saving only contours which have fulfill the criterias of markers (proper area and diameter of circumcurcle using **contourArea** and **minEnclosingCircle**) 
  * finding the distance of every object from the middle of current picture using **pointPolygonTest**
  * finding what type of marker we have by matching shapes with patterns using **matchShapes**
  * finding corners - center of gravity for circle and L-shape marker, and intersections of lines fitted in rectangles for third marker (**fitLine**)
  * finding the right order of connecting corners
  * cutting the interior of markers
  * finding objects of every particular color and filtrating to delete the sharp edges and noise (**medianBlur**, **dilate**, **erode**)
  * saving objects of different colors in different Mats
  * if it is the first image of the scene, the vectors are cleared
  * if it is the last image of the scene - computing how many objects are on the image - number is based on their area
  * sorting the number of objects in particular colors from min to max for every image in current scene
  * using median for objects found on every picture for current scene
  * results are printed on the screen and are saved in results.txt
  
 #### Licensing
_Gummibears_ is licensed under **_Apache 2.0_** license. You can read more about the terms of this license on the [web page](https://www.apache.org/licenses/LICENSE-2.0) or in [LICENSE.txt](/LICENSE.txt) file.
  
  
  
  
  
  
  
  
  
