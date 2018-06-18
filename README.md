# Computer_Vision
Computing relative focus measure

Steps:
1. Download Zip 
2. Open terminal and go to the directory where you have dowloaded the file
3. Unpack all the files in the directory using command.
   $ unzip -a <file_name>.zip
4. You will get following directories: input, src.
   $ ls
5. Create a new directory as 'build' and Go to the 'build' directory as follows: 
   $ mkdir build
   
   $ cd build/
6. Run following commands to get output.
   $ cmake .. && make
8. For first set of images use Sequence1 and for second set of images 
   use Sequence2 as mentioned below:
9. $ ./measure_focus ../input/Sequence1/ ../output/

   $ ./measure_focus ../input/Sequence2/ ../output/
11. Once you run above commands you will receive a message as "DONE !!!".
12. An output directory will be created which contains the image with highest relative measure.


