#include <iostream>
#include <time.h>
#include <ctime>
#include "RSA.h"




int main() {
  //initialize RSA connection
  Rsa connection;

  //start a timer
  std::clock_t start;
  double duration;
  start = std::clock();

  //encrypt a file (outputs to cipheredMessage.txt)
  connection.encryptMessage("test.txt");

  //print how long the generation took
  duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
  std::cout<< "Time taken to encrypt: " << duration << '\n';

  //restart the timer
  start = std::clock();

  connection.decryptMessage("cipheredMessage.txt");

  //print how long the generation took
  duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
  std::cout<< "Time taken to decrypt: " << duration << '\n';

  return 0;
}
