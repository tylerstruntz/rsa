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
  e = 65537;

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
      outfile << ciphertext.c_str();

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




//Reads the infile (ciphertext) block by block, converts the
// block into plaintext using decryptBlock(buffer), and prints
// the decrypted block into an outfile
/** METHOD NOT COMPLETE. Just reads text block by block so far. **/
void Rsa::decryptMessage(std::string fileName){

  //open file
  std::ifstream infile(fileName, std::ifstream::binary);
  std::cout << "Decrypting " << fileName.c_str() << std::endl;

  //open an outfile to write to
  std::ofstream outfile;
  outfile.open("decryptedMessage.txt");

  if (infile) {
    // get length of file:
    infile.seekg(0, infile.end);
    int length = infile.tellg();
    infile.seekg(0, infile.beg);

    //initialize a buffer
    InfInt *buffer= new InfInt[blockSizeE];


    //pos counter to keep track of where we are in the file
    int pos = 0;

    //while the pos is less than the length of the file, grab blocks of chars
    // from the file and decrypt them
    while (infile) {
      //pack buffer with ints from file
      infile >> buffer[0];
      infile >> buffer[1];
      infile >> buffer[2];

      //decrypt the block
      std::string plaintext = decryptBlock(buffer);
      outfile << plaintext.c_str();

    }

    std::cout << std::endl;
    infile.close();
    outfile.close();

    delete[] buffer;
  }
}




//Takes a block of 3 characters, represented by their ascii value, and converts
// each one of them into a ciphertext number. The three numbers are packed into a
// string, delimited by spaces, and returned.
std::string Rsa::encryptBlock(InfInt buffer[]) {
  InfInt output[3];
  std::stringstream ss;
  std::string space = " ";

  //encrypt text with m^e mod n, pack each result into a stringstream
  for (int i=0; i<blockSizeE; i++) {
    output[i] = modularExp(buffer[i], e, n);
    ss << output[i];
    ss << space;
  }

  std::string ciphertext = ss.str();

  return ciphertext;
}


//Takes blocks of ciphertext ints from the infile, decrypts them, and converts
// them back to their original 3 char blocks
std::string Rsa::decryptBlock(InfInt buffer[]) {
  InfInt decrypt[3];
  int asInts[3];
  char asChars[3];
  std::stringstream ss;

  //Decrypt, convert to the result to an int (which will be an ascii value),
  // and convert each ascii value into it's corresponding char. Pack chars into
  // a string to later return.
  for (int i=0; i<blockSizeE; i++) {
    decrypt[i] = modularExp(buffer[i], d, n);
    asInts[i]  = decrypt[i].toInt();
    asChars[i] = (char)asInts[i];
    if (asChars[i] != '0') //we don't care about padding
      ss << asChars[i];
  }

  std::string plaintext = ss.str();

  return plaintext;
}
