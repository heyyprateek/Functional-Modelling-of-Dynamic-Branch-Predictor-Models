#include <iostream>
#include <vector>
#include <algorithm>
#include <vector>
#include <cstdarg> // Include the cstdarg header for variable argument handling
#include <cmath>
#include <bitset>
#include <chrono>

// Global stat variables
struct PredictionStatus {
    unsigned long int mispredictions;
    double mispredictionRate;
};
PredictionStatus bimodalPredStat = {0, 0.0};
PredictionStatus gsharePredStat =  {0, 0.0};
PredictionStatus hybridPredStat =  {0, 0.0};

// Total address count 
unsigned long int addrCount = 0;

// Enable/disable debug prints using DEBUG macro
# define DEBUG 0

void debugPrint(const char* format, ...) {
   #if DEBUG
      va_list args;
      va_start(args, format);
      vprintf(format, args);
      va_end(args);
   #endif
}

// Generic method to extract bits between a start and end position
unsigned long int extractBits(unsigned long int num, unsigned long int start, unsigned long int end) {
    // Create a mask to extract the desired bits
    unsigned long int mask = (1 << (end - start + 1)) - 1;
    mask <<= start;
    // Use the mask to extract the bits
    unsigned long int value = num & mask;
    value >>= start;
    return value;
}
// Print Contents
void printBiModalContents(std::vector<unsigned long int>* predTabPtr, char* bp_name) {
    std::string bp_name_upper;
    while(*bp_name) {
        bp_name_upper += std::toupper(*bp_name);
        bp_name++;
    }
    printf("OUTPUT\n");
    printf(" number of predictions:    %lu\n", addrCount);
    printf(" number of mispredictions: %lu\n", bimodalPredStat.mispredictions);
    printf(" misprediction rate:       %.2f%%\n", bimodalPredStat.mispredictionRate);
    printf("FINAL %s CONTENTS\n", bp_name_upper.c_str());
    for (size_t i = 0; i < predTabPtr->size(); ++i) {
        printf("%zu %lu\n", i, (*predTabPtr)[i]);
    }
}

void printGShareContents(std::vector<unsigned long int>* predTabPtr, char* bp_name) {
    std::string bp_name_upper;
    while(*bp_name) {
        bp_name_upper += std::toupper(*bp_name);
        bp_name++;
    }
    printf("OUTPUT\n");
    printf(" number of predictions:    %lu\n", addrCount);
    printf(" number of mispredictions: %lu\n", gsharePredStat.mispredictions);
    printf(" misprediction rate:       %.2f%%\n", gsharePredStat.mispredictionRate);
    printf("FINAL %s CONTENTS\n", bp_name_upper.c_str());
    for (size_t i = 0; i < predTabPtr->size(); ++i) {
        printf("%zu %lu\n", i, (*predTabPtr)[i]);
    }
}

void printHybridContents(std::vector<unsigned long int>* bipredTabPtr,
                         std::vector<unsigned long int>* gspredTabPtr, 
                         std::vector<unsigned long int>* predTabPtr, 
                         char* bp_name) {
    // Print chooser contents
    std::string bp_name_upper;
    while(*bp_name) {
        bp_name_upper += std::toupper(*bp_name);
        bp_name++;
    }
    printf("OUTPUT\n");
    printf(" number of predictions:    %lu\n", addrCount);
    printf(" number of mispredictions: %lu\n", hybridPredStat.mispredictions);
    printf(" misprediction rate:       %.2f%%\n", hybridPredStat.mispredictionRate);
    printf("FINAL CHOOSER CONTENTS\n");
    for (size_t i = 0; i < predTabPtr->size(); ++i) {
        printf("%zu %lu\n", i, (*predTabPtr)[i]);
    }
    // Print gshare counter
    printf("FINAL GSHARE CONTENTS\n");
    for (size_t i = 0; i < gspredTabPtr->size(); ++i) {
        printf("%zu %lu\n", i, (*gspredTabPtr)[i]);
    }

    // Print bimodal counter
    printf("FINAL BIMODAL CONTENTS\n");
    for (size_t i = 0; i < bipredTabPtr->size(); ++i) {
        printf("%zu %lu\n", i, (*bipredTabPtr)[i]);
    }
}

// Update Counter
void updateCounter(std::string bp_name,
                   char actualOutcome,
                   unsigned long int counter,
                   std::vector<unsigned long int>* predTabPtr, 
                   unsigned long int index) {
    if (actualOutcome == 't') {
        if (counter < 3) {
            counter++;
        }
    }
    if (actualOutcome == 'n') {
        if (counter > 0) {
            counter--;
        }
    }
    (*predTabPtr)[index] = counter;
    if (bp_name == "bimodal") debugPrint("\tBU: %lu %lu\n", index, counter);
    if (bp_name == "gshare")  debugPrint("\tGU: %lu %lu\n", index, counter);
}

// Perform a prediction
char getPrediction(unsigned long int counter) {
    char value = (char) 0;
    if (counter >= 2) {
        value = 't';
    }
    else
    {
        value = 'n';
    }
    return value;
}

unsigned long int getCounter(unsigned long int index, const std::vector<unsigned long int>& predTable) {
    if (index >= 0 && index < predTable.size()) {
        return predTable[index];
    }
    return static_cast<unsigned long int>(-1); // Return unsigned -1 instead of signed -1
}

// Get prediction table
std::vector<unsigned long int> getPredictionTable(unsigned long int pcBits, unsigned long int initVal=2) {
    std::vector<unsigned long int> predTable(pow(2, pcBits), initVal);
    return predTable;
}

std::vector<unsigned long int> getHybridPredictionTable(unsigned long int pcBits) {
    std::vector<unsigned long int> predTable(pow(2, pcBits), 1);
    return predTable;
}

// Bimodal prediction
void bimodalPredict(unsigned long int addr,
                    char outcome, 
                    std::vector<unsigned long int>* pTab, 
                    unsigned long int M2) {
    
    // Increment predictions
    debugPrint("=%lu %x %c\n", addrCount, addr, outcome);
    addrCount++;
    
    // Evaluate index
    // for bimodal it is the lower order PC bits m+1 downto 2
    unsigned long int index = extractBits(addr, 2, M2+1);
    // Fetch the counter
    // unsigned long int counter = getCounter(index, *pTab);
    unsigned long int counter = getCounter(index, *pTab);
    debugPrint("\tBP: %lu %lu\n",index, counter);
    // Make prediction
    char pred = getPrediction(counter);
    // Update Counter
    updateCounter("bimodal", outcome, counter, pTab, index);
    // Update misprediction stats
    if (pred != outcome) {
        bimodalPredStat.mispredictions += 1;
        bimodalPredStat.mispredictionRate = 100*double(bimodalPredStat.mispredictions)/double(addrCount);
    }
    // return pTab;
}

// Gshare prediction
unsigned long int gsharePredict(unsigned long int addr,
                    char outcome, 
                    std::vector<unsigned long int>* pTab, 
                    unsigned long int M1,
                    unsigned long int N,
                    unsigned long int globalBHR) {
    // Increment predictions
    debugPrint("=%lu %lu %c\n", addrCount, addr, outcome);
    addrCount++;

    // Evaluate index
    // for gshare it is bitwise XOR of:
    //      n bits of BHR and upper n bits of lower order PC bits m+1 downto 2
    unsigned long int upperPCBHRBitsXORed = globalBHR ^ extractBits(addr, M1+2-N, M1+1);
    unsigned long int lowerPCSansBHRBits = extractBits(addr, 2, M1-N+1);
    unsigned long int indexMask = upperPCBHRBitsXORed << (M1-N);
    unsigned long int index = indexMask | lowerPCSansBHRBits;

    // Fetch the counter
    unsigned long int counter = getCounter(index, *pTab);

    // Fetch the counter
    debugPrint("\tGP: %lu %lu\n", index, counter);

    // Make prediction
    char pred = getPrediction(counter);

    // Update Counter
    updateCounter("gshare", outcome, counter, pTab, index);

    // Update misprediction stats
    if (pred != outcome) {
        gsharePredStat.mispredictions += 1;
        gsharePredStat.mispredictionRate = 100*double(gsharePredStat.mispredictions)/double(addrCount);
    }
    // Update the global branch history register
    // Right shift by 1
    unsigned long int mask;
    if (N > 0 ) {
        globalBHR >>= 1;
        mask = 1;
        if (outcome == 't') {
            mask = 1 << (N - 1);
            globalBHR |= mask; // Set MSB to 1
        }
        else if (outcome == 'n') {
            mask = ((1 << N) - 1) >> 1;
            globalBHR &= mask; // Set MSB to 0
        }
    }
    else {
        globalBHR = 0;
    }
    return globalBHR;
}

unsigned long int hybridPredict(unsigned long int addr, 
                                char outcome,
                                std::vector<unsigned long int>* bipTab, 
                                std::vector<unsigned long int>* gspTab, 
                                std::vector<unsigned long int>* pTab, 
                                unsigned long int M2, 
                                unsigned long int M1, 
                                unsigned long int N, 
                                unsigned long int K, 
                                unsigned long int globalBHR) {

    // Increment predictions
    debugPrint("=%lu %x %c\n", addrCount, addr, outcome);
    addrCount++;

    // ------------ Make gshare prediction ------------
    // Evaluate index
    // for gshare it is bitwise XOR of:
    //      n bits of BHR and upper n bits of lower order PC bits m+1 downto 2
    unsigned long int upperPCBHRBitsXORed = globalBHR ^ extractBits(addr, M1+2-N, M1+1);
    unsigned long int lowerPCSansBHRBits = extractBits(addr, 2, M1-N+1);
    unsigned long int indexMask = upperPCBHRBitsXORed << (M1-N);
    unsigned long int gsindex = indexMask | lowerPCSansBHRBits;
    // Fetch the counter
    unsigned long int gscounter = getCounter(gsindex, *gspTab);
    debugPrint("\tGP: %lu %lu\n", gsindex, gscounter);
    // Make prediction
    char gspred = getPrediction(gscounter);
    debugPrint("gspred %c\n", gspred);

    // ------------ Make a bimodal prediction ------------
    // Evaluate index
    // for bimodal it is the lower order PC bits m+1 downto 2
    unsigned long int biindex = extractBits(addr, 2, M2+1);
    // Fetch the counter
    unsigned long int bicounter = getCounter(biindex, *bipTab);
    debugPrint("\tBP: %lu %lu\n",biindex, bicounter);
    // Make prediction
    char bipred = getPrediction(bicounter);
    debugPrint("bipred %c\n", bipred);

    // ------------ Make chooser table prediction ------------
    // Evaluate index
    // lower order PC bits from k+1 downto 2
    unsigned long int index = extractBits(addr, 2, K+1);
    // Fetch the counter
    unsigned long int counter =  getCounter(index, *pTab);
    debugPrint("\tCP: %lu %lu\n",index, counter);
    // Make prediction
    char pred = (char) 0;
    if (counter >= 2 ) {
        pred = gspred;
        // Update gshare counter
        updateCounter("gshare", outcome, gscounter, gspTab, gsindex);
    }
    else {
        pred=bipred;
        // Update bimodal counter
        updateCounter("bimodal", outcome, bicounter, bipTab, biindex);
    }

    // ------------ Update the global branch history register ------------
    // Right shift by 1
    unsigned long int mask;
    if (N > 0 ) {
        globalBHR >>= 1;
        mask = 1;
        if (outcome == 't') {
            mask = 1 << (N - 1);
            globalBHR |= mask; // Set MSB to 1
        }
        else if (outcome == 'n') {
            mask = ((1 << N) - 1) >> 1;
            globalBHR &= mask; // Set MSB to 0
        }
    }
    else {
        globalBHR = 0;
    }
    // ------------ Update counter ------------
    if ((gspred != outcome && bipred != outcome) || ((gspred == outcome) && (bipred == outcome))) {
        counter = counter;
    }
    else if (gspred == outcome && bipred != outcome) {
        if (counter < 3){
            counter++;
        }
    }
    else if (gspred != outcome && bipred == outcome) {
        if (counter > 0) {
            counter--;
        }
    }
    (*pTab)[index] = counter;
    // ------------ Update misprediction stats ------------
    // printf("\t-------- prediction: %c, actual: %c, miss:%d\n", pred, outcome, pred != outcome);
    if (pred != outcome) {
        hybridPredStat.mispredictions += 1;
        hybridPredStat.mispredictionRate = 100*double(hybridPredStat.mispredictions)/double(addrCount);
    }
    return globalBHR;
}