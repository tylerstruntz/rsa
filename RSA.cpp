#include "RSA.h"
#include <cstring>
#include <math.h>
#include <fstream>
#include <string>
#include <iomanip>
const int  blockSize = 3;
const InfInt cubed = 26*26*26;
const InfInt squared = 26*26;


/*
//Recursive extended euclidean algorithm
InfInt xGCD(InfInt a, InfInt b, InfInt &x, InfInt &y) {
    if(b == 0) {
       x = 1;
       y = 0;
       return a;
    }

    InfInt temp = a%b;
    InfInt x1, y1, gcd = xGCD(b, temp, x1, y1);
    x = y1;

    temp = (a/b);
    temp = temp*y1;
    temp = x1 - temp;
    y = temp;
    return gcd;
}
*/

//Initializes all private members for the RSA algorithm
void Rsa::initialize() {
  std::ifstream infile;
  infile.open("primes.txt");

  infile >> p;
  infile >> q;

  n = p * q;
  e = 65537;
  sigN = (p - 1) * (q - 1);

  /*
  //// EUCLIDEAN algorithm /////
  InfInt a,b,q,x,lastx,y,lasty,temp,temp1,temp2,temp3;
  a=54;
  b=24;
  if (b>a) {//we switch them
      temp=a; a=b; b=temp;
  }

  //begin function
  x=0;
  y=1;
  lastx=1;
  lasty=0;
  while (b!=0) {
      q= a/b;
      temp1= a%b;
      a=b;
      b=temp1;

      temp2 = x;
      x=lastx-q*x;
      lastx = temp2;

      temp3 = y;
      y = lasty-q*y;
      lasty=temp3;
  }

  std::cout << "gcd= " << a << std::endl;
  std::cout << "x= " << lastx << std::endl;
  std::cout << "y= " << lasty << std::endl;
  //////////////////////////////
*/

  //Finds d in a naive way
  InfInt i = 1;
  InfInt iTnP1 = i * n;
  iTnP1 = iTnP1 + 1;
  while (iTnP1 % e != 0) {
    i = i + 1;
    iTnP1 = i * n;
    iTnP1 = iTnP1 + 1;
  }
  iTnP1 = i * n;
  iTnP1 = iTnP1 + 1;
  d = iTnP1 / e;

  InfInt temp4 = e * d;
  temp4 = temp4 % n;

  //std::cout << temp4 << std::endl;
  //std::cout << temp4 << std::endl;
  std::cout << "P: " << p << std::endl << std::endl;
  std::cout << "Q: " << q << std::endl << std::endl;
  std::cout << "N: " << n << std::endl << std::endl;
  std::cout << "sigN: " << sigN << std::endl << std::endl;
  std::cout << "e: " << e << std::endl << std::endl;
  std::cout << "d: " << d << std::endl << std::endl;

  infile.close();
}

void Rsa::breakMessageToBlock(std::string fileName)
{
  std::string s;
  

  std::ifstream infile(fileName, std::ifstream::binary);
  if (infile) {
    // get length of file:
    infile.seekg(0, infile.end);
    int length = infile.tellg();
    infile.seekg(0, infile.beg);

    //initialize a buffer
    char *buffer = new char[blockSize];
    int *bufferAsInts = new int[blockSize];

    int pos = 0;
    while (pos < length) {
      //pack buffer of chars from file
      infile.read(buffer, blockSize);

      //convert chars to ints and pack in new buffer
      for (int i = 0; i < blockSize; i++) {
        bufferAsInts[i] = (int)buffer[i];
      }

      // no need for printing it out
      // for (int i = 0; i < blockSize; i++) {
      //   std::cout << bufferAsInts[i] << " ";
      // }
      // std::cout << std::endl;

      encryptBlock(bufferAsInts);

      //move to the next block
      pos += blockSize;
      infile.seekg(pos, infile.beg);
    }

    infile.close();

    delete[] buffer;
    delete[] bufferAsInts;
  }
}

void Rsa::encryptBlock(int buffer[]) {
  char eBuffer[4];
  InfInt eMessage, tempMessage1, tempMessage2, tempMessage3, tempMessage4, encipheredTrigraph;
  for(int i = 0; i != 1; i++) {
    eMessage = ((buffer[i]) * (26*26) + (buffer[i + 1]) * (26) + (buffer[i + 2]));
  }

  /*  TODO: emessage needs to be raised to our generated e to produce our trigraph code.
            the tripgraph code then needs to do (((TGC / 26^3)%26)+ 65). this is the first quadgraph number.
            do the computation 3 more times decrementing 26^3 by 1 until the fourth time then you do ((TGC%26) +65)
            these four numbers then need to be converted to ascii characters and sent to a text file 
            for decryption
  */
}
