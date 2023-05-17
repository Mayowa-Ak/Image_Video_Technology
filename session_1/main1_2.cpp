#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
using namespace std;


//store function
int store (float *image, string name)
{
    int image_size = 256 * 256;
    ofstream file;
    file.open(name + ".raw", ios:: binary);
    if (file.is_open())
    {
        //address of an array of bytes where the data elements to be written are taken
        file.write(reinterpret_cast<char*>(image), image_size * sizeof (float));
        file.close();
    }
    else cout<<"Unable to open file" <<endl;
    return 0;
}


 // load function
float *load (const char *file_name)

 {
     ifstream file;
     file.open(file_name,ios::binary|ios::ate);//pointer positioned at end of file
     if (file.is_open())
     {
         streampos file_size = file.tellg(); //obtain file size
         char *memory_block = new char [file_size]; //allocation of memory block to hold file
         file.seekg (0, ios::beg); // position at beginning of file
         cout << "The file size of " << file_name << " is: " << file_size << " bytes."<< endl;
         //address of an array of bytes where the read data elements are stored
         file.read(memory_block, file_size);
         file.close();
         delete [] memory_block;
         return (float*) memory_block;

     }
     else cout << file_name << " can not be found." << endl;
     return 0;
 }


// mse function
float mse (const float* image_1, const float* image_2, int size_x , int size_y)
{
    float squared_error = 0;
    float mse_result;
    int pixels = size_x * size_y;
    for (int i = 0; i < pixels; i++)
    {
        squared_error += pow(image_1[i] - image_2[i], 2);
        mse_result = (squared_error/pixels);
    }
    return mse_result;
}


//psnr function
float psnr (const float* image_1, const float* image_2, float max_pixel, int size_x, int size_y)
{
    float mse_result = mse (image_1, image_2, size_x , size_y);
    if (mse_result != 0)
    {
        float psnrResult = 20 * log10(max_pixel) - 10 * log10(sqrt(mse_result));
        return psnrResult;
    }
    else cout << "MSE is 0 and PSNR is infinity" << endl;
    return 0;
}


int main()
{
    int size_x = 256;
    int size_y = 256;
    float max_pixel = 255;
    float gray_image [256 * 256]; //static memory allocation
    float modified_lena[256 * 256];
    const double pi = 3.14159265359;

    for (int y = 0; y < 256; y++) // vert
    {
       for (int x = 0; x < 256; x++) // hor
       {
           gray_image[y * 256 + x] = 0.5 + 0.5 * cos (x*pi/32) * cos (y * pi/64);
       }
    }
    //store (gray_image, "grayscale_image");

    float *original_lena = load ("lena_256x256.raw");


    for (int i = 0; i < 256*256; i++)
    {
      modified_lena[i] =  gray_image[i] * original_lena[i];
    }
      //store (modified_lena, "lena_modified");


      cout<< "MSE: " << mse((float*) original_lena, (float*) modified_lena, size_x, size_y) << endl;
      cout << "PSNR: " << psnr ((float*) original_lena, (float*) modified_lena, max_pixel, size_x, size_y) <<endl;

    return 0;

}

