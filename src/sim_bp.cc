#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstring>
#include "sim_bp.h"
#include "BranchPredictor.cpp"

/*  argc holds the number of command line arguments
    argv[] holds the commands themselves

    Example:-
    sim bimodal 6 gcc_trace.txt
    argc = 4
    argv[0] = "sim"
    argv[1] = "bimodal"
    argv[2] = "6"
    ... and so on
*/
int main (int argc, char* argv[])
{
    FILE *FP;               // File handler
    char *trace_file;       // Variable that holds trace file name;
    bp_params params;       // look at sim_bp.h header file for the the definition of struct bp_params
    char outcome;           // Variable holds branch outcome
    unsigned long int addr; // Variable holds the address read from input file
    
    if (!(argc == 4 || argc == 5 || argc == 7))
    {
        printf("Error: Wrong number of inputs:%d\n", argc-1);
        exit(EXIT_FAILURE);
    }
    
    params.bp_name  = argv[1];
    
    // strtoul() converts char* to unsigned long. It is included in <stdlib.h>
    if(strcmp(params.bp_name, "bimodal") == 0)              // Bimodal
    {
        if(argc != 4)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.M2       = strtoul(argv[2], NULL, 10);
        trace_file      = argv[3];
        printf("COMMAND\n%s %s %lu %s\n", argv[0], params.bp_name, params.M2, trace_file);
        params.M1 = 0;
        params.N = 0;
        params.K = 0;
    }
    else if(strcmp(params.bp_name, "gshare") == 0)          // Gshare
    {
        if(argc != 5)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.M1       = strtoul(argv[2], NULL, 10);
        params.N        = strtoul(argv[3], NULL, 10);
        trace_file      = argv[4];
        printf("COMMAND\n%s %s %lu %lu %s\n", argv[0], params.bp_name, params.M1, params.N, trace_file);
        params.K = 0;
        params.M2 = 0;

    }
    else if(strcmp(params.bp_name, "hybrid") == 0)          // Hybrid
    {
        if(argc != 7)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.K        = strtoul(argv[2], NULL, 10);
        params.M1       = strtoul(argv[3], NULL, 10);
        params.N        = strtoul(argv[4], NULL, 10);
        params.M2       = strtoul(argv[5], NULL, 10);
        trace_file      = argv[6];
        printf("COMMAND\n%s %s %lu %lu %lu %lu %s\n", argv[0], params.bp_name, params.K, params.M1, params.N, params.M2, trace_file);

    }
    else
    {
        printf("Error: Wrong branch predictor name:%s\n", params.bp_name);
        exit(EXIT_FAILURE);
    }
    
    // Open trace_file in read mode
    FP = fopen(trace_file, "r");
    if(FP == NULL)
    {
        // Throw error and exit if fopen() failed
        printf("Error: Unable to open file %s\n", trace_file);
        exit(EXIT_FAILURE);
    }
    
    char str[2];
    // unsigned long int indexBits = 0;
    unsigned long int counterInitVal = 2;
    std::vector<unsigned long int>* pTab = nullptr;
    std::vector<unsigned long int>* bipTab = nullptr;
    std::vector<unsigned long int>* gspTab = nullptr;
    if (std::strcmp(params.bp_name, "bimodal") == 0) {
        pTab = new std::vector<unsigned long int>(getPredictionTable(params.M2, counterInitVal));
    }
    else if (std::strcmp(params.bp_name, "gshare") == 0) {
        pTab = new std::vector<unsigned long int>(getPredictionTable(params.M1, counterInitVal));
    }
    else if (std::strcmp(params.bp_name, "hybrid") == 0) {
        counterInitVal = 1;
        bipTab = new std::vector<unsigned long int>(getPredictionTable(params.M2));
        gspTab = new std::vector<unsigned long int>(getPredictionTable(params.M1));
          pTab = new std::vector<unsigned long int>(getPredictionTable(params.K, counterInitVal));
    }
    unsigned long int globalBHR = ((1 << params.N) - 1) & 0;
    while(fscanf(FP, "%lx %s", &addr, str) != EOF)
    {   
        outcome = str[0];
        // if (outcome == 't')
        //     printf("%lx %s\n", addr, "t");           // Print and test if file is read correctly
        // else if (outcome == 'n')
        //     printf("%lx %s\n", addr, "n");          // Print and test if file is read correctly
        /*************************************
            Add branch predictor code here
        **************************************/
       if (std::strcmp(params.bp_name, "bimodal") == 0) {
        if (pTab != nullptr) {
            bimodalPredict(addr, outcome, pTab, params.M2);
        }
        else {
            printf("Could not create prediction table\n");
            }
       }
       else if (std::strcmp(params.bp_name, "gshare") == 0) {
        if (pTab != nullptr) {
            globalBHR = gsharePredict(addr, outcome, pTab, params.M1, params.N, globalBHR);
        }
        else {
            printf("Could not create prediction table\n");
        }
       }
       else if (std::strcmp(params.bp_name, "hybrid") == 0) {
        if (bipTab != nullptr && gspTab != nullptr && pTab != nullptr) {
            globalBHR = hybridPredict(addr, outcome, bipTab, gspTab, pTab, params.M2, params.M1, params.N, params.K, globalBHR);
        }
        else {
            printf("Could not create prediction table\n");
        }
       }
    }
    // Print final contents
    if (std::strcmp(params.bp_name, "bimodal") == 0) printBiModalContents(pTab, params.bp_name);
    else if (std::strcmp(params.bp_name, "gshare") == 0) printGShareContents(pTab, params.bp_name);
    else if (std::strcmp(params.bp_name, "hybrid") == 0) printHybridContents(bipTab, gspTab, pTab, params.bp_name);
    // Delete dyanmically allocated variables
    delete pTab;
    delete bipTab;
    delete gspTab;
    return 0;
}
