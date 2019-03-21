#include <iostream>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <ctime>
#include <fstream>
#include "InfInt.h"
#include "mersennetwister.h"

//Prime tests
bool primeQuickTest(InfInt num);
bool millerRabin(InfInt num, int passes);

//Generation methods
int generateSeed();
InfInt generateGuess();
InfInt generatePrime();

//Helper Functions
InfInt power(InfInt base, InfInt power);
InfInt modulo(InfInt base, InfInt exponent, InfInt mod);
InfInt mulmod(InfInt a, InfInt b, InfInt mod);




int main() {
  //seed rand()
  srand(time(NULL));

  //start a timer
  std::clock_t start;
  double duration;
  start = std::clock();

  //find two large primes
  InfInt p = generatePrime();
  InfInt q = generatePrime();

  //print how long the generation took
  duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
  std::cout<< "Time taken: " << duration << '\n';

  //print the primes to console
  std::cout << p << std::endl;
  std::cout << q << std::endl;

  //print the primes to a text file
  std::ofstream outfile;
  outfile.open("primes.txt");
  outfile << p << std::endl;
  outfile << q << std::endl;

  return 0;
}

////////////////////////////////////////////////////////////////////////
///////////////// Prime Tests //////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

  //Quickly tests whether a given number is prime
  bool primeQuickTest(InfInt num){
   //convert int to string
   std::string numString = num.toString();

   //convert string to char array
   char numArray[numString.length()];
   strcpy(numArray, numString.c_str());

   //get the last digit in the char array as an int
   int lastDigit = numArray[numString.length()-1] - '0';

   //check the last digit
   if (lastDigit == 0 || lastDigit == 2 || lastDigit == 4 || lastDigit == 5 || lastDigit == 6 || lastDigit == 8)
     return false;

   //perform 6n+1 and 6n-1 test
   bool plusOne  = false;
   bool minusOne = false;

   num++;

   if (num % 6 == 0)
     plusOne = true;

   num--;
   num--;

   if (num % 6 == 0)
     minusOne = true;

   return ((plusOne || minusOne) ? true : false);
  }


  //Miller-Rabin test for primality.
  bool millerRabin(InfInt num, int passes){
    InfInt s = num-1;
    InfInt numMinusOne = s;
    while (s % 2 == 0){s /= 2;}

    for (int i = 0; i < passes; i++){

      InfInt a = (InfInt)generateSeed();
      InfInt temp = s;
      InfInt mod = modulo(a, temp, num);

      while (temp != numMinusOne && mod != 1 && mod != numMinusOne){
        mod = mulmod(mod, mod, num);
        temp *= 2;
      }

      if (mod != numMinusOne && temp % 2 == 0) return false;
    }

    return true;
  }

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

/**********************************************************************/

////////////////////////////////////////////////////////////////////////
//////////////// Generation Methods ////////////////////////////////////
////////////////////////////////////////////////////////////////////////

  //Generates a random seed to use in the mersenne twister
  int generateSeed() {
    //create a cstr to hold digits of random number
    char s[9];

    //generate a random number (0-10) for each digit. If the number
    // is 10, throw it out. The first number cannot be 0.
    // Gives us varying lengths of values
    int charCount=0;
    for (int i=0; i<8; i++){
      int r = rand() % 11;
      while (charCount == 0 && r == 0)
        r = rand() % 11;

      if (r != 10) {
        s[charCount] = r + '0';
        charCount++;
      }
    }

    //convert to an int and return
    int num = atoi(s);
    return num;
  }


  //Generates a large random number
  InfInt generateGuess() {
    InfInt guess;
    int seed = generateSeed();

    //initialize the mersenne twister
    MtRng32 mt;
    mt.init(seed);

    //find a large, random number with an equation and smaller random numbers
    InfInt randomNums[13];
    for (int i=0; i<13; i++) randomNums[i] = (InfInt)mt.getUint();
    guess = power(randomNums[1], 4);
    guess = guess + randomNums[3];
    guess = power(guess, 2);
    guess = guess * randomNums[4];
    guess = guess + randomNums[0];
    guess = guess * randomNums[2];
    guess = guess + randomNums[5];
    guess = guess * randomNums[6];
    guess = guess + randomNums[7];
    guess = guess * randomNums[8];
    guess = guess + randomNums[9];
    guess = guess * randomNums[10];
    guess = guess / 2;
    guess = guess + randomNums[11];
    guess = guess * randomNums[12];

    if (guess % 2 == 0)
      guess = guess + 1;

    return guess;
  }


  //Generates a prime number, using a quick test to quickly
  // determine if a number could be prime, then verifies it
  // with the Miller-Rabin test for primality.
  InfInt generatePrime() {

    //Generate a large number
    InfInt primeNum = generateGuess();
    bool prime = false;


    //increment 2 until the number is prime
    while (!prime) {
      while (!primeQuickTest(primeNum))
        primeNum += 1;

        if (!millerRabin(primeNum, 40)) {
          primeNum += 2;
        }
        else{
          prime = true;
        }
    }

    //return the prime
    return primeNum;
  }

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

/**********************************************************************/

////////////////////////////////////////////////////////////////////////
/////////////////// Helper Functions ///////////////////////////////////
////////////////////////////////////////////////////////////////////////

  //power function for InfInts
  InfInt power(InfInt base, InfInt power){
    InfInt lastNum = base, curNum = base;
    for (InfInt i=1; i < power; i++) {
      curNum = lastNum * base;
      lastNum = curNum;
    }
    return curNum;
  }


  InfInt mulmod(InfInt a, InfInt b, InfInt mod){
      InfInt x = 0,y = a % mod;
      while (b > 0)
      {
          if (b % 2 == 1)
          {
              x = (x + y) % mod;
          }
          y = (y * 2) % mod;
          b /= 2;
      }
      return x % mod;
  }
  /*
   * modular exponentiation
   */
  InfInt modulo(InfInt base, InfInt exponent, InfInt mod){
      InfInt x = 1;
      InfInt y = base;
      while (exponent > 0)
      {
          if (exponent % 2 == 1)
              x = (x * y) % mod;
          y = (y * y) % mod;
          exponent = exponent / 2;
      }
      return x % mod;
  }

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
