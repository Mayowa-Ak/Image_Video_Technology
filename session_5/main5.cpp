#include <fstream>
#include <iostream>
#include <cmath>
#include <vector>
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
         //delete [] memory_block;
         return (float*) memory_block;

     }
     else cout << file_name << " can not be found." << endl;
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


 //Encode function
float* create_32x32_image_DCT_Q(float* image, float* Q)
{

    int pixel_block = 8 * 8;
    int image_size = 256 * 256;
    int image_dim = 256;
    int pixels = 8;
    int block_count = 32;
    int new_size = 32*32;
    float* DCT = new float[pixel_block];
    float* image_8x8 = new float[pixel_block];
    float* image_8x8_DCT = new float [pixel_block];
    float* image_32x32_DCT_Q = new float [new_size];

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
            			//Quantization
			image_32x32_DCT_Q[32*i+j] = round(image_8x8_DCT[0] / Q[0]); //DC term
		}
	}
	//delete [] DCT;
	//delete [] image_8x8;
	//delete [] image_8x8_DCT;
	//delete [] image_32x32_DCT_Q;
	return image_32x32_DCT_Q;
}

//Text file successive differences
void txt_file_differences(float* image, string name)
{
    int block_count = 32;
    int new_size = block_count*block_count;
	int* differences = new int[new_size];
	for (int i = 0; i < new_size; i++)
    {
		if (i == 0)
			differences[i] = (int)image[i];
		else
			differences[i] = (int)image[i] - (int)image[i - 1];
	}
    ofstream file;
    file.open(name);
	for (int i = 0; i < new_size; i++)

        file << differences[i] << " ";
        file.close();
        //delete [] differences;
}


//reconstruct image
float* reconstruct_image (int* differences)
{
    int new_size = 32*32;
	float* image_reconstructed = new float[new_size];
	for (int i = 0;i < new_size; i++)
        {
		if (i == 0)
			image_reconstructed[i] = (float)differences[i];
		else
			image_reconstructed[i] = (float)differences[i] + image_reconstructed[i - 1];
	}
	//delete []image_reconstructed ;
	return image_reconstructed;
}

//Read text file
int* read_file_txt(string name)
{
	int* differences = new int[32 * 32];
	int counter = 0;
	ifstream file;
	file.open(name);

	//read the numbers to the array
	while (counter <32 * 32 && file >> differences[counter]) {
		counter ++;
	}
	file.close();
	//delete differences[];
	return differences;
}


//RL ordering zigzag method
float* RL_ordering(float* matrix, int length) // accepts matrix parameter and returns RL_ordered matrix
{

	float* ordered_matrix = new float[length * length];
	int i =0, j=0;
    if( i < length && j < length)
	for (int x = 0; x < length; x++)

	    for (int y = 0; y < length; y++)
            {

			ordered_matrix[length * x + y] = matrix[length * i + j]; // input i,j: output x,y

			// (1,j):(row,column)  j is @ even column and i at first or last row, move right

			if ((i == length - 1 || i == 0) && j % 2 == 0)
            {
				j++;
				continue;
			}



			// i is odd and j =0 or last column, move down

			if ((j == 0 || j == length - 1) && i % 2 == 1)
			{
			    i++;
			    continue;
			}


			// i + j = even, move up and right
			if ((i + j) % 2 == 0)
            {
				i--;  // move up
				j++;  // move right
			}

			// i +j = odd, move down, left
			else if ((i + j) % 2 == 1)
            {
				i++;
				j--;
			}
		}

    //delete [] ordered_matrix;
	return ordered_matrix;
}

// Encoding run length
vector<float> RL_encoding(float* matrix, int length) // accepts RL_ordered matrix as parameter and returns encoded values 13 0
{
	int counter = 1;
	vector<float> encode_RL;
	for (int i = 0; i < length; i++)
	{
		counter = 1; // count first value

		while ((i < length - 1) && matrix[i] == matrix[i + 1]) // check if current value is the same as the next
        {
		counter++; // counter for repeated values
		i++; //compare the next element
		}
		encode_RL.push_back(counter); // counter values saved
		encode_RL.push_back(matrix[i]); // Actual values saved
	}

	return encode_RL;
}

// Decoding RL
vector<float> RL_decoding(vector<float> matrix)// accepts encoded values and decodes them 00000000000000
{
	int counter;
	vector<float> decode_RL;
	for (int i = 0; i < matrix.size(); i++)
    {
		if (i % 2 == 0)  // counter saved at every even index
		{
			counter = matrix[i];  //get counter value at every even index
			while (counter > 0)
            {
				decode_RL.push_back(matrix[i + 1]); // save the value after counter (@ odd index)
				counter--;                           // decrement counter
			}
		}
	}
	return decode_RL;
}

//inverse zig zag RL ordering
float* iRL_ordering(float* matrix, int length)// accepts decoded values and returns original matrix
{
	int i = 0, j = 0;
	float* inverse_matrix = new float[length * length];
	if (i < length && j < length)
		for (int x = 0; x < length; x++)
        {
			for (int y = 0; y < length; y++)
            {

				inverse_matrix[length * i + j] = matrix[length * x + y];

				if ((i == length - 1 || i == 0) && j % 2 == 0)
                {
					j++;
					continue;
				}

				if ((j == 0 || j == length - 1) && i % 2 == 1)
                {
					i++;
					continue;
				}

				if ((i + j) % 2 == 0)
                {
					i--;
					j++;
				}
				else if ((i + j) % 2 == 1)
				{
					i++;
					j--;
				}
			}
        }

    //delete []inverse_matrix;
	return inverse_matrix;
}


float* AC_terms(float* image, float* Q)
{

    int pixel_block = 8 * 8;
    int image_size = 256 * 256;
    int image_dim = 256;
    int pixels = 8;
    int block_count = 32;
    //int pixels_per_block = 64;
    int ac_pixels_per_block =63;
    float* DCT = new float[pixel_block];
    float* image_8x8 = new float[pixel_block];
    float* image_8x8_DCT = new float [pixel_block];
    float* image_8x8_DCT_Q = new float [pixel_block];
    float* image_256x256_DCT = new float [image_size];
    float* ac_values = new float [image_size - block_count*block_count]; //65536-1024= 64512 = 32*32*63


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
            for (int m=0; m < pixels; m++)
            {
                for (int n = 0; n < pixels; n++)
                {
                    image_256x256_DCT[image_dim * (pixels * i+ m)+ (pixels * j + n)] = image_8x8_DCT[pixels * m + n];
                }
            }
            // Apply Q

            for (int m = 0; m < pixels; m++)
            {
                for (int n = 0; n < pixels; n++)
                {
                    image_8x8_DCT_Q[pixels * m + n] = round (image_8x8_DCT[pixels * m + n]/Q[pixels * m + n]);

                }
            }

            for (int v = 0; v < ac_pixels_per_block; v++)
                ac_values[63*(block_count * i + j)+ (v)] = image_8x8_DCT_Q[v + 1]; //63*1023+63 = 64512


        }
    }

    /*

    delete [] image_256x256_DCT;
    delete [] image_8x8_DCT_Q;
    delete [] image_8x8_DCT;
    delete [] image_8x8;
    delete [] DCT;
    */

    return ac_values;
}



float* get_AC_DC_terms(float* image, float* Q)
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
    //float* ac_values = new float [image_size];

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
            for (int m=0; m < pixels; m++)
            {
                for (int n = 0; n < pixels; n++)
                {
                    image_256x256_DCT[image_dim * (pixels * i+ m)+ (pixels * j + n)] = image_8x8_DCT[pixels * m + n];
                }
            }
            // Apply Q

            for (int m = 0; m < pixels; m++)
            {
                for (int n = 0; n < pixels; n++)
                {
                    image_8x8_DCT_Q[pixels * m + n] = round (image_8x8_DCT[pixels * m + n]/Q[pixels * m + n]);
                    image_256x256_DCT_Q [image_dim * ((pixels * i)+ m)+ (pixels * j + n)] = image_8x8_DCT_Q[pixels * m + n];
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


vector<float> RL_encoding_ac_terms(float* ac_terms)
{

    int pixel_block = 8 * 8;
    int image_size = 256 * 256;
    int image_dim = 256;
    int pixels = 8;
    int pixels_per_block = 64;
    int ac_pixels_per_block =63;
    int block_count = 32;

	float* zigzag_all_ac = new float[image_size - block_count*block_count];
	float* zigzag_ac_dc_64 = new float[pixels_per_block];
	float* zigzag_ac_63 = new float[ac_pixels_per_block];
	float* ac_dc_before_zigzag_64 = new float[pixels_per_block];
	vector<float> results_encoded_RL_ac_terms;
	vector<float> encoded_RL_ac_terms;

	for (int i = 0;i < block_count; i++)
    {
		for (int j = 0;j < block_count; j++)
		{
			for (int m = 0;m < pixels; m++)
			{
				for (int n = 0;n < pixels; n++)
				{
					ac_dc_before_zigzag_64[pixels * m + n] = ac_terms[image_dim * ((pixels * i) + m) + (pixels * j + n)];
				}
			}

			zigzag_ac_dc_64 = RL_ordering(ac_dc_before_zigzag_64, pixels);
			for (int k = 1; k < pixels_per_block; k++) // 1 -63
            zigzag_ac_63[k-1] = zigzag_ac_dc_64[k]; // 0 - 62 index AC terms // DC term removed
			encoded_RL_ac_terms = RL_encoding(zigzag_ac_63, ac_pixels_per_block);



			for (int m = 0; m < encoded_RL_ac_terms.size(); m++)
            {
				results_encoded_RL_ac_terms.push_back(encoded_RL_ac_terms[m]);
			}
		}
	}
	return results_encoded_RL_ac_terms;
}


float* RL_decoding_ac_terms(vector<float> encoded_ac_results)
{

    int pixel_block = 8 * 8;
    int image_size = 256 * 256;
    int image_dim = 256;
    int pixels = 8;
    int pixels_per_block = 64;
    int ac_pixels_per_block = 63;
    int block_count = 32;

	float* ac_terms_iRL_8x8 = new float[pixel_block];
	float* ac_terms_iRL = new float[image_size];
	float* all_ac_terms = new float[image_size - block_count*block_count];
	float* ac_terms_8x8 = new float[pixel_block];

	vector<float> decoded_ac_terms;
	decoded_ac_terms = RL_decoding(encoded_ac_results);
	float* p_decoded_ac = new float[decoded_ac_terms.size()];
    p_decoded_ac= &decoded_ac_terms[0];

	for (int i = 0;i < block_count;i++)
    {
		for (int j = 0;j < block_count; j++)
    {
			for (int v = 0; v < ac_pixels_per_block ;v++)
        {
				ac_terms_8x8[0] = 0; // DC term 0
				ac_terms_8x8[v+1] = p_decoded_ac[64 *(block_count * i + j)  + v];
				ac_terms_iRL_8x8 = iRL_ordering(ac_terms_8x8, pixels);
			}
			for (int v = 0;v < ac_pixels_per_block; v++)

			    all_ac_terms[63*(block_count * i + j)+ (v)] = ac_terms_iRL_8x8[v + 1];




		}
	}
	return all_ac_terms;
}


void file_txt(float* matrix, int size_file_txt, string name)
{
	ofstream file(name);
	for (int i = 0; i < size_file_txt; i++)
		file << matrix[i] << " ";
	file.close();
}

//---------------------------------------------------------------------
int main()
{

	int image_size = 256 * 256;
	int pixel_block = 8 * 8;
	int pixels =8;
	int block_count = 32;
	float* lena = load("lena_256x256.raw");
	float Q[pixel_block] = {16,11,10,16,24,40,51,61,
	                        12,12,14,19,26,58,60,55,
	                        14,13,16,24,40,57,69,56,
	                        14,17,22,29,51,87,80,62,
	                        18,22,37,56,68,109,103,77,
	                        24,35,55,64,81,104,113,92,
	                        49,64,78,87,103,121,120,101,
	                        72,92,95,98,112,100,103,99};





    // image_32x32_DCT_Q
    float* image_32x32_DCT_Q = new float[block_count * block_count];
	image_32x32_DCT_Q = create_32x32_image_DCT_Q(lena, Q);
	//store(image_32x32_DCT_Q, "image_32x32_DCT_Q", block_count);
	cout << "image_32x32_DCT_Q saved " << endl;

	// Text file differences
	//txt_file_differences(image_32x32_DCT_Q,"Q_DCT_differences.txt");

	// Read text file for image reconstruction
	int* file_differences = new int[block_count * block_count];
	//file_differences = read_file_txt("Q_DCT_differences.txt");

	// image reconstruction
	float* image_reconstruction = new float[block_count * block_count];
	image_reconstruction = reconstruct_image(file_differences);
	//store(image_reconstruction, "reconstructed_image", block_count);



	// ac, xxxdcxxx
	float* all_ac_dc_values = new float[image_size]; //ac and dc
	float* only_ac_values = new float[image_size - block_count * block_count]; //ac only
	vector<float> ac_encoded;
	float* ac_decoded = new float[image_size];

	only_ac_values = AC_terms(lena,Q);
	all_ac_dc_values = get_AC_DC_terms(lena, Q);
	//file_txt(only_ac_values, (image_size - block_count * block_count),"only_ac_values.txt");

	ac_encoded = RL_encoding_ac_terms(all_ac_dc_values);
	float* a = new float[ac_encoded.size()];
	a = &ac_encoded[0];
	//file_txt(a, ac_encoded.size(),"ac_values_encoded.txt");

	ac_decoded = RL_decoding_ac_terms(ac_encoded);
	//file_txt(ac_decoded, image_size - block_count * block_count,"ac_values_decoded.txt");

	return 0;
}

