--- StitcHD Project ---

1. View Demo Videos

www.youtube.com/watch?v=3in4e_yYOIA
www.youtube.com/watch?v=mMcrOpVx9aY


2. Running binaries with pre-compiled libraries

a) Copy the appropriate zip file in Bin/ to your hard-disk.
b) Unzip the file.
c) Run GUI.exe


3. Building from source

a) Read the report in Documentation/ for installing 3rd party libraries.
b) Create the necessary environment variables and update the path after installing the libraries.
c) Build the source using Visual Studio 2010.

--- Executive Summary ---

This NASA funded project is intended to develop a video system which will ultimately replace windows on their spacecrafts. They have requested that we attack the problem of stitching high-definition video feeds together as part of the goal of the overall system. While keeping in mind the end goal of a comprehensive video system, we will focus on the mechanics of streaming several high-bandwidth, high-definition camera feeds and stitching them together quickly andreliably into a panoramic video stream.

Our goal is to provide NASA with a program that helps a user to quickly learn how the mechanics of stitching videos together work. A user should be able to use our graphical user interface (GUI) to explore the different parameters used in the different modules of our system to determine how each parameter affects the speed and quality of the resulting stitched video stream.

We implemented video stitching by separating our code into several threads. First, there is a separate CameraCapture thread for of the cameras, allowing them all to stream simultaneously. Then, once the images are retrieved from each camera, there are two operations which must be done to create a stitched panorama from the images. The images must be analyzed to find the relationship between them, and the images must be stitched together into one panoramic image.

First, the images have to be analyzed based on the overlap region between the cameras to calculate how much each image needs to be transformed to align the images appropriately. This is done by utilizing some of OpenCV’s computer vision algorithms to calculate a perspective transformation between two images. These parameters are called homographies, so we have dubbed these threads, “Homographiers.” Several of these homographies are calculated between several pairs of images so that in the end, all of the images can be stitched together. Each of these calculations can be done independently, so they are each done in a separate thread.

Once the perspective transformation parameters have been calculated, the main thread can take images from each of the CameraCapture threads, and parameters from each of the Homographier threads, and use them to stitch the images together. The stitching is done on the GPU, since it is an easily parallelizable problem. And the output images are displayed on a GUI built using the Qt framework. This GUI not only displays the stitched video stream, but it also allows the user to make changes to various configuration options on the fly, and see the resulting changes in the output video immediately. These configuration options let the user specify several options about how the images are stitched together, and a plethora of options regarding how the Homographiers should calculate the homographies.

The results for this project can be seen in the attached demo videos. The GUI allows the user to adjust the parameters until they are content with the results, up to the theoretical limits of the algorithms used and the physical limits of the hardware used. Our project also helps the user to understand how the positioning of the cameras and the relative distance to different objects in the scene affect the quality and stability of the output video. We feel we have met our goal of creating a useful tool for learning the mechanics of video stitching.

The timing results of this project, however, are disappointing on several points, due not to errors in the code, but to limitations introduced by our USB 2.0 cameras and from sharing a single GPU with other processes on the computer. Solutions to these problems are given in the results section below. We had initially aimed to stitch high-definition video at high-definition speeds, but quickly realized that hardware limitations would not let us reach that objective. So, rather than spending more money on expensive hardware, we have provided mechanisms for measuring these limitations alongside developing a program that is useful for educational purposes regardless of what performance the hardware is able to deliver.

The experience we had working together on a team was a positive one, as was our relationship working closely with NASA. It was rough in the beginning trying to discover the most productive way to communicate and get things done, but these problems were noticed and resolved about a fourth of the way into the semester. The biggest issue was time management, and finding a time that every member of the team was available. The most rewarding part of our project to all of us was being able to work for a real client, especially such a well-renowned company like NASA. The fact that our research will go on to serve important endeavors such as space exploration was excellent motivation this semester to develop a quality product.