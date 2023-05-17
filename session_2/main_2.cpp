#include <iostream>
#include <fstream>
#include <random>
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
        file.write(reinterpret_cast<char*>(image), image_size * sizeof (float));
        file.close();
    }
    else cout<<"Unable to open file" <<endl;
    return 0;
}



int main()
{
     // uniform distribution
      int image_size = 256*256;
      float image_UD[image_size];
      default_random_engine random;
      uniform_real_distribution<double>uniform_D(-0.5, 0.5);
      for (int i = 0; i<image_size; i++)
      {
          image_UD[i] = uniform_D(random);
      }
      //store(image_UD, "UD.raw");
      cout << "UD image saved successfully" << endl;


      //Gaussian distribution
      float image_GD[image_size];
      normal_distribution<double> gaussian_D(0, 1);
      for (int i = 0; i < image_size; i++)
      {
          image_GD[i] = gaussian_D(random);
      }

        //store (image_GD, "GD.raw");
        cout << "GD image saved successfully" << endl;

        return 0;

}
