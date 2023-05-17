#include <fstream>
#include <iostream>
#include <cmath>
#include <string>


using namespace std;


//store function
int store (float *image, string name, int img_size)
{
    int image_size = img_size * img_size;
    ofstream file;
    file.open(name + ".raw", ios:: binary);
    if (file.is_open())
    {
        //myFile.write((const char *)image, image_size * sizeof (float) );
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
         //address of an array of bytes where the read data elements are stored
         file.read(memory_block, file_size);
         file.close();
         return (float*) memory_block;

     }
     else cout << file_name << " can not be found." << endl;
     //delete [] memory_block;
     return 0;
 }



// Matrix multiplication
float* matrix_multiplication (float* matrix_1, float* matrix_2, int matrix_size)
{
    float* matrix_mult_result = new float [matrix_size * matrix_size];

    for (int i =0; i < matrix_size ; i++)
    {
        for (int j=0; j < matrix_size; j++)
        {
            matrix_mult_result[matrix_size*i+j] = 0;
            for (int k = 0; k < matrix_size; k++)
            {
                matrix_mult_result[matrix_size*i + j] += matrix_1[matrix_size*i+k] * matrix_2[k*matrix_size + j];
            }
        }
    }
    //delete [] matrix_mult_result;
    return matrix_mult_result;
}



// Transpose Matrix
float* transpose(float* matrix, int matrix_size)
{
    float* matrix_transpose = new float[matrix_size * matrix_size];
    for (int y = 0; y < matrix_size; y++)
    {
        for (int x = 0; x < matrix_size; x++)
        {
            matrix_transpose [matrix_size*x +y] = matrix[matrix_size*y +x];
        }
    }
    //delete [] matrix_transpose;
    return matrix_transpose;


}



// DCT transform // coefficients
float *DCT_Transform (float* image, float* basis, int image_size)
{
    float* transformation_result = new float [image_size * image_size];
    float* temp = new float[image_size * image_size];
    temp = matrix_multiplication(basis, image, image_size);
    transformation_result = matrix_multiplication(temp, transpose(basis, image_size), image_size);
    //delete [] transformation_result;
    //delete [] temp;
    return transformation_result;

}



 // DCT Matrix
 float* DCT_matrix (int dimension)
 {
     const float pi = 3.14159265359;
     float* DCT = new float [dimension * dimension];
     float* alpha = new float [dimension * dimension];

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
    //delete [] DCT;
    //delete [] alpha;
    return DCT;  //DCT basis

 }


// Approximate function

float* approximate(float* image, float* Q)
{
    int pixel_block = 8 * 8;
    int image_size = 256 * 256;
    int image_dim = 256;
    int pixels = 8;
    int block_count = 32;
    float* DCT = new float[pixel_block];
    float* image_8x8 = new float[pixel_block];
    float* image_8x8_DCT = new float [pixel_block];
    float* image_8x8_DCT_Q = new float [pixel_block];
    float* image_8x8_DCT_Q_IQ = new float [pixel_block];
    float* image_8x8_DCT_Q_IQ_IDCT = new float [pixel_block];
    float* image_256x256_DCT = new float [image_size];
    float* image_256x256_DCT_Q = new float [image_size];
    float* image_256x256_DCT_Q_IQ = new float [image_size];
    float* image_256x256_DCT_Q_IQ_IDCT = new float [image_size];


    DCT =  DCT_matrix (pixels);

    //store(DCT, "DCT_matrix_8", pixels);


    for (int i = 0; i < block_count; i++)
    {
        for (int j = 0; j < block_count; j++)
        {
            for (int m = 0; m < pixels; m++)
            {
                for (int n =0; n < pixels; n++)
                {
                     image_8x8[pixels * m + n] = image [image_dim * (pixels * i + m)+ (pixels * j + n)]; //convert input image to 8x8 pixel block

                }

            }



                // 8x8 DCT Image
            float* image_8x8_DCT = DCT_Transform(image_8x8, DCT,pixels);
            for (int m = 0; m < pixels; m++)
            {
                for (int n = 0; n < pixels; n++)
                {
                     image_256x256_DCT[image_dim * (pixels * i + m) + (pixels * j + n)] = image_8x8_DCT [pixels * m + n];
                }
            }

            // Quantization

            for (int m = 0; m < pixels; m++)
            {
                for (int n = 0; n < pixels; n++)
                {
                    image_8x8_DCT_Q[pixels * m + n] = round (image_8x8_DCT[pixels * m + n] / Q[pixels * m + n]);
                    image_256x256_DCT_Q[image_dim * (pixels * i + m) + (pixels * j + n)]  = image_8x8_DCT_Q[pixels * m + n];
                }
            }


            // IQ
            for (int m = 0; m < pixels; m++)
            {
                for (int n = 0; n < pixels; n++)
                {
                    image_8x8_DCT_Q_IQ [pixels * m + n] = image_8x8_DCT_Q[pixels * m + n]*Q[pixels * m + n];
                    image_256x256_DCT_Q_IQ[image_dim * (pixels * i + m) + (pixels * j + n)]  = image_8x8_DCT_Q_IQ[pixels * m + n];
                }
            }


            // IDCT

			image_8x8_DCT_Q_IQ_IDCT = matrix_multiplication(matrix_multiplication(transpose(DCT,pixels),image_8x8_DCT_Q_IQ,pixels),DCT,pixels);
			for (int m = 0;m < pixels;m++)
			{
				for (int n = 0;n < pixels;n++)
				{
					image_256x256_DCT_Q_IQ_IDCT[256 * (pixels * i + m) + (pixels * j + n)] = image_8x8_DCT_Q_IQ_IDCT[pixels * m + n]; // IDCT image
				}
			}

        }
    }

   /*


    store(image_8x8,"image_8x8",pixels);
    store(image_8x8_DCT,"image_8x8_DCT",pixels);
    store(image_256x256_DCT,"image_256x256_DCT",image_dim);
	store(image_256x256_DCT_Q,"image_256x256_DCT_Q",image_dim);
	store(image_256x256_DCT_Q_IQ, "image_256x256_DCT_Q_IQ",image_dim);
	store(image_256x256_DCT_Q_IQ_IDCT,"image_256x256_DCT_Q_IQ_IDCT",image_dim);
	*/
	cout << "Approximate function: DCT, Q, IQ, IDCT saved"  << endl;

	/*
	delete [] image_256x256_DCT_Q_IQ_IDCT;
    delete [] image_256x256_DCT_Q_IQ;
    delete [] image_256x256_DCT_Q;
    delete [] image_256x256_DCT;
    delete [] image_8x8_DCT_Q_IQ_IDCT;
    delete [] image_8x8_DCT_Q_IQ;
    delete [] image_8x8_DCT_Q;
    delete [] image_8x8_DCT;
    delete [] image_8x8;
    delete [] DCT;
    */

	return image_256x256_DCT_Q_IQ_IDCT;


}


// clip function
float* clip(float* image, int image_dim)
{
	float* clipped_image = new float[image_dim];
	for (int i = 0; i < image_dim; i++)
	{
		if (image[i] > 255)
		{
			clipped_image[i] = 255; //max 255
		}
		else if (image[i] < 0)
		{
			clipped_image[i] = 0; //min 0
		}
		else
		{
			clipped_image[i] = round(image[i]);
		}
	}

	//delete []clipped_image;
	return clipped_image;

}


//Encode function contigous
float* encode(float* image, float* Q)
{

    int pixel_block = 8 * 8;
    int image_size = 256 * 256;
    int image_dim = 256;
    int pixels = 8;
    int block_count = 32;
    float* DCT = new float[pixel_block];
    float* image_8x8 = new float[pixel_block];
    float* image_8x8_DCT = new float [pixel_block];
    float* image_8x8_DCT_Q = new float [pixel_block];
    float* image_256x256_DCT = new float [image_size];
    float* image_256x256_DCT_Q = new float [image_size];

	DCT =  DCT_matrix (pixels);

	for (int i = 0; i < block_count; i++)
    {
        for (int j = 0; j < block_count; j++)
        {
            for (int m = 0; m < pixels; m++)
            {
                for (int n =0; n < pixels; n++)
                {
                    image_8x8[pixels * m + n] = image [image_dim * (pixels * i + m)+ (pixels * j + n)];
                }

            }

                // 8x8 DCT Image
            image_8x8_DCT = DCT_Transform(image_8x8, DCT,pixels);
            for (int m = 0; m < pixels; m++)
            {
                for (int n = 0; n < pixels; n++)
                {
                    image_256x256_DCT[image_dim * (pixels * i + m) + (pixels * j + n)] = image_8x8_DCT [pixels * m + n];
                }
            }

            // Quantization

            for (int m = 0; m < pixels; m++)
            {
                for (int n = 0; n < pixels; n++)
                {
                    image_8x8_DCT_Q[pixels * m + n] = round (image_8x8_DCT[pixels * m + n] / Q[pixels * m + n]);
                    image_256x256_DCT_Q[image_dim * (pixels * i + m) + (pixels * j + n)]  = image_8x8_DCT_Q[pixels * m + n];
                }
            }

        }
    }
/*
    delete [] image_256x256_DCT_Q;
    delete [] image_256x256_DCT;
    delete [] image_8x8_DCT_Q;
    delete [] image_8x8_DCT;
    delete [] image_8x8;
    delete [] DCT;
    */
    return image_256x256_DCT_Q;
}

//decode contigous

float* decode(float* image, float* Q) //received float* image_256x256_DCT_Q
{

	int pixel_block = 8 * 8;
    int image_size = 256 * 256;
    int image_dim = 256;
    int pixels = 8;
    int block_count = 32;
    float* DCT = new float[pixel_block];
    float* image_8x8_DCT_Q = new float [pixel_block];
    float* image_8x8_DCT_Q_IQ = new float [pixel_block];
    float* image_8x8_DCT_Q_IQ_IDCT = new float [pixel_block];
    float* image_256x256_DCT_Q_IQ = new float [image_size];
    float* image_256x256_DCT_Q_IQ_IDCT = new float [image_size];



	DCT =  DCT_matrix (pixels);

	for (int i = 0; i < block_count; i++)
    {
        for (int j = 0; j < block_count; j++)
        {
            for (int m = 0; m < pixels; m++)
            {
                for (int n =0; n < pixels; n++)
                {
                    image_8x8_DCT_Q[pixels * m + n] = image [image_dim * (pixels * i + m)+ (pixels * j + n)];
                }

            }
             // IQ
            for (int m = 0; m < pixels; m++)
            {
                for (int n = 0; n < pixels; n++)
                {
                    image_8x8_DCT_Q_IQ [pixels *m + n] = image_8x8_DCT_Q[pixels*m+n] * Q[pixels *m +n];
                    image_256x256_DCT_Q_IQ[image_dim * (pixels * i + m) + (pixels * j + n)]  = image_8x8_DCT_Q_IQ[pixels * m + n];
                }
            }


            // IDCT
            image_8x8_DCT_Q_IQ_IDCT = matrix_multiplication(matrix_multiplication(transpose(DCT,pixels),image_8x8_DCT_Q_IQ,pixels),DCT,pixels);

			for (int m = 0;m < pixels;m++)
			{
				for (int n = 0;n < pixels;n++)
				{
					image_256x256_DCT_Q_IQ_IDCT[256 * (pixels * i + m) + (pixels * j + n)] = image_8x8_DCT_Q_IQ_IDCT[pixels * m + n]; // IDCT image
				}
			}

        }
    }

    /*
    delete [] image_256x256_DCT_Q_IQ_IDCT;
    delete [] image_256x256_DCT_Q_IQ;
    delete [] image_8x8_DCT_Q_IQ_IDCT;
    delete [] image_8x8_DCT_Q_IQ;
    delete [] image_8x8_DCT_Q;
    delete [] DCT;
    */

	return image_256x256_DCT_Q_IQ_IDCT; //return decoded image
}


//compare contigous DCT vs interleaved DCT

float* contigous(float* image)
{

    int pixel_block = 8 * 8;
    int image_size = 256 * 256;
    int image_dim = 256;
    int pixels = 8;
    int block_count = 32;
    float* DCT = new float[pixel_block];
    float* image_8x8 = new float[pixel_block];
    float* image_8x8_DCT = new float [pixel_block];
    float* image_256x256_DCT = new float [image_size];

	DCT =  DCT_matrix (pixels);

	for (int i = 0; i < block_count; i++)
    {
        for (int j = 0; j < block_count; j++)
        {
            for (int m = 0; m < pixels; m++)
            {
                for (int n =0; n < pixels; n++)
                {
                    image_8x8[pixels * m + n] = image [image_dim * (pixels * i + m)+ (pixels * j + n)];
                }

            }

                // 8x8 DCT Image
            image_8x8_DCT = DCT_Transform(image_8x8, DCT,pixels);
            for (int m = 0; m < 8; m++)
            {
                for (int n = 0; n < 8; n++)
                {
                    image_256x256_DCT[image_dim * (pixels * i + m) + (pixels * j + n)] = image_8x8_DCT [pixels * m + n];
                }
            }
        }
    }
    //store(image_256x256_DCT,"compare_DCT_contigous", 256);
    cout << "compare_contigous saved" << endl;

    /*
    delete [] image_256x256_DCT;
    delete [] image_8x8_DCT;
    delete [] image_8x8;
    delete [] DCT;
    */
    return image_256x256_DCT;

}



// compare contigous DCT vs interleaved DCT

float* interleaved(float* image)
{

    int pixel_block = 8 * 8;
    int image_size = 256 * 256;
    int image_dim = 256;
    int pixels = 8;
    int block_count = 32;

    float* DCT = new float[pixel_block];
    float* image_32x32 = new float[block_count*block_count];
    float* image_32x32_DCT = new float [block_count*block_count];
    float* image_256x256_DCT = new float [image_size];

	DCT =  DCT_matrix (block_count);

	for (int i = 0; i < pixels; i++)
    {
        for (int j = 0; j < pixels; j++)
        {
            for (int m = 0; m < block_count; m++)
            {
                for (int n =0; n < block_count; n++)
                {
                    image_32x32[block_count * m + n] = image [image_dim * (block_count * i + m)+ (block_count * j + n)];
                }

            }


            image_32x32_DCT = DCT_Transform(image_32x32, DCT,32);
            for (int m = 0; m < block_count; m++)
            {
                for (int n = 0; n < block_count; n++)
                {
                    image_256x256_DCT[image_dim * (block_count * i + m) + (block_count * j + n)] = image_32x32_DCT [block_count * m + n];
                }
            }
        }
    }
    //store(image_256x256_DCT,"compare_DCT_interleave", 256);
    cout << "compare_interleave saved" << endl;
    return image_256x256_DCT;

}





/*
// Test
//Encode function interleave
float* encode_interleave (float* image, float* Q)
{

    int pixel_block = 32 * 32;
    int image_size = 256 * 256;
    int image_dim = 256;
    int pixels = 32;
    int block_count = 8;
    float* DCT = new float[pixel_block];
    float* image_8x8 = new float[pixel_block];
    float* image_8x8_DCT = new float [pixel_block];
    float* image_8x8_DCT_Q = new float [pixel_block];
    float* image_256x256_DCT = new float [image_size];
    float* image_256x256_DCT_Q = new float [image_size];

	DCT =  DCT_matrix (pixels);

	for (int i = 0; i < block_count; i++)
    {
        for (int j = 0; j < block_count; j++)
        {
            for (int m = 0; m < pixels; m++)
            {
                for (int n =0; n < pixels; n++)
                {
                    image_8x8[pixels * m + n] = image [image_dim * (pixels * i + m)+ (pixels * j + n)];
                }

            }

                // 8x8 DCT Image
            image_8x8_DCT = DCT_Transform(image_8x8, DCT,pixels);
            for (int m = 0; m < pixels; m++)
            {
                for (int n = 0; n < pixels; n++)
                {
                    image_256x256_DCT[image_dim * (pixels * i + m) + (pixels * j + n)] = image_8x8_DCT [pixels * m + n];
                }
            }

            // Quantization

            for (int m = 0; m < pixels; m++)
            {
                for (int n = 0; n < pixels; n++)
                {
                    image_8x8_DCT_Q[pixels * m + n] = round (image_8x8_DCT[pixels * m + n] / Q[pixels * m + n]);
                    image_256x256_DCT_Q[image_dim * (pixels * i + m) + (pixels * j + n)]  = image_8x8_DCT_Q[pixels * m + n];
                }
            }

        }
    }
    store(image_256x256_DCT_Q,"image_256x256_DCT_Q_interleaved",image_dim);
    cout << "image_256x256_DCT_Q_interleaved saved" << endl;
    return image_256x256_DCT_Q;

    delete [] image_256x256_DCT_Q;
    delete [] image_256x256_DCT;
    delete [] image_8x8_DCT_Q;
    delete [] image_8x8_DCT;
    delete [] image_8x8;
    delete [] DCT;
}

*/


// decode interleave
/*
float* decode_interleave(float* image, float* Q) //received float* image_256x256_DCT_Q
{

	int pixel_block = 32 * 32;
    int image_size = 256 * 256;
    int image_dim = 256;
    int pixels = 32;
    int block_count = 8;
    float* DCT = new float[pixel_block];
    //float* image_8x8 = new float[pixel_block];
    float* image_8x8_DCT_Q = new float [pixel_block];
    float* image_8x8_DCT_Q_IQ = new float [pixel_block];
    float* image_8x8_DCT_Q_IQ_IDCT = new float [pixel_block];
    float* image_256x256_DCT_Q_IQ = new float [image_size];
    float* image_256x256_DCT_Q_IQ_IDCT = new float [image_size];



	DCT =  DCT_matrix (pixels);

	for (int i = 0; i < block_count; i++)
    {
        for (int j = 0; j < block_count; j++)
        {
            for (int m = 0; m < pixels; m++)
            {
                for (int n =0; n < pixels; n++)
                {
                    image_8x8_DCT_Q[pixels * m + n] = image [image_dim * (pixels * i + m)+ (pixels * j + n)];
                }

            }
             // IQ
            for (int m = 0; m < pixels; m++)
            {
                for (int n = 0; n < pixels; n++)
                {
                    image_8x8_DCT_Q_IQ [pixels *m + n] = image_8x8_DCT_Q[pixels*m+n] * Q[pixels *m +n];
                    //image_8x8_DCT_Q_IQ [pixels * m + n] = image_8x8[pixels * m + n]*Q[pixels * m + n];
                    image_256x256_DCT_Q_IQ[image_dim * (pixels * i + m) + (pixels * j + n)]  = image_8x8_DCT_Q_IQ[pixels * m + n];
                }
            }


            // IDCT
            image_8x8_DCT_Q_IQ_IDCT = matrix_multiplication(matrix_multiplication(transpose(DCT,pixels),image_8x8_DCT_Q_IQ,pixels),DCT,pixels);
            //image_8x8_DCT_Q_IQ_IDCT = matrix_multiplication(transpose(matrix_multiplication( image_8x8_DCT_Q_IQ, transpose(DCT,pixels),pixels),pixels), transpose(DCT,pixels),pixels);
			for (int m = 0;m < pixels;m++)
			{
				for (int n = 0;n < pixels;n++)
				{
					image_256x256_DCT_Q_IQ_IDCT[256 * (pixels * i + m) + (pixels * j + n)] = image_8x8_DCT_Q_IQ_IDCT[pixels * m + n]; // IDCT image
				}
			}

        }
    }
    store(image_256x256_DCT_Q_IQ_IDCT,"image_256x256_IDCT_interleaved",image_dim);
    cout << "image_256x256_IDCT_interleaved saved" << endl;

	return image_256x256_DCT_Q_IQ_IDCT; //return decoded image
    delete [] image_256x256_DCT_Q_IQ_IDCT;
    delete [] image_256x256_DCT_Q_IQ;
    delete [] image_8x8_DCT_Q_IQ_IDCT;
    delete [] image_8x8_DCT_Q_IQ;
    delete [] image_8x8_DCT_Q;
    delete [] DCT;
}
*/

/*
// visualize image resizing:
// original 256*256 image to 128*128
// 128*128 back to 256*256

float* resn(float* image)
{
    float* image_128x128 = new float [128*128];
    float* image256 = new float [256*256];

    for (int i=0; i < 2; i++)
    {
        for (int j =0; j< 2; j++)
        {
            for (int m=0; m< 128; m++)
            {
                for (int n =0; n< 128; n++)
                {
                    image_128x128[128 * m + n] = image [256 *(128*i+m) + (128*j+n)];
                    image256 [256 *(128*i+m) + (128*j+n)] = image_128x128[128 * m + n];

                }
            }
        }
    }
    store(image_128x128, "image_128x128", 128);
    store(image256, "image_256x256", 256);
    cout << "Image saved" << endl;
    return 0;

}

*/





int main()
{

	int image_size = 256 * 256;
	int pixel_block = 8 * 8;
	int pixels =8;

	// only used for contigous and interleaved comparison
	//int pixel_block = 32 * 32;
	//int pixels =32;

	int image_dim = 256;

	float* image_IDCT = new float[image_size];
	float* encoded_image = new float[image_size];
	float* decoded_image = new float[image_size];
	float* clipped_image = new float[image_size];
	float* compare_1 = new float[image_size];

	float Q[pixel_block] = {16,11,10,16,24,40,51,61,
	                        12,12,14,19,26,58,60,55,
	                        14,13,16,24,40,57,69,56,
	                        14,17,22,29,51,87,80,62,
	                        18,22,37,56,68,109,103,77,
	                        24,35,55,64,81,104,113,92,
	                        49,64,78,87,103,121,120,101,
	                        72,92,95,98,112,100,103,99};
	//store(Q,"8x8_Q", pixels);
	cout<< "8x8_Q saved" << endl;

	float* lena = load("lena_256x256.raw"); //load image
	image_IDCT = approximate(lena, Q);
	//store(image_IDCT,"image_IDCT", image_dim);
	cout<< "image_IDCT saved" << endl;

	clipped_image = clip(image_IDCT,image_size);
	//store(clipped_image,"image_IDCT_clipped", image_dim);
	cout<< "image_IDCT_clipped saved" << endl;



	encoded_image = encode(lena, Q);
	//store(encoded_image,"encoded_image", image_dim);
	cout<< "encoded image saved" << endl;



	decoded_image = decode(encoded_image, Q);
	//store(decoded_image,"decoded_image", image_dim);
	cout<< "decoded image saved" << endl;

	contigous(lena);
	interleaved(lena);


	//resn(lena);
	//encode_interleave(lena,Q);
	//decode_interleave(lena, Q);


	return 0;
}






