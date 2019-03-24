#include "RSA.h"
#include "InfInt.h"
#include <cstring>
#include <math.h>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>

//Block sizes for encryption and decryption, respectively
const int blockSizeE = 3;
const int blockSizeD = 4;

// we only are going to convert values on ascii table from 32-126
//const int asciiLowerBound = 65;
//const int asciiUpperBound = 91;
const InfInt base    = 26; // difference of 127-32
const InfInt squared = base*base;
const InfInt cubed   = squared*base;



//Recursive extended euclidean algorithm
InfInt xGCD(InfInt a, InfInt b, InfInt &x, InfInt &y) {
    if(b == 0) {
       x = 1;
       y = 0;
       return a;
    }

    InfInt temp = a%b;
    InfInt x1;
    InfInt y1;
    InfInt gcd = xGCD(b, temp, x1, y1);
    x = y1;

    temp = (a/b);
    temp = temp*y1;
    temp = x1 - temp;
    y = temp;
    return gcd;
}


//Converts an InfInt into a binary string
std::string decToBinary(InfInt num) {
    std::string binNum;
    InfInt binDigit;
    std::stringstream ss;

    //perform the conversion. this will produce the binary representation
    // and store it in a string, but using this method, the binary is in
    // reverse. We will later need to reverse the string
    while (num > 0) {
        binDigit = num % 2;
        ss << binDigit; //load the digit into a string stream
        num = num/2;
    }
    binNum.append(ss.str()); //put content from string stream into a string

    //copy the number as a string into 2 cstrings
    int length = binNum.length();
    char     numCstr[length+1];
    char numCstrCopy[length+1];
    strcpy(numCstr, binNum.c_str());

    //reverse the cstr
    for (int i=0, j=length-1; i<length; i++, j--)
      numCstrCopy[i] = numCstr[j];

    //null terminate
    numCstrCopy[length] = '\0';

    //numCstrCopy now holds the reversed string, which is the correct bin
    // converion. Load this into a string and return
    binNum = numCstrCopy;

    return binNum;
}


//Method for modular exponentiation as laid out in Appendix A of project description
InfInt modularExp(InfInt base, InfInt exponent, InfInt divisor) {
  //if the divisor is 1, the result of the operation is always 0
  if(divisor == 1)
    return (0);

  //convert the exponent into binary
  std::string exAsBin = decToBinary(exponent);

  //find number of digits in binary representation
  // and also convert the str into a cstr
  int length = exAsBin.length();
  char exAsCstr[length];
  strcpy(exAsCstr, exAsBin.c_str());

  int j=0;
  InfInt c=0, f=1;
  for(InfInt i = length-1; i >= 0; i--, j++) {
    c = (InfInt)2 * c;
    f = (f*f) % divisor;

    if (exAsCstr[j] == '1'){
      c = c+1;
      f = (f*base) % divisor;
    }
  }
  return f;
}



//////////////////////////////////////////////////////////////////////////////////////////
/////////// MEMBER FUNCTIONS /////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////


//Initializes all private members for the RSA algorithm
void Rsa::initialize() {
  std::ifstream infile;
  infile.open("primes.txt");

  //get prime values from text file
  infile >> p;
  infile >> q;

  //find n and find sigN using RSA formulas
  n = p * q;
  sigN = (p - 1) * (q - 1);


  //find d and e using extended euclidean alg
  InfInt x;
  InfInt y;

  bool foundE = false, foundD = false;
  InfInt GCD;
  e = 7333;

  while (!foundE || !foundD) {
    GCD = xGCD(sigN, e, x, y);

    if (GCD == 1) {
      foundE = true;

      d = sigN+y;
      if (d*e % sigN == 1)
        if (d < sigN)
          foundD = true;
        else
          e++;
      else
        e++;
    }
    else
      e++;
  }

  std::cout << "P: " << p << std::endl << std::endl;
  std::cout << "Q: " << q << std::endl << std::endl;
  std::cout << "N: " << n << std::endl << std::endl;
  std::cout << "sigN: " << sigN << std::endl << std::endl;
  std::cout << "e: " << e << std::endl << std::endl;
  std::cout << "d: " << d << std::endl << std::endl;

  infile.close();
}



//Reads the infile (message) block by block, converts the
// block into ciphertext using encryptBlock(buffer), and prints
// the encrypted block into an outfile
void Rsa::encryptMessage(std::string fileName){

  //open the message file
  std::ifstream infile(fileName, std::ifstream::binary);
  std::cout << "Encrypting " << fileName.c_str() << std::endl;

  //open an outfile to write to
  std::ofstream outfile;
  outfile.open("cipheredMessage.txt");

  if (infile) {
    // get length of file:
    infile.seekg(0, infile.end);
    int length = infile.tellg();
    infile.seekg(0, infile.beg);

    //initialize a buffer
    char *buffer = new char[blockSizeE];
    InfInt *bufferAsInts = new InfInt[blockSizeE];

    //find if there's going to be any null padding
    int numPadChars = length%blockSizeE;
    if (numPadChars != 0)
      numPadChars = blockSizeE - numPadChars;

    //pos counter to keep track of where we are in the file
    int pos = 0;

    //while the pos is less than the length of the file, grab blocks of chars
    // from the file and encrypt them
    while (pos < length) {
      //pack buffer of chars from file
      infile.read(buffer, blockSizeE);
      //std::cout << buffer << " ";

      //convert chars to ints and pack in new buffer
      int revI = blockSizeE;
      for (int i = 0; i < blockSizeE; i++, revI--) {
        //take account for padding
        if (pos+blockSizeE >= length){
          if (revI <= numPadChars)
            bufferAsInts[i] = 0;
          else
            bufferAsInts[i] = (int)buffer[i];
        }
        else
          bufferAsInts[i] = (int)buffer[i];
      }

      //encrypt the block and write it to the outfile
      std::string ciphertext = encryptBlock(bufferAsInts);
      std::cout << ciphertext;
      outfile << ciphertext.c_str();
      //char cipherAsCStr[blockSizeD];
      //strcpy(cipherAsCStr, ciphertext.c_str());
      //for (int i = 0; i<blockSizeD; i++)
        //outfile << cipherAsCStr[i];

      //move to the next block
      pos += blockSizeE;
      infile.seekg(pos, infile.beg);
    }

    //close files
    infile.close();
    outfile.close();

    delete[] buffer;
    delete[] bufferAsInts;
  }
  //Error reading file
  else {
    std::cout << "Couldn't open the message for encryption. Terminating." << std::endl;
    exit(-1);
  }
  std::cout << std::endl;
}




//Takes a block of 3 characters, represented by their ascii value, and converts
// it into a 4 char ciphertext string
std::string Rsa::encryptBlock(InfInt buffer[]) {
  //std::cout << "Quad: ";

  //for (int i=0; i<3; i++)
    //std::cout << buffer[i] << " ";

  //std::cout << std::endl;

  //find the trigraph code
  InfInt trigraphCode;
  InfInt temp;

  temp         = buffer[0]-65;
  trigraphCode = temp*squared;
  temp         = buffer[1]-65;
  temp         = temp*base;
  trigraphCode = trigraphCode + temp;
  temp         = buffer[2]-65;
  trigraphCode = trigraphCode + temp;

  //find the enciphered trigraph code using modular exponentiation
  InfInt encipheredCode = modularExp(trigraphCode, e, n);

  //convert into quadragraph
  InfInt quadragraph[4];
  temp = 0;

  temp           = encipheredCode/cubed;
  temp           = temp % base;
  quadragraph[0] = temp + 65;
  temp           = encipheredCode/squared;
  temp           = temp % base;
  quadragraph[1] = temp + 65;
  temp           = encipheredCode/base;
  temp           = temp % base;
  quadragraph[2] = temp + 65;
  temp           = encipheredCode % base;
  quadragraph[3] = temp + 65;


  //convert quadragraph into ints, then chars
  int  asInts[4];
  char cipher[5];
  for (int i=0; i<4; i++){
    //std::cout << quadragraph[i] << " ";
    asInts[i] = quadragraph[i].toInt();
    cipher[i] = (char)asInts[i];
  }
  cipher[4] = '\0';

  //std::cout << std::endl;
  std::string ciphertext = cipher;

  return ciphertext;
}




//Reads the infile (ciphertext) block by block, converts the
// block into plaintext using decryptBlock(buffer), and prints
// the decrypted block into an outfile
/** METHOD NOT COMPLETE. Just reads text block by block so far. **/
void Rsa::decryptMessage(std::string fileName){

  //open file
  std::ifstream infile(fileName, std::ifstream::binary);
  std::cout << "Decrypting " << fileName.c_str() << std::endl;

  if (infile) {
    // get length of file:
    infile.seekg(0, infile.end);
    int length = infile.tellg();
    infile.seekg(0, infile.beg);

    //initialize a buffer
    char *buffer = new char[blockSizeD];
    InfInt *bufferAsInts = new InfInt[blockSizeD];

    //There is no null padding since ciphertext from an encrypted message
    // is always a multiple of 4.

    //pos counter to keep track of where we are in the file
    int pos = 0;

    //while the pos is less than the length of the file, grab blocks of chars
    // from the file and decrypt them
    while (pos < length) {
      //pack buffer of chars from file
      infile.read(buffer, blockSizeD);
      std::cout << buffer << " ";

      //convert chars to ints and pack in new buffer
      for (int i = 0; i < blockSizeD; i++)
          bufferAsInts[i] = (int)buffer[i];

      /*
      for (int i=0; i<blockSizeD; i++)
        std::cout << bufferAsInts[i] << " ";
      std::cout << std::endl;
      */

      //decrypt the block
      std::string plaintext = decryptBlock(bufferAsInts);
      //std::cout << plaintext.c_str();

      //move to the next block
      pos += blockSizeD;
      infile.seekg(pos, infile.beg);
    }

    std::cout << std::endl;
    infile.close();

    delete[] buffer;
    delete[] bufferAsInts;
  }

}


std::string Rsa::decryptBlock(InfInt buffer[]) {

  //Find the code (numeric representation) of quadragraph
  InfInt code;
  InfInt temp;


  temp = buffer[0]-65;
  code = temp * cubed;
  temp = buffer[1]-65;
  temp = temp * squared;
  code = code + temp;
  temp = buffer[2]-65;
  temp = temp * base;
  code = code + temp;
  temp = buffer[3]-65;
  code = code + temp;


  //decipher the quadragraph code
  InfInt decipheredCode = modularExp(code, d, n);

  //make into a trigraph
  InfInt trigraph[3];
  temp        = decipheredCode/squared;
  trigraph[0] = temp + 65;
  temp        = decipheredCode/base;
  temp        = temp % base;
  trigraph[1] = temp + 65;
  temp        = decipheredCode%base;
  trigraph[2] = temp + 65;



  //convert trigraph into ints, then chars
  int  asInts[3];
  char cipher[4];
  for (int i=0; i<3; i++){
    //std::cout << quadragraph[i] << " ";
    asInts[i] = trigraph[i].toInt();
    cipher[i] = (char)asInts[i];
  }

  std::cout << "Trigraph: " << cipher[0] << "  " << cipher[1] << "  " << cipher[2] << std::endl;

  return "H";
}
