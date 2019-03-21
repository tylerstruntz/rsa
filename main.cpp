#include <iostream>
#include "RSA.h"

int main() {
  Rsa connection;

  connection.breakMessageToBlock("ranText.txt");

  return 0;
}
