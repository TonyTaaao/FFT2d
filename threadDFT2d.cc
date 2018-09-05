// Threaded two-dimensional Discrete FFT transform
// Ran Tao
// ECE8893 Project 2


#include <iostream>
#include <string>
#include <math.h>

#include "Complex.h"
#include "InputImage.h"

// You will likely need global variables indicating how
// many threads there are, and a Complex* that points to the
// 2d image being transformed.
int nThreads= 16;
Complex transpose1D[1024];
int N;
Complex* ImageData;
int ImageWidth;
int ImageHeight;
//Thw mutex and condition variables allow the main thread to know when
//all helper threads are completed
pthread_mutex_t startCountMutex;
pthread_mutex_t exitMutex;
pthread_cond_t exitCond;
int startCount;
pthread_mutex_t startCountMutex1;
pthread_mutex_t exitMutex1;
pthread_cond_t exitCond1;
int startCount1;
pthread_barrier_t barrier;
Complex* H;
Complex W[512];

using namespace std;

// Function to reverse bits in an unsigned integer
// This assumes there is a global variable N that is the
// number of points in the 1D transform.
unsigned ReverseBits(unsigned v)
{ //  Provided to students
  unsigned n = N; // Size of array (which is even 2 power k value)
  unsigned r = 0; // Return value
   
  for (--n; n > 0; n >>= 1)
    {
      r <<= 1;        // Shift return value
      r |= (v & 0x1); // Merge in next bit
      v >>= 1;        // Shift reversal value
    }
  return r;
}

// GRAD Students implement the following 2 functions.
// Undergrads can use the built-in barriers in pthreads.

// Call MyBarrier_Init once in main
void MyBarrier_Init()// you will likely need some parameters)
{
}

// Each thread calls MyBarrier after completing the row-wise DFT
void MyBarrier() // Again likely need parameters
{
}
                    
void Transform1D(Complex* h, int N)
{
  // Implement the efficient Danielson-Lanczos DFT here.
  // "h" is an input/output parameter
  // "N" is the size of the array (assume even power of 2)
  //Step 1. Reordering h
  //Complex* temph = h;
  Complex temph[N];
  for (int i = 0; i < N; i++) {
    temph[i] = h[i];
  }
  for (int i = 0; i < N; i++) {
    int index = (int) ReverseBits(i);
    //unsigned index = ReverseBits(i);
    h[index] = temph[i];
  }
  //reassign temph to be temporary h
  for (int i = 0; i < N; i++) {
    temph[i] = h[i];
  }
  //Step 2. Precomputing W[n]
  //should be placed outside
  
  //Step 3. Do the transformation
  //Complex* tempH = h; //temporary buffer to store the initial values of h
  //int np = 2;
  for (int n = 2; n <= N; n *= 2) {
    //Complex* tempH = h; //temporary buffer to store the initial values of h
    for (int i = 0; i < N; i += n) {
      
      for (int k = 0; k < n/2; k++) {
        //h[i+k] = tempH[(k % (n/2))] + W[k*N/n] * tempH[n/2 + (k%(n/2))];
        //h[i+k+n/2] = tempH[(k % (n/2))] - W[k*N/n] * tempH[n/2 + (k%(n/2))];

        h[i+k] = temph[i + (k % (n/2))] + W[k*N/n] * temph[i + n/2 + (k%(n/2))];
        h[i+k+n/2] = temph[i + (k % (n/2))] - W[k*N/n] * temph[i + n/2 + (k%(n/2))];
        temph[i+k] = h[i + k];
        temph[i+k+n/2] = h[i+k+n/2];
      }
      
    }
    
  }

} 


void* Transform2DTHread(void* v)
{ // This is the thread startign point.  "v" is the thread number
  unsigned long myId = (unsigned long) v; //myId is the thread number
  int rowsPerThread = ImageHeight / nThreads;
  int startingRow = myId * rowsPerThread;
  // Calculate 1d DFT for assigned rows
  for (int r = 0; r < rowsPerThread; r++) {
    int thisRow = startingRow + r;
    Transform1D((Complex*)&ImageData[thisRow*ImageWidth], N);
  }
  // wait for all to complete
  pthread_mutex_lock(&startCountMutex);
  startCount--;
  if (startCount == 0) {
    //Last to exit, notify main
    pthread_mutex_unlock(&startCountMutex);
    pthread_mutex_lock(&exitMutex);
    pthread_cond_signal(&exitCond);
    pthread_mutex_unlock(&exitMutex);
  }
  else {
    pthread_mutex_unlock(&startCountMutex);
  }
  //Tranverse the data matrix
//  for (int i = startingRow; i < startingRow + rowsPerThread; i++) {
//    for (int j = 0; j < ImageWidth; j++) {
//      transpose1D[j*ImageHeight + i] = ImageData[i*ImageWidth + j];
//    }
//  }
  //wait for all threads to finish transpose
//  pthread_mutex_lock(&startCountMutex);
//  startCount++;
//  if (startCount == nThreads) {
    //Last to exit, notify main
//    pthread_mutex_unlock(&startCountMutex);
//    pthread_mutex_lock(&exitMutex);
//    pthread_cond_signal(&exitCond);
//    pthread_mutex_unlock(&exitMutex);
//  }
//  else {
//    pthread_mutex_unlock(&startCountMutex);
//  } 
  // Calculate 1d DFT for assigned columns
  //for (int r = 0; r < rowsPerThread; r++) {
  //  int thisRow = startingRow + r;
  //  Transform1D((Complex*)&transpose1D[thisRow*ImageWidth], N);
  //}
  // wait for all to complete
  //pthread_mutex_lock(&startCountMutex);
  //startCount--;
  //if (startCount == 0) {
  //Last to exit, notify main
  //  pthread_mutex_unlock(&startCountMutex);
  //  pthread_mutex_lock(&exitMutex);
  //  pthread_cond_signal(&exitCond);
  //  pthread_mutex_unlock(&exitMutex);
  //}
  //else {
  //  pthread_mutex_unlock(&startCountMutex);
  //}
  //Tranverse back
  //for (int i = startingRow; i < startingRow + rowsPerThread; i++) {
  //  for (int j = 0; j < ImageWidth; j++) {
  //    ImageData[j*ImageHeight + i] = transpose1D[i*ImageWidth + j];
  // }
  //}
  //wait for all threads to finish transpose
  //pthread_mutex_lock(&startCountMutex);
  //startCount++;
  //if (startCount == nThreads) {
    //Last to exit, notify main
  //  pthread_mutex_unlock(&startCountMutex);
  //  pthread_mutex_lock(&exitMutex);
  //  pthread_cond_signal(&exitCond);
  //  pthread_mutex_unlock(&exitMutex);
  //}
  //else {
  //  pthread_mutex_unlock(&startCountMutex);
  //}
                                                                  
  // Decrement active c unt and signal main if all complete
  return 0;
}

void* Transpose(void* v)
{ // This is the thread startign point.  "v" is the thread number
   unsigned long myId = (unsigned long) v; //myId is the thread number
   int rowsPerThread = ImageHeight / nThreads;
   int startingRow = myId * rowsPerThread;
   // Calculate 1d DFT for assigned rows
   for (int i = startingRow; i < startingRow + rowsPerThread; i++) {
     for (int j = 0; j < ImageWidth; j++) {
       transpose1D[j*ImageHeight + i] = ImageData[i*ImageWidth + j];
     }
   }
   // wait for all to complete
   pthread_mutex_lock(&startCountMutex1);
   startCount--;
   if (startCount == 0) {
     //Last to exit, notify main
     pthread_mutex_unlock(&startCountMutex1);
     pthread_mutex_lock(&exitMutex1);
     pthread_cond_signal(&exitCond1);
     pthread_mutex_unlock(&exitMutex1);
   }
   else {
     pthread_mutex_unlock(&startCountMutex1);
   }
   return 0;
}


void* Transform2DTHread2(void* v)
 { // This is the thread startign point.  "v" is the thread number
   unsigned long myId = (unsigned long) v; //myId is the thread number
   int rowsPerThread = ImageHeight / nThreads;
   int startingRow = myId * rowsPerThread;
   // Calculate 1d DFT for assigned rows
   for (int r = 0; r < rowsPerThread; r++) {
     int thisRow = startingRow + r;
     Transform1D((Complex*)&ImageData[thisRow*ImageWidth], N);
   }
   // wait for all to complete
   pthread_mutex_lock(&startCountMutex);
   startCount--;
   if (startCount == 0) {
     //Last to exit, notify main
     pthread_mutex_unlock(&startCountMutex);
     pthread_mutex_lock(&exitMutex);
     pthread_cond_signal(&exitCond);
     pthread_mutex_unlock(&exitMutex);
   }
   else {
     pthread_mutex_unlock(&startCountMutex);
   }
   return 0;
}

void Transform2D(const char* inputFN) 
{ // Do the 2D transform here.
  InputImage image(inputFN);  // Create the helper object for reading the image
  ImageData = image.GetImageData();
  ImageWidth = image.GetWidth();
  ImageHeight = image.GetHeight();
  N = ImageWidth;
  startCount = nThreads; //Total threads (to be) started
  //Initialize W (weight matrix)
  for (int n = 0; n < 512; n++) {
    W[n] = Complex(cos(2.0*M_PI*n/N), -sin(2.0*M_PI*n/N));
    if (fabs(W[n].imag) < 1e-10) W[n].imag = 0;
    if (fabs(W[n].real) < 1e-10) W[n].real = 0;
  }
  //All mutex and condition variables must be "initialized"
  pthread_mutex_init(&exitMutex,0);
  pthread_mutex_init(&startCountMutex,0);
  pthread_cond_init(&exitCond,0);
  //Main holds the exit mutex until waiting for the exitCond condition
  pthread_mutex_lock(&exitMutex);
  // Create the global pointer to the image array data
  // Create 16 threads
  for (int i = 0; i<nThreads;i++)
  {
    //Create the thread
    pthread_t pt;
    pthread_create(&pt,0,Transform2DTHread,(void*)i);
  }
  //Main program now wait until all child threads completed
  pthread_cond_wait(&exitCond, &exitMutex);
  //Test 1D Transform
  char fileName[] = "MyAfter1D.txt";
  image.SaveImageData((char*)&fileName, ImageData, ImageWidth, ImageHeight);
  char fileName2[] = "MyAfter2D.txt";
  image.SaveImageData((char*)&fileName2, ImageData, ImageWidth, ImageHeight);
  
  //pthread_mutex_init(&exitMutex1,0);
  //pthread_mutex_init(&startCountMutex1,0);
  //pthread_cond_init(&exitCond1,0);
  //Main holds the exit mutex until waiting for the exitCond condition
  //  pthread_mutex_lock(&exitMutex);
  //Transpose the data
  //startCount = nThreads;
  //for (int i = 0; i<nThreads;i++)
 // {//Create the thread
  //  pthread_t pt;
  //  pthread_create(&pt,0,Transpose,(void*)i);
  //}
  //pthread_cond_wait(&exitCond1, &exitMutex1);
  //Transpose the data
//  for (int i = 0; i < ImageHeight; i++){
//    for (int j = 0; j < ImageWidth; j++) {
//      transpose1D[j*ImageHeight + i] = ImageData[i*ImageWidth + j];
//    }
//  }
//  for (int i = 0; i < ImageHeight* ImageWidth; i++) {
//    ImageData[i] = transpose1D[i];
//  }
  //Transform columns
  //pthread_mutex_init(&exitMutex,0);
  //pthread_mutex_init(&startCountMutex,0);
  //pthread_cond_init(&exitCond, 0);
//  pthread_mutex_lock(&exitMutex);
//  startCount = nThreads;
//  for (int i = 0; i < nThreads; ++i) {
    // Now create the thread
//    pthread_t pt; // pThread variable (output param from create)
    // Third param is the thread starting function
    // Fourth param is passed to the thread starting function
//    pthread_create(&pt, 0, Transform2DTHread, (void*)i);
//  }
//  pthread_cond_wait(&exitCond, &exitMutex);  
  // Wait for all threads complete
  //uncomment next line
  //pthread_cond_wait(&exitCond, &exitMutex);
  // Write the transformed data
}

int main(int argc, char** argv)
{
  string fn("Tower.txt"); // default file name
  if (argc > 1) fn = string(argv[1]);  // if name specified on cmd line
  // MPI initialization here
  
  Transform2D(fn.c_str()); // Perform the transform.
}  

