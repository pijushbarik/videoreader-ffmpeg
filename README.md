# Video Reader Using FFMPEG
Reads a video stream from a input file, http feed, usb cam, ip cam etc. and 
converts the frame data into a OpenCV Mat object.

## How to build
The code can be built with CMake. First create a directory called `build` and 
cd to the `build` folder. Then run `cmake ..` to create Makefiles. 

After run `make` to compile the program.

## How to run
To run the compiled code, from `build` directory run `./main <input video stream>`

Examples:
```
# Video file
./main /home/pijush/videos/rain.avi

# http stream
# I used Droidcam to feed video stream from my phone
./main http://10.7.4.117:4747/mjpegfeed?640x480
```