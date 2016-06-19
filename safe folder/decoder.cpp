// My new MIDI decoder program

#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <sstream>
#include <vector>

using namespace std;

bool tofile=true;

const int vol=7;

bool timeDivision=false;
int space=6;
string type;
int typeLiteral;
string keySignature;
int totalTracks;
int clicksPerBeat=0,framesPerSecond=0,clicksPerFrame=0,clicksPerSecond=0;
int timeSigNum=4,timeSigDen=4,timeSigMet=24,timeSig32s=8;
int doneTracks=0;
double tempo=120;                //default is 120 if not specified otherwise
long lineNumber=0;
short prevType=0;

char midiName[120];
char textName[120];
char noterName[120];

unsigned long totalTime=0,deltaTime=0,noteTime=0;
unsigned long delta0Time=0,delta1Time=0,delta2Time=0,delta3Time=0,delta4Time=0,
delta5Time=0,delta6Time=0,delta7Time=0,delta8Time=0,delta9Time=0,delta10Time=0,
delta11Time=0,delta12Time=0,delta13Time=0,delta14Time=0,delta15Time=0;
string instrument0,instrument1,instrument2,instrument3,instrument4,instrument5,
instrument6,instrument7,instrument8,instrument9,instrument10,instrument11,instrument12,
instrument13,instrument14,instrument15;

int highest0=0,highest1=0,highest2=0,highest3=0,highest4=0,highest5=0,highest6=0,highest7=0,highest8=0,
highest10=0,highest11=0,highest12=0,highest13=0,highest14=0,highest15=0;
int lowest0=127,lowest1=127,lowest2=127,lowest3=127,lowest4=127,lowest5=127,lowest6=127,lowest7=127,lowest8=127,
lowest10=127,lowest11=127,lowest12=127,lowest13=127,lowest14=127,lowest15=127;

string notary[128];
string soundSet[128]={"acoustic grand piano","bright acoustic piano","electric grand piano","honky-tonk piano","electric piano 1",
"electric piano 2","harpsichord","clavi","celesta","glockenspiel",
"music box","vibraphone","marimba","xylophone","tubular bells",
"dulcimer","drawbar organ","percussive organ","rock organ","church organ",
"reed organ","accordion","harmonica","tango accordion","acoustic guitar (nylon)",
"acoustic guitar (steel)","electric guitar (jazz)","electric guitar (clean)","electric guitar (muted)","overdriven guitar",
"distortion guitar","guitar harmonics","acoustic bass","electric bass (finger)","electric bass (pick)",
"fretless bass","slap bass 1","slap bass 2","synth bass 1","synth bass 2",
"violin","viola","cello","contrabass","tremolo strings",
"pizzicato strings","orchestral harp","timpani","string ensemble 1","string ensemble 2",
"synthstrings 1","synthstrings 2","choir aahs","voice oohs","synth voice",
"orchestra hit","trumpet","trombone","tuba","muted trumpet",
"french horn","brass section","synthbrass 1","synthbrass 2","soprano sax",
"alto sax","tenor sax","baritone sax","oboe","english horn",
"bassoon","clarinet","piccolo","flute","recorder",
"pan flute","blown bottle","shakuhachi","whistle","ocarina",
"lead 1 (square)","lead 2 (sawtooth)","lead 3 (calliope)","lead 4 (chiff)","lead 5 (charang)",
"lead 6 (voice)","lead 7 (fifths)","lead 8 (bass + lead)","pad 1 (new age)","pad 2 (warm)",
"pad 3 (polysynth)","pad 4 (choir)","pad 5 (bowed)","pad 6 (metallic)","pad 7 (halo)",
"pad 8 (sweep)","fx 1 (train)","fx 2 (soundtrack)","fx 3 (crystal)","fx 4 (atmosphere)",
"fx 5 (brightness)","fx 6 (goblins)","fx 7 (echoes)","fx 8 (sci-fi)","sitar",
"banjo","shamisen","koto","kalimba","bag pipe",
"fiddle","shanai","tinkle bell","agogo","steel drums",
"woodblock","taiko drum","melodic tom","synth drum","reverse cymbal",
"guitar fret noise","breath noise","seashore","bird tweet","telephone ring",
"helicopter","applause","gunshot"};

vector<short> channel0N,channel1N,channel2N,channel3N,channel4N,channel5N,channel6N,
channel7N,channel8N,channel9N,channel10N,channel11N,channel12N,
channel13N,channel14N,channel15N;

vector<unsigned long> channel0T,channel1T,channel2T,channel3T,channel4T,channel5T,channel6T,
channel7T,channel8T,channel9T,channel10T,channel11T,channel12T,
channel13T,channel14T,channel15T;

vector<short> finalNotes;
vector<unsigned long> finalTimes;

ifstream midiFile;
ofstream textFile;
ofstream DEBUG;
ofstream noterFile;

void genNotary();
void fileHeader();
short readValue(int);
void textIt();
void getNote(short,short);
void enterNote(short,short,short);
void addTimeType0(short);
void addTimeType1(short);
void justify(int,char);
void transmuteMenu();
void meldMenu();
void twoMeldMenu();
void threeMeldMenu();
void twoMeld(int,int);
void threeMeld(int,int,int);
unsigned long readVarLen();
void sortChan(short);
void sortMeta();
void trackHeader();
void fileOpener();
void outputters(string);
void outputterh(string,int);
void outputterd(string,int);
void noter(int);
void notermenu();
void keyChange(int,int);
void keyChangeMenu();

int main() {

    short typeByte;

    fileOpener();

    genNotary();
    fileHeader();
    trackHeader();

    while (doneTracks<totalTracks) {
        deltaTime=readVarLen();
        if (!timeDivision) {
            //noteTime+=(unsigned long)deltaTime;
            //totalTime+=(unsigned long)deltaTime;
            noteTime+=(unsigned long)((double)deltaTime*(60000.0/((double)tempo*(double)clicksPerBeat))+0.5);
            totalTime+=(unsigned long)((double)deltaTime*(60000.0/((double)tempo*(double)clicksPerBeat))+0.5);
            deltaTime=0;
        } else {
            //noteTime+=(unsigned long)deltaTime;
            //totalTime+=(unsigned long)deltaTime;
            noteTime+=(unsigned long)((double)deltaTime*(60000.0/(double)clicksPerSecond)+0.5);
            totalTime+=(unsigned long)((double)deltaTime*(60000.0/(double)clicksPerSecond)+0.5);
            deltaTime=0;
        }
        typeByte=readValue(1);
        if (typeByte==0xFF) {
            sortMeta();
        } else if (typeByte==0xF0||typeByte==0xF7) {         //sysex event
            outputters("sysex event found");
            while (typeByte!=0xF7) {
                  typeByte=readValue(1);
            }
        } else {
            sortChan(typeByte);
        }
    }
    textIt();
    midiFile.close();
    textFile.close();
    DEBUG.close();
    transmuteMenu();
    keyChangeMenu();
    textFile.open(textName);
    textIt();
    meldMenu();
    notermenu();
    textFile.close();
}

void sortChan(short channelByte) {

    short initData;

    if (channelByte/16!=0x09&channelByte/16!=0x08&channelByte/16!=0x0A&channelByte/16!=0x0E&channelByte/16!=0x0D&channelByte/16!=0x0B&channelByte/16!=0x0C) {
        initData=channelByte;
        outputters("running status type");
        outputterh("prevType = ",prevType);
        outputterh("initData = ",initData);
    } else {
        prevType=channelByte;
        initData=readValue(1);
        outputters("novel type");
        outputterh("prevType = ",prevType);
        outputterh("initData = ",initData);
    }

    if (prevType/16==0x09) {          //note on event
        outputters("note on found!");
	    getNote(prevType,initData);
    } else if (prevType/16==0x08) {
        outputters("note OFF event");
	    //readValue(2);
	    readValue(1);
    } else if (prevType/16==0x0A) {
	    outputters("note aftertouch event");
	    //readValue(2);
	    readValue(1);
    } else if (prevType/16==0x0E) {
	    outputters("pitch bend event");
	    //readValue(2);
	    readValue(1);
    } else if (prevType/16==0x0D) {
	    outputters("channel aftertouch event");
	    //readValue(1);
    } else if (prevType/16==0x0B) {	//controller event has 2 bytes of data
	    outputters("controller event");
	    readValue(1);
    } else if (prevType/16==0x0C) {
	    outputters("program change event!");
	    switch (prevType) {
	        case 0xC0:
                instrument0=soundSet[initData];
                break;
            case 0xC1:
                instrument1=soundSet[initData];
                break;
            case 0xC2:
                instrument2=soundSet[initData];
                break;
            case 0xC3:
                instrument3=soundSet[initData];
                break;
            case 0xC4:
                instrument4=soundSet[initData];
                break;
            case 0xC5:
                instrument5=soundSet[initData];
                break;
            case 0xC6:
                instrument6=soundSet[initData];
                break;
            case 0xC7:
                instrument7=soundSet[initData];
                break;
            case 0xC8:
                instrument8=soundSet[initData];
                break;
            case 0xC9:
                instrument9=soundSet[initData];
                break;
            case 0xCA:
                instrument10=soundSet[initData];
                break;
            case 0xCB:
                instrument11=soundSet[initData];
                break;
            case 0xCC:
                instrument12=soundSet[initData];
                break;
            case 0xCD:
                instrument13=soundSet[initData];
                break;
            case 0xCE:
                instrument14=soundSet[initData];
                break;
            case 0xCF:
                instrument15=soundSet[initData];
                break;
            default:
                outputters("instrument name error!");
        }
    }
}

void sortMeta() {
    short writeByte;
    int skipBytes;
    char textData;
    int channelPrefix;
    outputters("meta event found!");
    writeByte = readValue(1);
    if (writeByte==0x01||writeByte==0x02||writeByte==0x03||  //text meta event
            writeByte==0x05||writeByte==0x06||writeByte==0x07) {
        outputters("text meta event");
        writeByte = readValue(1);
        int tempLoop=writeByte;
        for (int blub=0;blub<tempLoop;blub++) {                    //find the end of the data string
            writeByte=readValue(1);
            textData=writeByte;
            outputterh("",textData);
        }
        outputters("");
    } else if ((writeByte==0x04)) {
        outputters("instrument name");
        writeByte = readValue(1);
        int tempLoop=writeByte;
        for (int blub=0;blub<tempLoop;blub++) {
            textData=readValue(1);
            outputterh("",textData);
        }
        outputters("");
    } else if (writeByte==0x00) {
        outputters("sequence number event");
        skipBytes = readValue(1);
        writeByte=readValue(skipBytes);  //skip the 1+2 bytes of data
    } else if (writeByte==0x20) {
        outputters("channel prefix event");
        writeByte = readValue(1);
        channelPrefix = readValue(1);
    } else if (writeByte==0x2F) {	//end of track
        //cout << doneTracks << endl;
        doneTracks++;
        if (doneTracks==totalTracks) return;
        outputters("end of track");
        writeByte=readValue(1);	//skip the 1+0 data bytes
        trackHeader();
    } else if (writeByte==0x51) {            //set tempo event
        outputters("set tempo event");
        tempo=0;
        writeByte=readValue(2);
        tempo+=writeByte*65536;
        writeByte=readValue(1);
        tempo+=writeByte*256;
        writeByte=readValue(1);
        tempo+=writeByte;
        tempo=60000000/tempo;
    } else if (writeByte==0x54) {            //SMPTE offset...(?)
        outputters("SMPTE offset event");
        skipBytes = readValue(1);
        writeByte=readValue(skipBytes);	//skip the 1+5 data bytes
    } else if (writeByte==0x58) {            //time signature
        outputters("time signature event" );
        writeByte=readValue(2);
        timeSigNum=writeByte;
        outputterd("timeSigNum = ",timeSigNum);
        writeByte=readValue(1);
        outputterd("timeSigDenRaw = ",writeByte);
        timeSigDen=pow(2,writeByte);
        outputterd("timeSigDen = ",timeSigDen);
        writeByte=readValue(1);
        timeSigMet=writeByte;
        outputterd("timeSigMet = ",timeSigMet);
        writeByte=readValue(1);
        timeSig32s=writeByte;
        outputterd("timeSig32s = ",timeSig32s);
    } else if (writeByte==0x59) {            //key signature
        outputters("key signature event");
        writeByte=readValue(2);
        int key=writeByte;
        writeByte=readValue(1);
        bool scale=writeByte;
        switch (key) {
            case 0:
                if (!scale) keySignature="C major";
                else keySignature="a minor";
                break;
            case 1:
                if (!scale) keySignature="G major";
                else keySignature="e minor";
                break;
            case 2:
                if (!scale) keySignature="D major";
                else keySignature="b minor";
                break;
            case 3:
                if (!scale) keySignature="A major";
                else keySignature="f# minor";
                break;
            case 4:
                if (!scale) keySignature="E major";
                else keySignature="c# minor";
                break;
            case 5:
                if (!scale) keySignature="B major";
                else keySignature="g# minor";
                break;
            case 6:
                if (!scale) keySignature="F# major";
                else keySignature="d# minor";
                break;
            case 7:
                if (!scale) keySignature="C# major";
                else keySignature="a# minor";
                break;
            case 0xFF:
                if (!scale) keySignature="F major";
                else keySignature="d minor";
                break;
            case 0xFE:
                if (!scale) keySignature="Bb major";
                else keySignature="g minor";
                break;
            case 0xFD:
                if (!scale) keySignature="Eb major";
                else keySignature="g minor";
                break;
            case 0xFC:
                if (!scale) keySignature="Ab major";
                else keySignature="f minor";
                break;
            case 0xFB:
                if (!scale) keySignature="Db major";
                else keySignature="bb minor";
                break;
            case 0xFA:
                if (!scale) keySignature="Gb major";
                else keySignature="eb minor";
                break;
            case 0xF9:
                if (!scale) keySignature="B major";
                else keySignature="ab minor";
                break;
            default:
                keySignature="keySig error";
                break;
        }
    } else if (writeByte==0x7F) {            //sequencer specific data
        outputters("Sequencer-specific data detected!!!");
        while (writeByte!=0x00) {
            writeByte=readValue(1);
        }
    } else {
        outputters("unidentified meta event");
        skipBytes = readValue(1);
        outputterd("bytes to skip = ",skipBytes);
        writeByte=readValue(skipBytes);
    }
}

void fileOpener() {
    do {
        cout << "Please tell me the name of the MIDI file. (include .mid)" << endl;
        cin.getline(midiName,120);
        midiFile.open(midiName,ios::binary);
        } while (!(midiFile.is_open()));
    cout << "Input file opened successfully." << endl;
    do {
        cout << "Please tell me the name of the output file. (include .txt)" << endl;
        cin.getline(textName,120);
        textFile.open(textName);
    } while (!(textFile.is_open()));
    DEBUG.open("DEBUG.txt");
}

unsigned long readVarLen() {
    unsigned long finalVal=0;
    short tempVal;
    tempVal=readValue(1);
    outputterh("tempTime = ",tempVal);
    while (true) {
        if ((tempVal/128)==0) {
            outputters("activated time break");
            break;
        }
        outputters("entered time loop");
        finalVal+=((tempVal-128)*128);
        outputterd("finalVal plussed = ",finalVal);
        tempVal=readValue(1);
        outputterd("next tempVal = ",tempVal);
    }
    outputters("left time loop");
    finalVal+=tempVal;
    outputterd("finalTime = ",finalVal);
    return finalVal;
}

void textIt() {

    /*cout << instrument0 << endl << notary[highest0] << "," << notary[lowest0] << "," << highest0-lowest0 << endl;
    cout << instrument1 << endl << notary[highest1] << "," << notary[lowest1] << "," << highest1-lowest1 << endl;
    cout << instrument2 << endl << notary[highest2] << "," << notary[lowest2] << "," << highest2-lowest2 << endl;
    cout << instrument3 << endl << notary[highest3] << "," << notary[lowest3] << "," << highest3-lowest3 << endl;
    cout << instrument4 << endl << notary[highest4] << "," << notary[lowest4] << "," << highest4-lowest4 << endl;
    cout << instrument5 << endl << notary[highest5] << "," << notary[lowest5] << "," << highest5-lowest5 << endl;
    cout << instrument6 << endl << notary[highest6] << "," << notary[lowest6] << "," << highest6-lowest6 << endl;
    cout << instrument7 << endl << notary[highest7] << "," << notary[lowest7] << "," << highest7-lowest7 << endl;
    cout << instrument8 << endl << notary[highest8] << "," << notary[lowest8] << "," << highest8-lowest8 << endl;
    cout << instrument10 << endl << notary[highest10] << "," << notary[lowest10] << "," << highest10-lowest10 << endl;
    cout << instrument11 << endl << notary[highest11] << "," << notary[lowest11] << "," << highest11-lowest11 << endl;
    cout << instrument12 << endl << notary[highest12] << "," << notary[lowest12] << "," << highest12-lowest12 << endl;
    cout << instrument13 << endl << notary[highest13] << "," << notary[lowest13] << "," << highest13-lowest13 << endl;
    cout << instrument14 << endl << notary[highest14] << "," << notary[lowest14] << "," << highest14-lowest14 << endl;
    cout << instrument15 << endl << notary[highest15] << "," << notary[lowest15] << "," << highest15-lowest15 << endl;*/


    textFile << endl << "Type = " << type << endl;
	if (!timeDivision) {
		textFile << "Delta clicks per quarter note = " << dec << clicksPerBeat << endl;
	}
	else {
		textFile << "Frames per second = " << dec << framesPerSecond << endl;
		textFile << "Clicks per frame = " << dec << clicksPerFrame << endl;
		textFile << "Clicks per second = " << dec << clicksPerSecond << endl;
	}
    textFile << "Tempo = " << tempo << "BPM" << endl;
    textFile << "Time signature = " << timeSigNum << "/" << timeSigDen << endl;
    textFile << endl << endl << "keySignature = " << keySignature << endl;
    textFile << endl << endl << instrument0 << endl;
    textFile << notary[highest0] << "," << notary[lowest0] << "," << highest0-lowest0 << endl;
    textFile << "Channel0N:{";
    for (int i =0; i<channel0N.size();i++) {
      textFile << notary[channel0N[i]] << ",";
    }
    textFile << "0}";
    textFile << endl << "Channel0T:{";
    for (int i =0; i<channel0T.size();i++) {
      textFile << channel0T[i] << ",";
    }
    textFile << "0}";
    textFile << endl << endl  << instrument1 << endl;
    textFile << notary[highest1] << "," << notary[lowest1] << "," << highest1-lowest1 << endl;
    textFile <<  "Channel1N:{";
    for (int i =0; i<channel1N.size();i++) {
      textFile << notary[channel1N[i]] << ",";
    }
    textFile << "0}";
    textFile << endl <<  "Channel1T:{";
    for (int i =0; i<channel1T.size();i++) {
      textFile << channel1T[i] << ",";
    }
    textFile << "0}";
    textFile << endl << endl  << instrument2 << endl;
    textFile << notary[highest2] << "," << notary[lowest2] << "," << highest2-lowest2 << endl;
    textFile << "Channel2N:{";
    for (int i =0; i<channel2N.size();i++) {
      textFile << notary[channel2N[i]] << ",";
    }
    textFile << "0}";
    textFile << endl << "Channel2T:{";
    for (int i =0; i<channel2T.size();i++) {
      textFile << channel2T[i] << ",";
    }
    textFile << "0}";
    textFile << endl << endl  << instrument3 << endl;
    textFile << notary[highest3] << "," << notary[lowest3] << "," << highest3-lowest3 << endl;
    textFile << "Channel3N:{";
    for (int i =0; i<channel3N.size();i++) {
      textFile << notary[channel3N[i]] << ",";
    }
    textFile << "0}";
    textFile << endl << "Channel3T:{";
    for (int i =0; i<channel3T.size();i++) {
      textFile << channel3T[i] << ",";
    }
    textFile << "0}";
    textFile << endl << endl  << instrument4 << endl;
    textFile << notary[highest4] << "," << notary[lowest4] << "," << highest4-lowest4 << endl;
    textFile << "Channel4N:{";
    for (int i =0; i<channel4N.size();i++) {
      textFile << notary[channel4N[i]] << ",";
    }
    textFile << "0}";
    textFile << endl << "Channel4T:{";
    for (int i =0; i<channel4T.size();i++) {
      textFile << channel4T[i] << ",";
    }
    textFile << "0}";
    textFile << endl << endl  << instrument5 << endl;
    textFile << notary[highest5] << "," << notary[lowest5] << "," << highest5-lowest5 << endl;
    textFile << "Channel5N:{";
    for (int i =0; i<channel5N.size();i++) {
      textFile << notary[channel5N[i]] << ",";
    }
    textFile << "0}";
    textFile << endl << "Channel5T:{";
    for (int i =0; i<channel5T.size();i++) {
      textFile << channel5T[i] << ",";
    }
    textFile << "0}";
    textFile << endl << endl  << instrument6 << endl;
    textFile << notary[highest6] << "," << notary[lowest6] << "," << highest6-lowest6 << endl;
    textFile << "Channel6N:{";
    for (int i =0; i<channel6N.size();i++) {
      textFile << notary[channel6N[i]] << ",";
    }
    textFile << "0}";
    textFile << endl << "Channel6T:{";
    for (int i =0; i<channel6T.size();i++) {
      textFile << channel6T[i] << ",";
    }
    textFile << "0}";
    textFile << endl << endl  << instrument7 << endl;
    textFile << notary[highest7] << "," << notary[lowest7] << "," << highest7-lowest7 << endl;
    textFile << "Channel7N:{";
    for (int i =0; i<channel7N.size();i++) {
      textFile << notary[channel7N[i]] << ",";
    }
    textFile << "0}";
    textFile << endl << "Channel7T:{";
    for (int i =0; i<channel7T.size();i++) {
      textFile << channel7T[i] << ",";
    }
    textFile << "0}";
    textFile << endl << endl  << instrument8 << endl;
    textFile << notary[highest8] << "," << notary[lowest8] << "," << highest8-lowest8 << endl;
    textFile << "Channel8N:{";
    for (int i =0; i<channel8N.size();i++) {
      textFile << notary[channel8N[i]] << ",";
    }
    textFile << "0}";
    textFile << endl << "Channel8T:{";
    for (int i =0; i<channel8T.size();i++) {
      textFile << channel8T[i] << ",";
    }
    textFile << "0}";
    textFile << endl << endl  << "percussion" << endl;
    textFile << "Channel9N:{";
    for (int i =0; i<channel9N.size();i++) {
      textFile << notary[channel9N[i]] << ",";
    }
    textFile << "0}";
    textFile << endl << "Channel9T:{";
    for (int i =0; i<channel9T.size();i++) {
      textFile << channel9T[i] << ",";
    }
    textFile << "0}";
    textFile << endl << endl  << instrument10 << endl;
    textFile << notary[highest10] << "," << notary[lowest10] << "," << highest10-lowest10 << endl;
    textFile << "Channel10N:{";
    for (int i =0; i<channel10N.size();i++) {
      textFile << notary[channel10N[i]] << ",";
    }
    textFile << "0}";
    textFile << endl << "Channel10T:{";
    for (int i =0; i<channel10T.size();i++) {
      textFile << channel10T[i] << ",";
    }
    textFile << "0}";
    textFile << endl << endl  << instrument11 << endl;
    textFile << notary[highest11] << "," << notary[lowest11] << "," << highest11-lowest11 << endl;
    textFile << "Channel11N:{";
    for (int i =0; i<channel11N.size();i++) {
      textFile << notary[channel11N[i]] << ",";
    }
    textFile << "0}";
    textFile << endl << "Channel11T:{";
    for (int i =0; i<channel11T.size();i++) {
      textFile << channel11T[i] << ",";
    }
    textFile << "0}";
    textFile << endl << endl  << instrument12 << endl;
    textFile << notary[highest12] << "," << notary[lowest12] << "," << highest12-lowest12 << endl;
    textFile << "Channel12N:{";
    for (int i =0; i<channel12N.size();i++) {
      textFile << notary[channel12N[i]] << ",";
    }
    textFile << "0}";
    textFile << endl << "Channel12T:{";
    for (int i =0; i<channel12T.size();i++) {
      textFile << channel12T[i] << ",";
    }
    textFile << "0}";
    textFile << endl << endl  << instrument13 << endl;
    textFile << notary[highest13] << "," << notary[lowest13] << "," << highest13-lowest13 << endl;
    textFile << "Channel13N:{";
    for (int i =0; i<channel13N.size();i++) {
      textFile << notary[channel13N[i]] << ",";
    }
    textFile << "0}";
    textFile << endl << "Channel13T:{";
    for (int i =0; i<channel13T.size();i++) {
      textFile << channel13T[i] << ",";
    }
    textFile << "0}";
    textFile << endl << endl  << instrument14 << endl;
    textFile << notary[highest14] << "," << notary[lowest14] << "," << highest14-lowest14 << endl;
    textFile << "Channel14N:{";
    for (int i =0; i<channel14N.size();i++) {
      textFile << notary[channel14N[i]] << ",";
    }
    textFile << "0}";
    textFile << endl << "Channel14T:{";
    for (int i =0; i<channel14T.size();i++) {
      textFile << channel14T[i] << ",";
    }
    textFile << "0}";
    textFile << endl << endl  << instrument15 << endl;
    textFile << notary[highest15] << "," << notary[lowest15] << "," << highest15-lowest15 << endl;
    textFile << "Channel15N:{";
    for (int i =0; i<channel15N.size();i++) {
      textFile << notary[channel15N[i]] << ",";
    }
    textFile << "0}";
    textFile << endl << "Channel15T:{";
    for (int i =0; i<channel15T.size();i++) {
      textFile << channel15T[i] << ",";
    }
    textFile << "0}" << endl;
}


void fileHeader() {                //handles file header
    short writeByte;

    outputters("file header'd  ");

    for (int i=0;i<14;i++) {
        writeByte=readValue(1);
        outputterh("",writeByte);
        switch (i) {
            case 9:
                if (writeByte==0) {
                    type="Single Track";
                    typeLiteral=0;
                } else if (writeByte==1) {
                    type="Multiple Tracks, Synchronous";
                    typeLiteral=1;
                } else if (writeByte==2) {
                    type="Multiple Tracks, Asynchronous";
                    typeLiteral=2;
                }
                continue;
            case 11:
                totalTracks=writeByte;
                continue;
            case 12:
                if (writeByte<0x80) {
			timeDivision=false;
			clicksPerBeat+=writeByte*256;
		}
		else {
			timeDivision=true;
			framesPerSecond=writeByte&0x7F;
		}
		continue;
            case 13:
		if (!timeDivision) {
			clicksPerBeat+=writeByte;
		}
		else {
			clicksPerFrame=writeByte;
			clicksPerSecond=clicksPerFrame*framesPerSecond;
		}
		continue;
            }
        }
}

void trackHeader() {
        noteTime=0;
        deltaTime=0;
        int trackLength=0;
        short writeByte;
        writeByte=readValue(5);	//read in MTrk
        trackLength+=writeByte*16777216;
        writeByte=readValue(1);
        trackLength+=writeByte*65536;
        writeByte=readValue(1);
        trackLength+=writeByte*256;
        writeByte=readValue(1);
        trackLength+=writeByte;
        outputterd("new track = ",doneTracks+1);
        outputterd("track length = ",trackLength);
}

void genNotary() {
  int d=0;
  stringstream octave;
  string holder;
  for (int e=0;e<10;e++) {
    octave.str("");
    octave<<e;
    holder=octave.str();
    notary[d]=("c"+holder);
    notary[d+1]=("cd"+holder);
    notary[d+2]=("d"+holder);
    notary[d+3]=("de"+holder);
    notary[d+4]=("e"+holder);
    notary[d+5]=("f"+holder);
    notary[d+6]=("fg"+holder);
    notary[d+7]=("g"+holder);
    notary[d+8]=("ga"+holder);
    notary[d+9]=("a"+holder);
    notary[d+10]=("ab"+holder);
    notary[d+11]=("b"+holder);
    d+=12;
  }
    notary[120]=("c10");
    notary[121]=("cd10");
    notary[122]=("d10");
    notary[123]=("de10");
    notary[124]=("e10");
    notary[125]=("f10");
    notary[126]=("fg10");
    notary[127]=("g10");
}


void getNote(short channel,short note) {
  short volume;
  volume=readValue(1);
  enterNote(channel,note,volume);
}

void addTimeType1(short channel) {
    /*if (noteTime>6) {
        noteTime-=7;
    } else {
        noteTime=0;
    }*/
	switch (channel) {
		case 0x90:
			channel0T.push_back (noteTime);
			outputterd("channel0T-> ",noteTime);
			break;
		case 0x91:
			channel1T.push_back (noteTime);
			outputterd("channel1T-> ",noteTime);
			break;
		case 0x92:
			channel2T.push_back (noteTime);
			outputterd("channel2T-> ",noteTime);
			break;
		case 0x93:
			channel3T.push_back (noteTime);
			outputterd("channel3T-> ",noteTime);
			break;
		case 0x94:
			channel4T.push_back (noteTime);
			outputterd("channel4T-> ",noteTime);
			break;
		case 0x95:
			channel5T.push_back (noteTime);
			outputterd("channel5T-> ",noteTime);
			break;
		case 0x96:
			channel6T.push_back (noteTime);
			outputterd("channel6T-> ",noteTime);
			break;
		case 0x97:
			channel7T.push_back (noteTime);
			outputterd("channel7T-> ",noteTime);
			break;
		case 0x98:
			channel8T.push_back (noteTime);
			outputterd("channel8T-> ",noteTime);
			break;
		case 0x99:
			channel9T.push_back (noteTime);
			outputterd("channel9T-> ",noteTime);
			break;
		case 0x9A:
			channel10T.push_back (noteTime);
			outputterd("channel10T-> ",noteTime);
			break;
		case 0x9B:
			channel11T.push_back (noteTime);
			outputterd("channel11T-> ",noteTime);
			break;
		case 0x9C:
			channel12T.push_back (noteTime);
			outputterd("channel12T-> ",noteTime);
			break;
		case 0x9D:
			channel13T.push_back (noteTime);
			outputterd("channel13T-> ",noteTime);
			break;
		case 0x9E:
			channel14T.push_back (noteTime);
			outputterd("channel14T-> ",noteTime);
			break;
		case 0x9F:
			channel15T.push_back (noteTime);
			outputterd("channel15T-> ",noteTime);
			break;
		default:
			outputters("CRAAAAAAP!  (addTime error)");
	}
	noteTime=0;
	outputterd("totalTime=",totalTime);
}

void addTimeType0(short channel) {
	delta0Time+=noteTime;
	delta1Time+=noteTime;
	delta2Time+=noteTime;
	delta3Time+=noteTime;
	delta4Time+=noteTime;
	delta5Time+=noteTime;
	delta6Time+=noteTime;
	delta7Time+=noteTime;
	delta8Time+=noteTime;
	delta9Time+=noteTime;
	delta10Time+=noteTime;
	delta11Time+=noteTime;
	delta12Time+=noteTime;
	delta13Time+=noteTime;
	delta14Time+=noteTime;
	delta15Time+=noteTime;
	switch (channel) {
		case 0x90:
            if (delta0Time>6) {
                delta0Time-=7;
            } else {
                delta0Time=0;
            }
			channel0T.push_back (delta0Time);
			outputterd("channel0T-> ",delta0Time);
			delta0Time=0;
			break;
		case 0x91:
            if (delta1Time>6) {
                delta1Time-=7;
            } else {
                delta1Time=0;
            }
			channel1T.push_back (delta1Time);
			outputterd("channel1T-> ",delta1Time);
			delta1Time=0;
			break;
		case 0x92:
            if (delta2Time>6) {
                delta2Time-=7;
            } else {
                delta2Time=0;
            }
			channel2T.push_back (delta2Time);
			outputterd("channel2T-> ",delta2Time);
			delta2Time=0;
			break;
		case 0x93:
            if (delta3Time>6) {
                delta3Time-=7;
            } else {
                delta3Time=0;
            }
			channel3T.push_back (delta3Time);
			outputterd("channel3T-> ",delta3Time);
			delta3Time=0;
			break;
		case 0x94:
            if (delta4Time>6) {
                delta4Time-=7;
            } else {
                delta4Time=0;
            }
			channel4T.push_back (delta4Time);
			outputterd("channel4T-> ",delta4Time);
			delta4Time=0;
			break;
		case 0x95:
            if (delta5Time>6) {
                delta5Time-=7;
            } else {
                delta5Time=0;
            }
			channel5T.push_back (delta5Time);
			outputterd("channel5T-> ",delta5Time);
			delta5Time=0;
			break;
		case 0x96:
            if (delta6Time>6) {
                delta6Time-=7;
            } else {
                delta6Time=0;
            }
			channel6T.push_back (delta6Time);
			outputterd("channel6T-> ",delta6Time);
			delta6Time=0;
			break;
		case 0x97:
            if (delta7Time>6) {
                delta7Time-=7;
            } else {
                delta7Time=0;
            }
			channel7T.push_back (delta7Time);
			outputterd("channel7T-> ",delta7Time);
			delta7Time=0;
			break;
		case 0x98:
            if (delta8Time>6) {
                delta8Time-=7;
            } else {
                delta8Time=0;
            }
			channel8T.push_back (delta8Time);
			outputterd("channel8T-> ",delta8Time);
			delta8Time=0;
			break;
		case 0x99:
            if (delta9Time>6) {
                delta9Time-=7;
            } else {
                delta9Time=0;
            }
			channel9T.push_back (delta9Time);
			outputterd("channel9T-> ",delta9Time);
			delta9Time=0;
			break;
		case 0x9A:
            if (delta10Time>6) {
                delta10Time-=7;
            } else {
                delta10Time=0;
            }
			channel10T.push_back (delta10Time);
			outputterd("channel10T-> ",delta10Time);
			delta10Time=0;
			break;
		case 0x9B:
            if (delta11Time>6) {
                delta11Time-=7;
            } else {
                delta11Time=0;
            }
			channel11T.push_back (delta11Time);
			outputterd("channel11T-> ",delta11Time);
			delta11Time=0;
			break;
		case 0x9C:
            if (delta12Time>6) {
                delta12Time-=7;
            } else {
                delta12Time=0;
            }
			channel12T.push_back (delta12Time);
			outputterd("channel12T-> ",delta12Time);
			delta12Time=0;
			break;
		case 0x9D:
            if (delta13Time>6) {
                delta13Time-=7;
            } else {
                delta13Time=0;
            }
			channel13T.push_back (delta13Time);
			outputterd("channel13T-> ",delta13Time);
			delta13Time=0;
			break;
		case 0x9E:
            if (delta14Time>6) {
                delta14Time-=7;
            } else {
                delta14Time=0;
            }
			channel14T.push_back (delta14Time);
			outputterd("channel14T-> ",delta14Time);
			delta14Time=0;
			break;
		case 0x9F:
            if (delta15Time>6) {
                delta15Time-=7;
            } else {
                delta15Time=0;
            }
			channel15T.push_back (delta15Time);
			outputterd("channel15T-> ",delta15Time);
			delta15Time=0;
			break;
		default:
			outputters("CRAAAAAAP!  (addTime error)");
	}
	noteTime=0;
	outputterd("totalTime=",totalTime);
}


void enterNote(short channel,short note,short volume) {
    outputterd("volume=",volume);
    if (volume>0) {
        outputters("entering note");
        switch (channel) {
            case 0x90:
                if (note>highest0) highest0=note;
                if (note<lowest0) lowest0=note;
                outputters("channel0N");
                outputterh("note=",note);
                channel0N.push_back (note);
                outputterd("channel0N size=",channel0N.size());
                break;
            case 0x91:
                if (note>highest1) highest1=note;
                if (note<lowest1) lowest1=note;
                outputters("channel1N");
                outputterh("note=",note);
                channel1N.push_back (note);
                outputterd("channel1N size=",channel1N.size());
                break;
            case 0x92:
      if (note>highest2) highest2=note;
      if (note<lowest2) lowest2=note;
      outputters("channel2N");
      outputterh("note=",note);
      channel2N.push_back (note);
      outputterd("channel2N size=",channel2N.size());
      break;
    case 0x93:
      if (note>highest3) highest3=note;
      if (note<lowest3) lowest3=note;
      outputters("channel3N");
      outputterh("note=",note);
      channel3N.push_back (note);
      outputterd("channel3N size=",channel3N.size());
      break;
    case 0x94:
      if (note>highest4) highest4=note;
      if (note<lowest4) lowest4=note;
      outputters("channel4N");
      outputterh("note=",note);
      channel4N.push_back (note);
      outputterd("channel4N size=",channel4N.size());
      break;
    case 0x95:
      if (note>highest5) highest5=note;
      if (note<lowest5) lowest5=note;
      outputters("channel5N");
      outputterh("note=",note);
      channel5N.push_back (note);
      outputterd("channel5N size=",channel5N.size());
      break;
    case 0x96:
      if (note>highest6) highest6=note;
      if (note<lowest6) lowest6=note;
      outputters("channel6N");
      outputterh("note=",note);
      channel6N.push_back (note);
      outputterd("channel6N size=",channel6N.size());
      break;
    case 0x97:
      if (note>highest7) highest7=note;
      if (note<lowest7) lowest7=note;
      outputters("channel7N");
      outputterh("note=",note);
      channel7N.push_back (note);
      outputterd("channel7N size=",channel7N.size());
      break;
    case 0x98:
      if (note>highest8) highest8=note;
      if (note<lowest8) lowest8=note;
      outputters("channel8N");
      outputterh("note=",note);
      channel8N.push_back (note);
      outputterd("channel8N size=",channel8N.size());
      break;
    case 0x99:
      outputters("channel9N");
      outputterh("note=",note);
      channel9N.push_back (note);
      outputterd("channel9N size=",channel9N.size());
      break;
    case 0x9A:
      if (note>highest10) highest10=note;
      if (note<lowest10) lowest10=note;
      outputters("channel10N");
      outputterh("note=",note);
      channel10N.push_back (note);
      outputterd("channel10N size=",channel10N.size());
      break;
    case 0x9B:
      if (note>highest11) highest11=note;
      if (note<lowest11) lowest11=note;
      outputters("channel11N");
      outputterh("note=",note);
      channel11N.push_back (note);
      outputterd("channel11N size=",channel11N.size());
      break;
    case 0x9C:
      if (note>highest12) highest12=note;
      if (note<lowest12) lowest12=note;
      outputters("channel12N");
      outputterh("note=",note);
      channel12N.push_back (note);
      outputterd("channel12N size=",channel12N.size());
      break;
    case 0x9D:
      if (note>highest13) highest13=note;
      if (note<lowest13) lowest13=note;
      outputters("channel13N");
      outputterh("note=",note);
      channel13N.push_back (note);
      outputterd("channel13N size=",channel13N.size());
      break;
    case 0x9E:
      if (note>highest14) highest14=note;
      if (note<lowest14) lowest14=note;
      outputters("channel14N");
      outputterh("note=",note);
      channel14N.push_back (note);
      outputterd("channel14N size=",channel14N.size());
      break;
    case 0x9F:
      if (note>highest15) highest15=note;
      if (note<lowest15) lowest15=note;
      outputters("channel15N");
      outputterh("note=",note);
      channel15N.push_back (note);
      outputterd("channel15N size=",channel15N.size());
      break;
    default:
      outputterd("OOPS! Error in enterNote. Channel=",channel);
  }
  if (typeLiteral==0) {
      addTimeType0(channel);
  } else if (typeLiteral==1) {
      addTimeType1(channel);
  } else {
      outputters("COULD NOT FIGURE OUT FILE TYPE OH NO");
  }
  } else {
	outputters("volume=0, did not enter");
}
}

short readValue(int m) {
  lineNumber+=m;
  outputterd("",lineNumber);
  unsigned char readByte;
  short writeByte;
  for (int arb=0;arb<m;arb++) {
    readByte=midiFile.get();
    writeByte=readByte;
  }
  //DEBUG << "hex: " << hex << writeByte << endl;
  //DEBUG << dec;
  return writeByte;
}

void transmuteMenu() {

    cout << endl << endl << endl << "Robospiel range: g0 to c3" << endl << endl;
    cout << "0) " << instrument0 << endl << notary[highest0] << "," << notary[lowest0] << "," << highest0-lowest0 << endl;
    cout << "1) " << instrument1 << endl << notary[highest1] << "," << notary[lowest1] << "," << highest1-lowest1 << endl;
    cout << "2) " << instrument2 << endl << notary[highest2] << "," << notary[lowest2] << "," << highest2-lowest2 << endl;
    cout << "3) " << instrument3 << endl << notary[highest3] << "," << notary[lowest3] << "," << highest3-lowest3 << endl;
    cout << "4) " << instrument4 << endl << notary[highest4] << "," << notary[lowest4] << "," << highest4-lowest4 << endl;
    cout << "5) " << instrument5 << endl << notary[highest5] << "," << notary[lowest5] << "," << highest5-lowest5 << endl;
    cout << "6) " << instrument6 << endl << notary[highest6] << "," << notary[lowest6] << "," << highest6-lowest6 << endl;
    cout << "7) " << instrument7 << endl << notary[highest7] << "," << notary[lowest7] << "," << highest7-lowest7 << endl;
    cout << "8) " << instrument8 << endl << notary[highest8] << "," << notary[lowest8] << "," << highest8-lowest8 << endl;
    cout << "10) " << instrument10 << endl << notary[highest10] << "," << notary[lowest10] << "," << highest10-lowest10 << endl;
    cout << "11) " << instrument11 << endl << notary[highest11] << "," << notary[lowest11] << "," << highest11-lowest11 << endl;
    cout << "12) " << instrument12 << endl << notary[highest12] << "," << notary[lowest12] << "," << highest12-lowest12 << endl;
    cout << "13) " << instrument13 << endl << notary[highest13] << "," << notary[lowest13] << "," << highest13-lowest13 << endl;
    cout << "14) " << instrument14 << endl << notary[highest14] << "," << notary[lowest14] << "," << highest14-lowest14 << endl;
    cout << "15) " << instrument15 << endl << notary[highest15] << "," << notary[lowest15] << "," << highest15-lowest15 << endl;

    string input = "";
    char answer;
    int transChannel;
    while (true) {
        cout << "Would you like to octave-jump a part to the fixed range now? (y/n)" << endl;
        getline(cin,input);
        if (input.length() == 1) {
            answer=input[0];
            break;
        }
    }
    if (answer=='N'|answer=='n') {
        return;
    } else {
        while (true) {
            cout << "Which channel would you like to octave-jump? (0-15 excluding 9)" << endl;
            getline(cin,input);
            stringstream myStream(input);
            if (myStream >> transChannel) {
                if ((transChannel>-1)&(transChannel<16)&(transChannel!=9)) break;
            }
        }
        while (true) {
            cout << "Would you like it top justified (t) or bottom justified (b)?" << endl;
            getline(cin,input);
            if ((input.length() == 1)&((input[0]=='t')|(input[0]=='T')|(input[0]=='b')|(input[0]=='B'))) {
                answer=input[0];
                break;
            }
        }
    }
    justify(transChannel,answer);
    transmuteMenu();
}

void justify(int channel,char choice) {
    int guide;
    int marker=0;
    if (choice=='t'|choice=='T') {
        if (channel==0) {
            guide=highest0;
            for (;guide>36;guide-=12) {
                marker++;
            }
            highest0-=(12*marker);
            lowest0-=(12*marker);
            for (int i=0;i<channel0N.size();i++) {
                channel0N[i]-=(12*marker);
            }
        } else if (channel==1) {
            guide=highest1;
            for (;guide>36;guide-=12) {
                marker++;
            }
            highest1-=(12*marker);
            lowest1-=(12*marker);
            for (int i=0;i<channel1N.size();i++) {
                channel1N[i]-=(12*marker);
            }
        } else if (channel==2) {
            guide=highest2;
            for (;guide>36;guide-=12) {
                marker++;
            }
            highest2-=(12*marker);
            lowest2-=(12*marker);
            for (int i=0;i<channel2N.size();i++) {
                channel2N[i]-=(12*marker);
            }
        } else if (channel==3) {
            guide=highest3;
            for (;guide>36;guide-=12) {
                marker++;
            }
            highest3-=(12*marker);
            lowest3-=(12*marker);
            for (int i=0;i<channel3N.size();i++) {
                channel3N[i]-=(12*marker);
            }
        } else if (channel==4) {
            guide=highest4;
            for (;guide>36;guide-=12) {
                marker++;
            }
            highest4-=(12*marker);
            lowest4-=(12*marker);
            for (int i=0;i<channel4N.size();i++) {
                channel4N[i]-=(12*marker);
            }
        } else if (channel==5) {
            guide=highest5;
            for (;guide>36;guide-=12) {
                marker++;
            }
            highest5-=(12*marker);
            lowest5-=(12*marker);
            for (int i=0;i<channel5N.size();i++) {
                channel5N[i]-=(12*marker);
            }
        } else if (channel==6) {
            guide=highest6;
            for (;guide>36;guide-=12) {
                marker++;
            }
            highest6-=(12*marker);
            lowest6-=(12*marker);
            for (int i=0;i<channel6N.size();i++) {
                channel6N[i]-=(12*marker);
            }
        } else if (channel==7) {
            guide=highest7;
            for (;guide>36;guide-=12) {
                marker++;
            }
            highest7-=(12*marker);
            lowest7-=(12*marker);
            for (int i=0;i<channel7N.size();i++) {
                channel7N[i]-=(12*marker);
            }
        } else if (channel==8) {
            guide=highest8;
            for (;guide>36;guide-=12) {
                marker++;
            }
            highest8-=(12*marker);
            lowest8-=(12*marker);
            for (int i=0;i<channel8N.size();i++) {
                channel8N[i]-=(12*marker);
            }
        }  else if (channel==10) {
            guide=highest10;
            for (;guide>36;guide-=12) {
                marker++;
            }
            highest10-=(12*marker);
            lowest10-=(12*marker);
            for (int i=0;i<channel10N.size();i++) {
                channel10N[i]-=(12*marker);
            }
        } else if (channel==11) {
            guide=highest11;
            for (;guide>36;guide-=12) {
                marker++;
            }
            highest11-=(12*marker);
            lowest11-=(12*marker);
            for (int i=0;i<channel11N.size();i++) {
                channel11N[i]-=(12*marker);
            }
        } else if (channel==12) {
            guide=highest12;
            for (;guide>36;guide-=12) {
                marker++;
            }
            highest12-=(12*marker);
            lowest12-=(12*marker);
            for (int i=0;i<channel12N.size();i++) {
                channel12N[i]-=(12*marker);
            }
        } else if (channel==13) {
            guide=highest13;
            for (;guide>36;guide-=12) {
                marker++;
            }
            highest13-=(12*marker);
            lowest13-=(12*marker);
            for (int i=0;i<channel13N.size();i++) {
                channel13N[i]-=(12*marker);
            }
        } else if (channel==14) {
            guide=highest14;
            for (;guide>36;guide-=12) {
                marker++;
            }
            highest14-=(12*marker);
            lowest14-=(12*marker);
            for (int i=0;i<channel14N.size();i++) {
                channel14N[i]-=(12*marker);
            }
        } else if (channel==15) {
            guide=highest15;
            for (;guide>36;guide-=12) {
                marker++;
            }
            highest15-=(12*marker);
            lowest15-=(12*marker);
            for (int i=0;i<channel15N.size();i++) {
                channel15N[i]-=(12*marker);
            }
        }
    } else {
        if (channel==0) {
            guide=lowest0;
            for (;guide>=7;guide-=12) {
                marker++;
            }
            marker--;
            highest0-=(12*marker);
            lowest0-=(12*marker);
            for (int i=0;i<channel0N.size();i++) {
                channel0N[i]-=(12*marker);
            }
        } else if (channel==1) {
            guide=lowest1;
            for (;guide>=7;guide-=12) {
                marker++;
            }
            marker--;
            highest1-=(12*marker);
            lowest1-=(12*marker);
            for (int i=0;i<channel1N.size();i++) {
                channel1N[i]-=(12*marker);
            }
        } else if (channel==2) {
            guide=lowest2;
            for (;guide>=7;guide-=12) {
                marker++;
            }
            marker--;
            highest2-=(12*marker);
            lowest2-=(12*marker);
            for (int i=0;i<channel2N.size();i++) {
                channel2N[i]-=(12*marker);
            }
        } else if (channel==3) {
            guide=lowest3;
            for (;guide>=7;guide-=12) {
                marker++;
            }
            marker--;
            highest3-=(12*marker);
            lowest3-=(12*marker);
            for (int i=0;i<channel3N.size();i++) {
                channel3N[i]-=(12*marker);
            }
        } else if (channel==4) {
            guide=lowest4;
            for (;guide>=7;guide-=12) {
                marker++;
            }
            marker--;
            highest4-=(12*marker);
            lowest4-=(12*marker);
            for (int i=0;i<channel4N.size();i++) {
                channel4N[i]-=(12*marker);
            }
        } else if (channel==5) {
            guide=lowest5;
            for (;guide>=7;guide-=12) {
                marker++;
            }
            marker--;
            highest5-=(12*marker);
            lowest5-=(12*marker);
            for (int i=0;i<channel5N.size();i++) {
                channel5N[i]-=(12*marker);
            }
        } else if (channel==6) {
            guide=lowest6;
            for (;guide>=7;guide-=12) {
                marker++;
            }
            marker--;
            highest6-=(12*marker);
            lowest6-=(12*marker);
            for (int i=0;i<channel6N.size();i++) {
                channel6N[i]-=(12*marker);
            }
        } else if (channel==7) {
            guide=lowest7;
            for (;guide>=7;guide-=12) {
                marker++;
            }
            marker--;
            highest7-=(12*marker);
            lowest7-=(12*marker);
            for (int i=0;i<channel7N.size();i++) {
                channel7N[i]-=(12*marker);
            }
        } else if (channel==8) {
            guide=lowest8;
            for (;guide>=7;guide-=12) {
                marker++;
            }
            marker--;
            highest8-=(12*marker);
            lowest8-=(12*marker);
            for (int i=0;i<channel8N.size();i++) {
                channel8N[i]-=(12*marker);
            }
        } else if (channel==10) {
            guide=lowest10;
            for (;guide>=7;guide-=12) {
                marker++;
            }
            marker--;
            highest10-=(12*marker);
            lowest10-=(12*marker);
            for (int i=0;i<channel10N.size();i++) {
                channel10N[i]-=(12*marker);
            }
        } else if (channel==11) {
            guide=lowest11;
            for (;guide>=7;guide-=12) {
                marker++;
            }
            marker--;
            highest11-=(12*marker);
            lowest11-=(12*marker);
            for (int i=0;i<channel11N.size();i++) {
                channel11N[i]-=(12*marker);
            }
        } else if (channel==12) {
            guide=lowest12;
            for (;guide>=7;guide-=12) {
                marker++;
            }
            marker--;
            highest12-=(12*marker);
            lowest12-=(12*marker);
            for (int i=0;i<channel12N.size();i++) {
                channel12N[i]-=(12*marker);
            }
        } else if (channel==13) {
            guide=lowest13;
            for (;guide>=7;guide-=12) {
                marker++;
            }
            marker--;
            highest13-=(12*marker);
            lowest13-=(12*marker);
            for (int i=0;i<channel13N.size();i++) {
                channel13N[i]-=(12*marker);
            }
        } else if (channel==14) {
            guide=lowest14;
            for (;guide>=7;guide-=12) {
                marker++;
            }
            marker--;
            highest14-=(12*marker);
            lowest14-=(12*marker);
            for (int i=0;i<channel14N.size();i++) {
                channel14N[i]-=(12*marker);
            }
        } else if (channel==15) {
            guide=lowest15;
            for (;guide>=7;guide-=12) {
                marker++;
            }
            marker--;
            highest15-=(12*marker);
            lowest15-=(12*marker);
            for (int i=0;i<channel15N.size();i++) {
                channel15N[i]-=(12*marker);
            }
        }
    }
}

void meldMenu() {
    char answer;
    int meldnum;
    string input;
    while (true) {
        cout << "Would you like to meld channels together? (y/n)" << endl;
        getline(cin,input);
        if (input.length() == 1) {
            answer=input[0];
            break;
        }
    }
    if (answer=='N'|answer=='n') {
        return;
    }

    while (true) {
        cout << "Would you like to meld 2 or 3 channels?" << endl;
        getline(cin,input);
        stringstream myStream(input);
        if (myStream >> meldnum) {
            if ((meldnum>1)&(meldnum<4)) break;
        }
    }

    if (meldnum==2) {
        twoMeldMenu();
    } else if (meldnum==3) {
        threeMeldMenu();
    } else {
        cout << "meld menu error" << endl;
    }
}

void threeMeldMenu() {
    string input;
    int tracka,trackb,trackc;

    while (true) {
        cout << "Choose the first channel to meld. (0-15)" << endl;
        getline(cin,input);
        stringstream myStream(input);
        if (myStream >> tracka) {
            if ((tracka>-1)&(tracka<16)) break;
        }
    }

    while (true) {
        cout << "Choose the second channel to meld. (0-15)" << endl;
        getline(cin,input);
        stringstream myStream(input);
        if (myStream >> trackb) {
            if ((trackb>-1)&(trackb<16)&(trackb!=tracka)) break;
        }
    }

    while (true) {
        cout << "Choose the third channel to meld. (0-15)" << endl;
        getline(cin,input);
        stringstream myStream(input);
        if (myStream >> trackc) {
            if ((trackc>-1)&(trackc<16)&(trackb!=trackc)&(trackc!=tracka)) break;
        }
    }

    cout << "tracka = " << tracka << "   trackb = " << trackb << "    trackc = " << trackc << endl;
    threeMeld(tracka,trackb,trackc);
}

void twoMeldMenu() {
    string input;
    int tracka,trackb;
    while (true) {
        cout << "Choose the first channel to meld. (0-15)" << endl;
        getline(cin,input);
        stringstream myStream(input);
        if (myStream >> tracka) {
            if ((tracka>-1)&(tracka<16)) break;
        }
    }

    while (true) {
        cout << "Choose the second channel to meld. (0-15)" << endl;
        getline(cin,input);
        stringstream myStream(input);
        if (myStream >> trackb) {
            if ((trackb>-1)&(trackb<16)&(trackb!=tracka)) break;
        }
    }
    cout << "tracka = " << tracka << "   trackb = " << trackb << endl;
    twoMeld(tracka,trackb);
}

void twoMeld(int a, int b) {
    vector<short> trackNa;
    vector<unsigned long> trackTa;
    vector<short> trackNb;
    vector<unsigned long> trackTb;

    if (a==0) {
        trackNa=channel0N;
        trackTa=channel0T;
    } else if (a==1) {
        trackNa=channel1N;
        trackTa=channel1T;
    } else if (a==2) {
        trackNa=channel2N;
        trackTa=channel2T;
    } else if (a==3) {
        trackNa=channel3N;
        trackTa=channel3T;
    } else if (a==4) {
        trackNa=channel4N;
        trackTa=channel4T;
    } else if (a==5) {
        trackNa=channel5N;
        trackTa=channel5T;
    } else if (a==6) {
        trackNa=channel6N;
        trackTa=channel6T;
    } else if (a==7) {
        trackNa=channel7N;
        trackTa=channel7T;
    } else if (a==8) {
        trackNa=channel8N;
        trackTa=channel8T;
    } else if (a==9) {
        trackNa=channel9N;
        trackTa=channel9T;
    } else if (a==10) {
        trackNa=channel10N;
        trackTa=channel10T;
    } else if (a==11) {
        trackNa=channel11N;
        trackTa=channel11T;
    } else if (a==12) {
        trackNa=channel12N;
        trackTa=channel12T;
    } else if (a==13) {
        trackNa=channel13N;
        trackTa=channel13T;
    } else if (a==14) {
        trackNa=channel14N;
        trackTa=channel14T;
    } else if (a==15) {
        trackNa=channel15N;
        trackTa=channel15T;
    }

    if (b==0) {
        trackNb=channel0N;
        trackTb=channel0T;
    } else if (b==1) {
        trackNb=channel1N;
        trackTb=channel1T;
    } else if (b==2) {
        trackNb=channel2N;
        trackTb=channel2T;
    } else if (b==3) {
        trackNb=channel3N;
        trackTb=channel3T;
    } else if (b==4) {
        trackNb=channel4N;
        trackTb=channel4T;
    } else if (b==5) {
        trackNb=channel5N;
        trackTb=channel5T;
    } else if (b==6) {
        trackNb=channel6N;
        trackTb=channel6T;
    } else if (b==7) {
        trackNb=channel7N;
        trackTb=channel7T;
    } else if (b==8) {
        trackNb=channel8N;
        trackTb=channel8T;
    } else if (b==9) {
        trackNb=channel9N;
        trackTb=channel9T;
    } else if (b==10) {
        trackNb=channel10N;
        trackTb=channel10T;
    } else if (b==11) {
        trackNb=channel11N;
        trackTb=channel11T;
    } else if (b==12) {
        trackNb=channel12N;
        trackTb=channel12T;
    } else if (b==13) {
        trackNb=channel13N;
        trackTb=channel13T;
    } else if (b==14) {
        trackNb=channel14N;
        trackTb=channel14T;
    } else if (b==15) {
        trackNb=channel15N;
        trackTb=channel15T;
    }

    bool aDone=false,bDone=false;
    int i=0,j=0,k=0;
    int prevTime=0;
    unsigned long totala=trackTa[0];
    unsigned long totalb=trackTb[0];
    unsigned long masterTotal=0;
    //vector<short> finalNotes;
    //vector<unsigned long> finalTimes;

    for (int global=0;!(aDone&bDone);global++) {
        if ((global==totala)&!aDone) {
            finalNotes.push_back(trackNa[j]);
            finalTimes.push_back(global-prevTime);
            prevTime=global;
            totala+=trackTa[j+1];
            j++;
            cout << "j=" << j << endl;
        }
        if ((global==totalb)&!bDone) {
            finalNotes.push_back(trackNb[k]);
            finalTimes.push_back(global-prevTime);
            prevTime=global;
            totalb+=trackTb[k+1];
            k++;
            cout << "k=" << k << endl;
        }
        if ((global==totala)&!aDone) {
            finalNotes.push_back(trackNa[j]);
            finalTimes.push_back(global-prevTime);
            prevTime=global;
            totala+=trackTa[j+1];
            j++;
            cout << "j=" << j << endl;
        }
        if ((global==totalb)&!bDone) {
            finalNotes.push_back(trackNb[k]);
            finalTimes.push_back(global-prevTime);
            prevTime=global;
            totalb+=trackTb[k+1];
            k++;
            cout << "k=" << k << endl;
        }
        if ((global==totala)&!aDone) {
            finalNotes.push_back(trackNa[j]);
            finalTimes.push_back(global-prevTime);
            prevTime=global;
            totala+=trackTa[j+1];
            j++;
            cout << "j=" << j << endl;
        }
        if ((global==totalb)&!bDone) {
            finalNotes.push_back(trackNb[k]);
            finalTimes.push_back(global-prevTime);
            prevTime=global;
            totalb+=trackTb[k+1];
            k++;
            cout << "k=" << k << endl;
        }
        if ((global==totala)&!aDone) {
            finalNotes.push_back(trackNa[j]);
            finalTimes.push_back(global-prevTime);
            prevTime=global;
            totala+=trackTa[j+1];
            j++;
            cout << "j=" << j << endl;
        }
        if ((global==totalb)&!bDone) {
            finalNotes.push_back(trackNb[k]);
            finalTimes.push_back(global-prevTime);
            prevTime=global;
            totalb+=trackTb[k+1];
            k++;
            cout << "k=" << k << endl;
        }
        if ((global==totala)&!aDone) {
            finalNotes.push_back(trackNa[j]);
            finalTimes.push_back(global-prevTime);
            prevTime=global;
            totala+=trackTa[j+1];
            j++;
            cout << "j=" << j << endl;
        }
        if ((global==totalb)&!bDone) {
            finalNotes.push_back(trackNb[k]);
            finalTimes.push_back(global-prevTime);
            prevTime=global;
            totalb+=trackTb[k+1];
            k++;
            cout << "k=" << k << endl;
        }
        if ((global==totala)&!aDone) {
            finalNotes.push_back(trackNa[j]);
            finalTimes.push_back(global-prevTime);
            prevTime=global;
            totala+=trackTa[j+1];
            j++;
            cout << "j=" << j << endl;
        }
        if ((global==totalb)&!bDone) {
            finalNotes.push_back(trackNb[k]);
            finalTimes.push_back(global-prevTime);
            prevTime=global;
            totalb+=trackTb[k+1];
            k++;
            cout << "k=" << k << endl;
        }
        if ((global==totala)&!aDone) {
            finalNotes.push_back(trackNa[j]);
            finalTimes.push_back(global-prevTime);
            prevTime=global;
            totala+=trackTa[j+1];
            j++;
            cout << "j=" << j << endl;
        }
        if ((global==totalb)&!bDone) {
            finalNotes.push_back(trackNb[k]);
            finalTimes.push_back(global-prevTime);
            prevTime=global;
            totalb+=trackTb[k+1];
            k++;
            cout << "k=" << k << endl;
        }
        if (j>=trackTa.size()) aDone=true;
        if (k>=trackTb.size()) bDone=true;
    }

    /*while (!(aDone&bDone)) {
        if (((totala<totalb)&!aDone)|bDone) {
            finalNotes.push_back(trackNa[j-1]);
            finalTimes.push_back(totala-masterTotal);
            masterTotal+=trackTa[j];
            totala+=trackTa[j];
            j++;
            cout << "Note A noted, j=" << j  << "size=" << trackNa.size() << endl;
            cout << "totala=" << totala << endl;
        } else if (((totalb<totala)&!bDone)|aDone) {
            finalNotes.push_back(trackNb[k-1]);
            finalTimes.push_back(totalb-masterTotal);
            masterTotal+=trackTb[k];
            totalb+=trackTb[k];
            k++;
            cout << "Note B noted, k=" << k << "size=" << trackNb.size() << endl;
            cout << "totalb=" << totalb << endl;
        } else if (totala==totalb) {
            finalNotes.push_back(trackNa[j-1]);
            finalNotes.push_back(trackNb[k-1]);
            finalTimes.push_back(totala-masterTotal);
            finalTimes.push_back(0);
            masterTotal=totala;
            totala+=trackTa[j];
            totalb+=trackTb[k];
            j++;
            k++;
            cout << "Both noted, j=" << j << ", k=" << k << endl;
        }
        if (j>=trackTa.size()) aDone=true;
        if (k>=trackTb.size()) bDone=true;
    }*/

    textFile << "FINAL MIX:" << endl;
    textFile << "Notes:{";
    for (int i=0;i<finalTimes.size();i++) {
        textFile << notary[finalNotes[i]] << ",";
    }
    textFile << "};" << endl;
    textFile << "Times:{";
    for (int i=0;i<finalTimes.size();i++) {
        textFile << finalTimes[i] << ",";
    }
    textFile << "};" << endl;
}

void threeMeld(int a, int b, int c) {
    vector<short> trackNa;
    vector<unsigned long> trackTa;
    vector<short> trackNb;
    vector<unsigned long> trackTb;

    if (a==0) {
        trackNa=channel0N;
        trackTa=channel0T;
    } else if (a==1) {
        trackNa=channel1N;
        trackTa=channel1T;
    } else if (a==2) {
        trackNa=channel2N;
        trackTa=channel2T;
    } else if (a==3) {
        trackNa=channel3N;
        trackTa=channel3T;
    } else if (a==4) {
        trackNa=channel4N;
        trackTa=channel4T;
    } else if (a==5) {
        trackNa=channel5N;
        trackTa=channel5T;
    } else if (a==6) {
        trackNa=channel6N;
        trackTa=channel6T;
    } else if (a==7) {
        trackNa=channel7N;
        trackTa=channel7T;
    } else if (a==8) {
        trackNa=channel8N;
        trackTa=channel8T;
    } else if (a==9) {
        trackNa=channel9N;
        trackTa=channel9T;
    } else if (a==10) {
        trackNa=channel10N;
        trackTa=channel10T;
    } else if (a==11) {
        trackNa=channel11N;
        trackTa=channel11T;
    } else if (a==12) {
        trackNa=channel12N;
        trackTa=channel12T;
    } else if (a==13) {
        trackNa=channel13N;
        trackTa=channel13T;
    } else if (a==14) {
        trackNa=channel14N;
        trackTa=channel14T;
    } else if (a==15) {
        trackNa=channel15N;
        trackTa=channel15T;
    }

    if (b==0) {
        trackNb=channel0N;
        trackTb=channel0T;
    } else if (b==1) {
        trackNb=channel1N;
        trackTb=channel1T;
    } else if (b==2) {
        trackNb=channel2N;
        trackTb=channel2T;
    } else if (b==3) {
        trackNb=channel3N;
        trackTb=channel3T;
    } else if (b==4) {
        trackNb=channel4N;
        trackTb=channel4T;
    } else if (b==5) {
        trackNb=channel5N;
        trackTb=channel5T;
    } else if (b==6) {
        trackNb=channel6N;
        trackTb=channel6T;
    } else if (b==7) {
        trackNb=channel7N;
        trackTb=channel7T;
    } else if (b==8) {
        trackNb=channel8N;
        trackTb=channel8T;
    } else if (b==9) {
        trackNb=channel9N;
        trackTb=channel9T;
    } else if (b==10) {
        trackNb=channel10N;
        trackTb=channel10T;
    } else if (b==11) {
        trackNb=channel11N;
        trackTb=channel11T;
    } else if (b==12) {
        trackNb=channel12N;
        trackTb=channel12T;
    } else if (b==13) {
        trackNb=channel13N;
        trackTb=channel13T;
    } else if (b==14) {
        trackNb=channel14N;
        trackTb=channel14T;
    } else if (b==15) {
        trackNb=channel15N;
        trackTb=channel15T;
    }

    bool aDone=false,bDone=false;
    int i=0,j=0,k=0;
    int prevTime=0;
    unsigned long totala=trackTa[0];
    unsigned long totalb=trackTb[0];
    unsigned long masterTotal=0;
    //vector<short> finalNotes;
    //vector<unsigned long> finalTimes;

    for (int global=0;!(aDone&bDone);global++) {
        if ((global==totala)&!aDone) {
            finalNotes.push_back(trackNa[j]);
            finalTimes.push_back(global-prevTime);
            prevTime=global;
            totala+=trackTa[j+1];
            j++;
            cout << "j=" << j << endl;
        }
        if ((global==totalb)&!bDone) {
            finalNotes.push_back(trackNb[k]);
            finalTimes.push_back(global-prevTime);
            prevTime=global;
            totalb+=trackTb[k+1];
            k++;
            cout << "k=" << k << endl;
        }
        if ((global==totala)&!aDone) {
            finalNotes.push_back(trackNa[j]);
            finalTimes.push_back(global-prevTime);
            prevTime=global;
            totala+=trackTa[j+1];
            j++;
            cout << "j=" << j << endl;
        }
        if ((global==totalb)&!bDone) {
            finalNotes.push_back(trackNb[k]);
            finalTimes.push_back(global-prevTime);
            prevTime=global;
            totalb+=trackTb[k+1];
            k++;
            cout << "k=" << k << endl;
        }
        if ((global==totala)&!aDone) {
            finalNotes.push_back(trackNa[j]);
            finalTimes.push_back(global-prevTime);
            prevTime=global;
            totala+=trackTa[j+1];
            j++;
            cout << "j=" << j << endl;
        }
        if ((global==totalb)&!bDone) {
            finalNotes.push_back(trackNb[k]);
            finalTimes.push_back(global-prevTime);
            prevTime=global;
            totalb+=trackTb[k+1];
            k++;
            cout << "k=" << k << endl;
        }
        if ((global==totala)&!aDone) {
            finalNotes.push_back(trackNa[j]);
            finalTimes.push_back(global-prevTime);
            prevTime=global;
            totala+=trackTa[j+1];
            j++;
            cout << "j=" << j << endl;
        }
        if ((global==totalb)&!bDone) {
            finalNotes.push_back(trackNb[k]);
            finalTimes.push_back(global-prevTime);
            prevTime=global;
            totalb+=trackTb[k+1];
            k++;
            cout << "k=" << k << endl;
        }
        if ((global==totala)&!aDone) {
            finalNotes.push_back(trackNa[j]);
            finalTimes.push_back(global-prevTime);
            prevTime=global;
            totala+=trackTa[j+1];
            j++;
            cout << "j=" << j << endl;
        }
        if ((global==totalb)&!bDone) {
            finalNotes.push_back(trackNb[k]);
            finalTimes.push_back(global-prevTime);
            prevTime=global;
            totalb+=trackTb[k+1];
            k++;
            cout << "k=" << k << endl;
        }
        if ((global==totala)&!aDone) {
            finalNotes.push_back(trackNa[j]);
            finalTimes.push_back(global-prevTime);
            prevTime=global;
            totala+=trackTa[j+1];
            j++;
            cout << "j=" << j << endl;
        }
        if ((global==totalb)&!bDone) {
            finalNotes.push_back(trackNb[k]);
            finalTimes.push_back(global-prevTime);
            prevTime=global;
            totalb+=trackTb[k+1];
            k++;
            cout << "k=" << k << endl;
        }
        if ((global==totala)&!aDone) {
            finalNotes.push_back(trackNa[j]);
            finalTimes.push_back(global-prevTime);
            prevTime=global;
            totala+=trackTa[j+1];
            j++;
            cout << "j=" << j << endl;
        }
        if ((global==totalb)&!bDone) {
            finalNotes.push_back(trackNb[k]);
            finalTimes.push_back(global-prevTime);
            prevTime=global;
            totalb+=trackTb[k+1];
            k++;
            cout << "k=" << k << endl;
        }
        if (j>=trackTa.size()) aDone=true;
        if (k>=trackTb.size()) bDone=true;
    }

    /*while (!(aDone&bDone)) {
        if (((totala<totalb)&!aDone)|bDone) {
            finalNotes.push_back(trackNa[j-1]);
            finalTimes.push_back(totala-masterTotal);
            masterTotal+=trackTa[j];
            totala+=trackTa[j];
            j++;
            cout << "Note A noted, j=" << j  << "size=" << trackNa.size() << endl;
            cout << "totala=" << totala << endl;
        } else if (((totalb<totala)&!bDone)|aDone) {
            finalNotes.push_back(trackNb[k-1]);
            finalTimes.push_back(totalb-masterTotal);
            masterTotal+=trackTb[k];
            totalb+=trackTb[k];
            k++;
            cout << "Note B noted, k=" << k << "size=" << trackNb.size() << endl;
            cout << "totalb=" << totalb << endl;
        } else if (totala==totalb) {
            finalNotes.push_back(trackNa[j-1]);
            finalNotes.push_back(trackNb[k-1]);
            finalTimes.push_back(totala-masterTotal);
            finalTimes.push_back(0);
            masterTotal=totala;
            totala+=trackTa[j];
            totalb+=trackTb[k];
            j++;
            k++;
            cout << "Both noted, j=" << j << ", k=" << k << endl;
        }
        if (j>=trackTa.size()) aDone=true;
        if (k>=trackTb.size()) bDone=true;
    }*/

    textFile << "FINAL MIX:" << endl;
    textFile << "Notes:{";
    for (int i=0;i<finalTimes.size();i++) {
        textFile << notary[finalNotes[i]] << ",";
    }
    textFile << "};" << endl;
    textFile << "Times:{";
    for (int i=0;i<finalTimes.size();i++) {
        textFile << finalTimes[i] << ",";
    }
    textFile << "};" << endl;
}

void outputters(string keepers) {
    if (tofile) {
        DEBUG << keepers << endl;
    } else {
        cout << keepers << endl;
    }
}

void outputterh(string keepers, int number) {
    if (tofile) {
        DEBUG << keepers << hex << number << endl;
    } else {
        cout << keepers << hex << number << endl;
    }
}

void outputterd(string keepers, int number) {
    if (tofile) {
        DEBUG << keepers << dec << number << endl;
    } else {
        cout << keepers << dec << number << endl;
    }
}

void notermenu() {
    char answer;
    int track;
    string input;

    while (true) {
        cout << "Do you wish to have a channel displayed in function call formulation? (y/n)" << endl;
        getline(cin,input);
        if (input.length() == 1) {
            answer=input[0];
            break;
        }
    }

    if (answer=='N'|answer=='n') {
        return;
    }

    while (true) {
        cout << "Choose the channel to formulate (0-15) or select the melded part by entering 16." << endl;
        getline(cin,input);
        stringstream myStream(input);
        if (myStream >> track) {
            if ((track>-1)&(track<17)) break;
        }
    }

    noter(track);

}

void noter(int a) {
    vector<short> trackN;
    vector<unsigned long> trackT;
    unsigned long buffTime;

    if (a==0) {
        trackN=channel0N;
        trackT=channel0T;
    } else if (a==1) {
        trackN=channel1N;
        trackT=channel1T;
    } else if (a==2) {
        trackN=channel2N;
        trackT=channel2T;
    } else if (a==3) {
        trackN=channel3N;
        trackT=channel3T;
    } else if (a==4) {
        trackN=channel4N;
        trackT=channel4T;
    } else if (a==5) {
        trackN=channel5N;
        trackT=channel5T;
    } else if (a==6) {
        trackN=channel6N;
        trackT=channel6T;
    } else if (a==7) {
        trackN=channel7N;
        trackT=channel7T;
    } else if (a==8) {
        trackN=channel8N;
        trackT=channel8T;
    } else if (a==9) {
        trackN=channel9N;
        trackT=channel9T;
    } else if (a==10) {
        trackN=channel10N;
        trackT=channel10T;
    } else if (a==11) {
        trackN=channel11N;
        trackT=channel11T;
    } else if (a==12) {
        trackN=channel12N;
        trackT=channel12T;
    } else if (a==13) {
        trackN=channel13N;
        trackT=channel13T;
    } else if (a==14) {
        trackN=channel14N;
        trackT=channel14T;
    } else if (a==15) {
        trackN=channel15N;
        trackT=channel15T;
    } else if (a==16) {
        trackN=finalNotes;
        trackT=finalTimes;
    }

    textFile << endl << endl << "FUNCTION CALL FORMULATION" << endl;

    for (int i=1;i<trackT.size();) {
        if ((trackT[i]<vol)&(trackT[i+3]<vol)&(trackT[i+2]<vol)&(trackT[i+1]<vol)&(trackT[i+4]<vol)&(trackT[i+5]<vol)) {
            i+=7;
            cout << "too many notes at once!" << endl;
        }

        else if ((trackT[i]<vol)&(trackT[i+2]<vol)&(trackT[i+1]<vol)&(trackT[i+3]<vol)&(trackT[i+4]<vol)) {
            buffTime=trackT[i]+trackT[i+2]+trackT[i+1]+trackT[i+3]+trackT[i+4]+trackT[i+5]-vol;
            if (buffTime<0) {
                buffTime=0;
            }
            textFile<<"play6("<<notary[trackN[i-1]]<<","<<notary[trackN[i]]<<","<<notary[trackN[i+1]]<<","<<notary[trackN[i+2]]<<","<<notary[trackN[i+3]]<<","<<notary[trackN[i+4]]<<","<<buffTime<<");"<<endl;
            i+=6;
        }

        else if ((trackT[i]<vol)&(trackT[i+2]<vol)&(trackT[i+1]<vol)&(trackT[i+3]<vol)) {
            buffTime=trackT[i]+trackT[i+2]+trackT[i+1]+trackT[i+3]+trackT[i+4]-vol;
            if (buffTime<0) {
                buffTime=0;
            }
            textFile<<"play5("<<notary[trackN[i-1]]<<","<<notary[trackN[i]]<<","<<notary[trackN[i+1]]<<","<<notary[trackN[i+2]]<<","<<notary[trackN[i+3]]<<","<<buffTime<<");"<<endl;
            i+=5;
        }

        else if ((trackT[i]<vol)&(trackT[i+2]<vol)&(trackT[i+1]<vol)) {
            buffTime=trackT[i]+trackT[i+2]+trackT[i+1]+trackT[i+3]-vol;
            if (buffTime<0) {
                buffTime=0;
            }
            textFile<<"play4("<<notary[trackN[i-1]]<<","<<notary[trackN[i]]<<","<<notary[trackN[i+1]]<<","<<notary[trackN[i+2]]<<","<<buffTime<<");"<<endl;
            i+=4;
        }

        else if ((trackT[i]<vol)&(trackT[i+1]<vol)) {
            buffTime=trackT[i]+trackT[i+2]+trackT[i+1]-vol;
            if (buffTime<0) {
                buffTime=0;
            }
            textFile<<"play3("<<notary[trackN[i-1]]<<","<<notary[trackN[i]]<<","<<notary[trackN[i+1]]<<","<<buffTime<<");"<<endl;
            i+=3;
        }

        else if (trackT[i]<vol) {
            buffTime=trackT[i]+trackT[i+1]-vol;
            if (buffTime<0) {
                buffTime=0;
            }
            textFile<<"play2("<<notary[trackN[i-1]]<<","<<notary[trackN[i]]<<","<<buffTime<<");"<<endl;
            i+=2;
        }

        else {
            buffTime=trackT[i]-vol;
            if (buffTime<0) {
                buffTime=0;
            }
            textFile<<"play1("<<notary[trackN[i-1]]<<","<<buffTime<<");"<<endl;
            i+=1;
        }
    }
}

void keyChangeMenu(){

    cout << endl << endl << endl << "Robospiel range: g0 to c3" << endl << endl;
    cout << "0) " << instrument0 << endl << notary[highest0] << "," << notary[lowest0] << "," << highest0-lowest0 << endl;
    cout << "1) " << instrument1 << endl << notary[highest1] << "," << notary[lowest1] << "," << highest1-lowest1 << endl;
    cout << "2) " << instrument2 << endl << notary[highest2] << "," << notary[lowest2] << "," << highest2-lowest2 << endl;
    cout << "3) " << instrument3 << endl << notary[highest3] << "," << notary[lowest3] << "," << highest3-lowest3 << endl;
    cout << "4) " << instrument4 << endl << notary[highest4] << "," << notary[lowest4] << "," << highest4-lowest4 << endl;
    cout << "5) " << instrument5 << endl << notary[highest5] << "," << notary[lowest5] << "," << highest5-lowest5 << endl;
    cout << "6) " << instrument6 << endl << notary[highest6] << "," << notary[lowest6] << "," << highest6-lowest6 << endl;
    cout << "7) " << instrument7 << endl << notary[highest7] << "," << notary[lowest7] << "," << highest7-lowest7 << endl;
    cout << "8) " << instrument8 << endl << notary[highest8] << "," << notary[lowest8] << "," << highest8-lowest8 << endl;
    cout << "10) " << instrument10 << endl << notary[highest10] << "," << notary[lowest10] << "," << highest10-lowest10 << endl;
    cout << "11) " << instrument11 << endl << notary[highest11] << "," << notary[lowest11] << "," << highest11-lowest11 << endl;
    cout << "12) " << instrument12 << endl << notary[highest12] << "," << notary[lowest12] << "," << highest12-lowest12 << endl;
    cout << "13) " << instrument13 << endl << notary[highest13] << "," << notary[lowest13] << "," << highest13-lowest13 << endl;
    cout << "14) " << instrument14 << endl << notary[highest14] << "," << notary[lowest14] << "," << highest14-lowest14 << endl;
    cout << "15) " << instrument15 << endl << notary[highest15] << "," << notary[lowest15] << "," << highest15-lowest15 << endl;

    string input = "";
    char answer;
    int channel;
    int keyShift;
    while (true) {
        cout << "Would you like to perform a key change? (y/n)" << endl;
        getline(cin,input);
        if (input.length() == 1) {
            answer=input[0];
            break;
        }
    }
    if (answer=='N'|answer=='n') {
        return;
    } else {
        while (true) {
            cout << "Which channel would you like to modulate? (0-15 excluding 9)" << endl;
            getline(cin,input);
            stringstream myStream(input);
            if (myStream >> channel) {
                if ((channel>-1)&(channel<16)&(channel!=9)) break;
            }
        }
        while (true) {
            cout << "How many half-steps should it shift? Positive for up, negative for down (no more than 11 either direction) (DO NOT INCLUDE A +, ONLY A -)." << endl;
            getline(cin,input);
            stringstream myStream(input);
            if (myStream >> keyShift) {
                if ((keyShift>-12)&(keyShift<12)) break;
            }
        }
    }
    keyChange(keyShift,channel);
    keyChangeMenu();
}

void keyChange(int keyShift,int channel){
    if (channel==0) {
        highest0+=keyShift;
        lowest0+=keyShift;
        for (int i=0;i<channel0N.size();i++) {
            channel0N[i]+=keyShift;
        }
    } else if (channel==1) {
        highest1+=keyShift;
        lowest1+=keyShift;
        for (int i=0;i<channel1N.size();i++) {
            channel1N[i]+=keyShift;
        }
    } else if (channel==2) {
        highest2+=keyShift;
        lowest2+=keyShift;
        for (int i=0;i<channel2N.size();i++) {
            channel2N[i]+=keyShift;
        }
    } else if (channel==3) {
        highest3+=keyShift;
        lowest3+=keyShift;
        for (int i=0;i<channel3N.size();i++) {
            channel3N[i]+=keyShift;
        }
    } else if (channel==4) {
        highest4+=keyShift;
        lowest4+=keyShift;
        for (int i=0;i<channel4N.size();i++) {
            channel4N[i]+=keyShift;
        }
    } else if (channel==5) {
        highest5+=keyShift;
        lowest5+=keyShift;
        for (int i=0;i<channel5N.size();i++) {
            channel5N[i]+=keyShift;
        }
    } else if (channel==6) {
        highest6+=keyShift;
        lowest6+=keyShift;
        for (int i=0;i<channel6N.size();i++) {
            channel6N[i]+=keyShift;
        }
    } else if (channel==7) {
        highest7+=keyShift;
        lowest7+=keyShift;
        for (int i=0;i<channel7N.size();i++) {
            channel7N[i]+=keyShift;
        }
    } else if (channel==8) {
        highest8+=keyShift;
        lowest8+=keyShift;
        for (int i=0;i<channel8N.size();i++) {
            channel8N[i]+=keyShift;
        }
    }  else if (channel==10) {
        highest10+=keyShift;
        lowest10+=keyShift;
        for (int i=0;i<channel10N.size();i++) {
            channel10N[i]+=keyShift;
        }
    } else if (channel==11) {
        highest11+=keyShift;
        lowest11+=keyShift;
        for (int i=0;i<channel11N.size();i++) {
            channel11N[i]+=keyShift;
        }
    } else if (channel==12) {
        highest12+=keyShift;
        lowest12+=keyShift;
        for (int i=0;i<channel12N.size();i++) {
            channel12N[i]+=keyShift;
        }
    } else if (channel==13) {
        highest13+=keyShift;
        lowest13+=keyShift;
        for (int i=0;i<channel13N.size();i++) {
            channel13N[i]+=keyShift;
        }
    } else if (channel==14) {
        highest14+=keyShift;
        lowest14+=keyShift;
        for (int i=0;i<channel14N.size();i++) {
            channel14N[i]+=keyShift;
        }
    } else if (channel==15) {
        highest15+=keyShift;
        lowest15+=keyShift;
        for (int i=0;i<channel15N.size();i++) {
            channel15N[i]+=keyShift;
        }
    }
}
