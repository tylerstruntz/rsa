#ifndef RSA
#define RSA

#include <cstdlib>
#include <iostream>
#include "InfInt.h"

class Rsa {

public:
  //Constructor
  Rsa() { initialize(); };

  //Member functions
  void initialize(); //initializes all private variables

  void encryptMessage(std::string fileName);
  void decryptMessage(std::string fileName);

  std::string decryptBlock(InfInt buffer[]);
  std::string encryptBlock(InfInt buffer[]);

private:
  //Private fields
  InfInt p;
  InfInt q;
  InfInt n;
  InfInt sigN;
  InfInt e, d;

};
#endif
