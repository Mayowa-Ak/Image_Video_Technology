#include <fstream>
#include <iostream>
#include <cmath>
#include <limits>
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
         //cout << "The file size of " << file_name << " is: " << file_size << " bytes."<< endl;
         //address of an array of bytes where the read data elements are stored
         file.read(memory_block, file_size);
         file.close();
         //delete [] memory_block;
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
float psnr (const float* image_1, const float* image_2,  int size_x, int size_y, float max_pixel)
{
    float mse_result = mse (image_1, image_2, size_x , size_y);
    if (mse_result != 0)
    {
        float psnrResult = 20 * log10(max_pixel) - 10 * log10(sqrt(mse_result));
        return psnrResult;
    }
    else //cout << "MSE is 0 and PSNR is infinity" << endl;
        return numeric_limits<float>::infinity();
}


// Transpose Matrix
float* transpose(float* matrix)
{
    float* matrix_transpose = new float[256 *256];
    for (int y = 0; y < 256; y++)
    {
        for (int x = 0; x < 256; x++)
        {
            matrix_transpose [256*x +y] = matrix[256*y +x];
        }
    }
    //delete [] matrix_transpose;
    return matrix_transpose;
}


float* matrix_multiplication (float* matrix_1, float* matrix_2)
{
    float* matrix_mult_result = new float [256*256];
    for (int i =0; i<256; i++)
    {
        for (int j=0; j< 256; j++)
        {
            matrix_mult_result[256*i+j] = 0;
            for (int k = 0; k < 256; k++)
            {
                matrix_mult_result[256*i + j] += matrix_1[256*i+k] * matrix_2[k*256 + j];
            }
        }
    }
    //delete [] matrix_mult_result;
    return matrix_mult_result;
}



// DCT transform coefficients
float *DCT_Transform (float* image, float* basis)
{
    float* transformation_result = new float [256*256];
    float* temp = new float[256*256];
    temp = matrix_multiplication(basis, image);
    transformation_result = matrix_multiplication(temp, transpose(basis));
    //delete [] transformation_result;
    //delete [] temp;
    return transformation_result;
}



// Threshold function
float* threshold (float* image, double thresh)
{
    float* thresh_result = new float [256*256];
    for (int i =0; i < 256 * 256; i++)
    {
        if (abs(image[i]) < thresh)
        {
            thresh_result[i] = 0;
        }
        else thresh_result[i] = image[i];
    }
    //delete [] thresh_result;
    return thresh_result;
}




int main ()
{
    int size_x = 256;
    int size_y = 256;
    float max_pixel = 255;
    int dimension = 256;
    int image_dimension = 256*256;
    float alpha[dimension];
    const float pi = 3.14159265359;
    float DCT[image_dimension];
    float psnr_original, psnr_r, psnr0, psnr1, psnr5, psnr10, psnr50, psnr100, psnr255;

    // DCT Matrix
    for (int i = 0; i < dimension; i++)
    {
        alpha[i] = sqrt(2.0 / dimension);
    }
    alpha[0] = sqrt(1.0 / dimension);

    for (int i = 0; i < dimension; i++ )
    {
        for (int j =0; j < dimension; j++)
        {

            DCT[i*dimension+j] =  (alpha[i]) * cos((j + 0.5) * i*(pi)/dimension);

        }
    }
    //store (DCT, "DCT");  //DCT basis


    // Identity matrix
    float* identity_matrix = matrix_multiplication (DCT, transpose(DCT));
    //store(identity_matrix, "identity_matrix");


    //IDCT
    //store (transpose(DCT), "IDCT");


    float* lena_image = load ("lena_256x256.raw");

    float* DCT_coefficients = DCT_Transform(lena_image, DCT);
    //store(DCT_coefficients, "DCT_coefficients_Lena");

    float* thresh_0 = threshold(DCT_coefficients, 0);
    float* thresh_1 = threshold(DCT_coefficients, 1);
    float* thresh_5 = threshold(DCT_coefficients, 5);
    float* thresh_10 = threshold(DCT_coefficients, 10);
    float* thresh_50 = threshold(DCT_coefficients, 50);
    float* thresh_100 = threshold(DCT_coefficients, 100);
    float* thresh_255 = threshold(DCT_coefficients, 255);




    float* reconstruct_lena_image = matrix_multiplication(matrix_multiplication(transpose(DCT),DCT_coefficients),DCT);
    //store(reconstruct_lena_image, "reconstruct_lena_image");

    float* reconstruct_lena_image_thresh_0 = matrix_multiplication(matrix_multiplication(transpose(DCT),thresh_0),DCT);
    //store(reconstruct_lena_image_thresh_0, "reconstruct_lena_image_thresh_0");

    float* reconstruct_lena_image_thresh_1 = matrix_multiplication(matrix_multiplication(transpose(DCT),thresh_1),DCT);
    //store(reconstruct_lena_image_thresh_1, "reconstruct_lena_image_thresh_1");

    float* reconstruct_lena_image_thresh_5 = matrix_multiplication(matrix_multiplication(transpose(DCT),thresh_5),DCT);
    //store(reconstruct_lena_image_thresh_5, "reconstruct_lena_image_thresh_5");

    float* reconstruct_lena_image_thresh_10 = matrix_multiplication(matrix_multiplication(transpose(DCT),thresh_10),DCT);
    //store(reconstruct_lena_image_thresh_10, "reconstruct_lena_image_thresh_10");

    float* reconstruct_lena_image_thresh_50 = matrix_multiplication(matrix_multiplication(transpose(DCT),thresh_50),DCT);
    //store(reconstruct_lena_image_thresh_50, "reconstruct_lena_image_thresh_50");

    float* reconstruct_lena_image_thresh_100 = matrix_multiplication(matrix_multiplication(transpose(DCT),thresh_100),DCT);
    //store(reconstruct_lena_image_thresh_100, "reconstruct_lena_image_thresh_100");

    float* reconstruct_lena_image_thresh_255 = matrix_multiplication(matrix_multiplication(transpose(DCT),thresh_255),DCT);
    //store(reconstruct_lena_image_thresh_255, "reconstruct_lena_image_thresh_255");




    psnr_original = psnr (lena_image,lena_image, size_x , size_y, max_pixel);
    psnr_r = psnr (lena_image, reconstruct_lena_image, size_x , size_y, max_pixel);
    psnr0 = psnr (lena_image, reconstruct_lena_image_thresh_0, size_x , size_y, max_pixel);
    psnr1 = psnr (lena_image, reconstruct_lena_image_thresh_1, size_x , size_y, max_pixel);
    psnr5 = psnr (lena_image, reconstruct_lena_image_thresh_5, size_x , size_y, max_pixel);
    psnr10 = psnr (lena_image, reconstruct_lena_image_thresh_10, size_x , size_y, max_pixel);
    psnr50 = psnr (lena_image, reconstruct_lena_image_thresh_50, size_x , size_y, max_pixel);
    psnr100 = psnr (lena_image, reconstruct_lena_image_thresh_100, size_x , size_y, max_pixel);
    psnr255 = psnr (lena_image, reconstruct_lena_image_thresh_255, size_x , size_y, max_pixel);

    cout << endl;


    cout << "psnr_original: " << psnr_original << endl;
    cout << "psnr_r: " << psnr0 << endl;
    cout << "psnr0: " << psnr0 << endl;
    cout << "psnr1: " << psnr1 << endl;
    cout << "psnr5: " << psnr5 << endl;
    cout << "psnr10: " << psnr10 << endl;
    cout << "psnr50: " << psnr50 << endl;
    cout << "psnr100: " << psnr100 << endl;
    cout << "psnr255: " << psnr255 << endl;

    return 0;
}



















