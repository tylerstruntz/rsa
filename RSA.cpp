#include "RSA.h"
#include <cstring>
#include <fstream>
#include <string>
#include <iomanip>

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

  n    = p * q;
  e    = 65537;
  sigN = (p-1)*(q-1);

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
  InfInt i=1;
  InfInt iTnP1 = i*n;
  iTnP1 = iTnP1 + 1;
  while(iTnP1 % e != 0){
    i=i+1;
    iTnP1 = i*n;
    iTnP1 = iTnP1 + 1;
  }
  iTnP1 = i*n;
  iTnP1 = iTnP1 + 1;
  d=iTnP1/e;


  InfInt temp4 = e*d;
  temp4 = temp4%n;

  std::cout << p << std::endl;
  std::cout << q << std::endl << std::endl;
  std::cout << n << std::endl << std::endl;
  std::cout << sigN << std::endl << std::endl;
  std::cout << e << std::endl;
  std::cout << d << std::endl;


  infile.close();
}
