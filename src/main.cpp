#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <cmath>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>

using namespace::cv;
using namespace::std;

enum ForceMeasureType
{
    IMAGE_CONTRAST = 0,
    GRADIENT_ENERGY = 1,
    DIAGONAL_LAPLACIAN = 2,
};

// Change this to change the fore measure computation method.
ForceMeasureType force_measure_type = IMAGE_CONTRAST;


//Gradient Kernel for Sobel Operator--------------------------------
vector < vector<int> > Gx = { {-1, 0, 1},
                              {-2, 0, 2},
                              {-1, 0, 1} };

vector < vector<int> > Gy = { {-1, -2, -1},
                              { 0,  0,  0},
                              { 1,  2,  1} };

// Diagonal Laplacian kernel----------------------------------------
vector <int> Lx = {-1, 2, -1};

vector < vector<int> > Ly = { {-1}, 
                              { 2}, 
                              {-1} };

vector < vector<int> > Lx1 = { {0,  0, 1},
                               {0, -2, 0},
                               {1,  0, 0} };

vector < vector<int> > Lx2 = { {1,  0, 0},
                               {0, -2, 0},
                               {0,  0, 1} };



// Read Images from Directory--------------------------------------

typedef vector<string> stringvec;

void read_directory(const string& name, stringvec& v)
{
    DIR* dirp = opendir(name.c_str());
    struct dirent * dp;
    while ((dp = readdir(dirp)) != NULL) {
        v.push_back(dp->d_name);
    }
    closedir(dirp);
}

// Image Contrast Method -----------------------------------------

int get_pair_intensity(const Vec3b& A, const Vec3b& B)
{
    int b = abs ( A[0] - B[0] ); 
    int g = abs ( A[1] - B[1] ); 
    int r = abs ( A[2] - B[2] ); 
    return (b + g + r);
}

int contrast(Mat &img, int x0, int y0)
{
    int sum = 0;
    for (int dx = -1; dx <= 1; dx++)
    {
        for (int dy = -1; dy <= 1; dy++)
        {
            int x = x0 + dx;
            int y = y0 + dy;

            if (x >= 0 && x < img.rows && y >= 0 && y < img.cols)
                sum += get_pair_intensity( img.at<Vec3b> (x0, y0), 
                                           img.at<Vec3b> ( x,  y) );
        }
    }

    return sum;
}

// Gradient Energy Method -----------------------------------------

int get_pair_gradient(const Vec3b& A, int x)
{
    int b = A[0] * x;
    int g = A[1] * x;
    int r = A[2] * x;
    
    return (b + g + r);
}

int gradient (Mat &img, int x0, int y0)
{
    int sum_X = 0;
    int sum_Y = 0;
    for (int dx = -1; dx <= 1; dx++)
    {
        for (int dy = -1; dy <= 1; dy++)
        {
            int x = x0 + dx;
            int y = y0 + dy;

            if (x >= 0 && x < img.rows && y >= 0 && y < img.cols)
                sum_X += get_pair_gradient( img.at<Vec3b> (x, y), Gx[dx+1][dy+1] );


            if (x >= 0 && x < img.rows && y >= 0 && y < img.cols)
                sum_Y += get_pair_gradient( img.at<Vec3b> (x, y), Gy[dx+1][dy+1] );
        }
    }

    return ( sum_X * sum_X + sum_Y * sum_Y );
}

// Diagonal Laplacian Method -------------------------------------

int get_pair_laplacian (const Vec3b& A, int x)
{
    int b = A[0] * x;
    int g = A[1] * x;
    int r = A[2] * x;
    
    return (b + g + r);

}

int laplacian (Mat &img, int x0, int y0)
{
    int sum_Lx = 0;
    int sum_Ly = 0;
    int sum_Lx1 = 0;
    int sum_Lx2 = 0;

    for (int dy = -1; dy <= 1; dy++)
    {
        int x = x0;
        int y = y0 + dy;

        if (y >= 0 && y < img.cols)
            sum_Lx += get_pair_laplacian( img.at<Vec3b> (x, y), Lx[dy+1] );

    }

    for (int dx = -1; dx <= 1; dx++)
    {
        int dy = 0;
        int x = x0 + dx;
        int y = y0 + dy;

        if (x >= 0 && x < img.rows)
            sum_Ly += get_pair_laplacian( img.at<Vec3b> (x, y), Ly[dx+1][dy] );
    }
    
    for (int dx = -1; dx <= 1; dx++)
    {
        for (int dy = -1; dy <= 1; dy++)
        {
            int x = x0 + dx;
            int y = y0 + dy;

            if (x >= 0 && x < img.rows && y >= 0 && y < img.cols)
                sum_Lx1 += get_pair_laplacian( img.at<Vec3b> (x, y), Lx1[dx+1][dy+1] );

            if (x >= 0 && x < img.rows && y >= 0 && y < img.cols)
                sum_Lx2 += get_pair_laplacian( img.at<Vec3b> (x, y), Lx2[dx+1][dy+1] ); 
        }
    }

    return ( sum_Lx + sum_Ly + sum_Lx1 + sum_Lx2 );

}

// Main Function -------------------------------------------------

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        cerr << "Incorrect number of arguments" << endl;
        return 1;
    }

    // Store images into a vector v -------------------------------

    stringvec v;
    string path = argv[1];
    read_directory(path, v);
    mkdir(argv[2], S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    double high_relative_focus = INT_MIN;
    string high_focus_img;
    
    // Iterate images from vector -------------------------------
    
    for (int k = 0; k < v.size(); k++)
    {
        if (v[k] == "." || v[k] == "..")
            continue;

        Mat image = imread(path + v[k], 1 );
        
        if ( !image.data )
        {
            printf("No image data \n");
            return -1;
        }

        // Compute Relative focus Measure -------------------------

        for(int i = 0; i < image.rows; i++)
        {
            for(int j = 0; j < image.cols; j++)
            {   
                int pixel_contrast = 0;

                if (force_measure_type == IMAGE_CONTRAST)
                    pixel_contrast = contrast(image, i, j);

                if (force_measure_type == GRADIENT_ENERGY)
                    pixel_contrast = gradient(image, i, j);

                if (force_measure_type == DIAGONAL_LAPLACIAN)
                    pixel_contrast = laplacian(image, i, j);

                if (pixel_contrast > high_relative_focus)
                {
                    high_relative_focus = pixel_contrast;
                    high_focus_img = v[k];
                }
            }
        } 
    }
    
    // store the image into output directory --------------------

    string command = "cp  " + path + "/" + high_focus_img + " " + argv[2];
    system(command.c_str());
    cout << "DONE !!!" << endl;

    return 0;
}
