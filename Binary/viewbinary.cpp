

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <sstream>
#include <vector>

using namespace std;

ifstream midiFile;
ofstream textFile;


int main() {

    char infilename[50];
    char outfilename[50];

    cout << "Please tell me the name of the MIDI." << endl;
    cin >> infilename;

    midiFile.open(infilename);

    while (!(midiFile.is_open())) {
    cout << "INPUT FILE NOT FOUND. PLEASE TRY AGAIN." << endl;
    cin >> infilename;
    midiFile.open(infilename);
    }
    cout << "Input File Opened Successfully." << endl;

    cout << "Please name the output file." << endl;
    cin >> outfilename;

    textFile.open(outfilename);
    if (textFile.is_open()){
    cout << "Output File Created." << endl;
    }

    unsigned char bitChar;
    short bitNum;

    while (!(midiFile.eof())){
        bitChar=midiFile.get();
        bitNum=bitChar;
    textFile << setw(6) << hex << bitNum << endl;
    }
    midiFile.close();
    textFile.close();
}