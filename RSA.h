#ifndef RSA
#define RSA

#include <cstdlib>
#include <iostream>
#include "InfInt.h"


class Rsa{

  public:
    //Constructor
    Rsa(){initialize();};

    //Member functions
    void initialize(); //initializes all private variables


    //InfInt encrypt(InfInt plaintext);
    //InfInt decrypt(InfInt ciphertext);


  private:

    //Private fields
    InfInt p;
    InfInt q;
    InfInt n;
    InfInt sigN;
    InfInt e, d;
};
#endif
