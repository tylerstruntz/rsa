#include <iostream>
#include "RSA.h"




int main() {
  Rsa connection;

  connection.encryptMessage("test.txt");
  //std::cout << std::endl;

  connection.decryptMessage("cipheredMessage.txt");

  //std::cout << std::endl;
  return 0;
}
