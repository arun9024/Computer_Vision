# Computer_Vision
Computing relative focus measure
Steps:
1. Download Zip 
2. Open terminal and go to the directory where you have dowloaded the file
3. Unpack all the files in the directory using command
   $ unzip -a Computer_Vision-master.zip
4. You will get following directories: build, input, output, src
   $ ls
5. Go to the build directory 
   $ cd build/
6. Run following commands to get output
7. $ cmake .. && make
8. For first set of images use Sequence1 and for second set of images 
   use Sequence2 as mentioned below:
9. $ ./measure_focus ../input/Sequence1/ ../output/ 
10. $ ./measure_focus ../input/Sequence2/ ../output/
11. Once you run above commands you will receive a message as "DONE !!!", 
   You will get an image in the output directory with highest relative measure


