#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define mostSignificantBit (UINT8_MAX/2)+1

typedef struct page {
    int id;
    int positionInRealMemory;
    int positionInSwapMemory;
    uint8_t relevancy;
} page;

/*
 * Initializes a page with the desired ID and position in real memory (-1 if it is not in real memory)
 */
page initializePage(int pageId, int positionInRealMemory, int positionInSwapMemory);

/*
 * Runs the whole program. Properly explained inside function
 */
void initializeNFUAlgorithm(int vmSize, int rmSize, int clock);

/*
 * Finds and returns the page with the longest idle time (lowest relevancy) on the page array (memory)
 */
page findLeastFrequentlyUsedPage(page *virtualMemory, int vmSize, int rmSize);

/*
 * Receives page array and its size and prints all its members
 */
void printMemory(page *memory, int size);

/*
 * Receives a page and prints its attributes
 */
void printPage(page page);

/*
 * Auxiliary function used for analysis
 */
void nfuaAlgorithmAnalysis(int vmSize, int rmSize, int clocks, int interactions);

/*
 * Auxiliary function used to get and random based on the relevancy
 */
int biasedRand(double relevancy);

FILE *f;


int main() {
    int vmSize, rmSize, clock;
    printf("To initialize the simulation of the Not Frequently Used Algorithm, please enter:\n- Size of the Virtual Memory (VM);\n- The size of the Real Memory (RM);\n- The amount of clocks you want the algorithm to run for.\n");
    printf("Please note that, in order for the algorithm to run properly, the following must be true:\n |VM| > |RM|.\n");
    scanf("%d %d %d", &vmSize, &rmSize, &clock);
    if (vmSize <= rmSize) {
        printf("Virtual Memory must be bigger than real memory. Please try again.\n");
        exit(0);
    }
    f = fopen("output.csv", "w");
    initializeNFUAlgorithm(vmSize, rmSize, clock);
//    nfuaAlgorithmAnalysis(vmSize, rmSize, clock, 20);
    return 0;
}

page initializePage(int pageId, int positionInRealMemory, int positionInSwapMemory) {
    page newPage;
    newPage.id = pageId;
    newPage.positionInRealMemory = positionInRealMemory;
    newPage.positionInSwapMemory = positionInSwapMemory;
    newPage.relevancy = 0;
    return newPage;
}

void initializeNFUAlgorithm(int vmSize, int rmSize, int clock) {
    fprintf(f, "clockTick,hitCount,missCount\n");

    int missCount = 0;
    int hitCount = 0;

    srand(time(NULL));
    page *virtualMemory = malloc(sizeof(page) * vmSize);
    int *R = malloc(sizeof(int) * vmSize);

    //Populates virtualMemory with newly created pages
    for (int i = 0; i < vmSize; i++) {
        int positionInRealMemory = (i < rmSize) ? i : -1;
        int positionInSwapMemory = (i >= rmSize) ? i - rmSize : -1;
        virtualMemory[i] = initializePage(i, positionInRealMemory, positionInSwapMemory);
    }

    printf("Memoria Virtual montada:\n");
    printMemory(virtualMemory, vmSize);

    //Runs the Not Frequently Used Algorithm for as many times as the user wishes
    for (int i = 0; i < clock; i++) {
        for (int j = 0; j < vmSize; j++) {
            R[j] = biasedRand((double) (virtualMemory[j].relevancy));
//            R[j] = rand() % 2;

            virtualMemory[j].relevancy = (virtualMemory[j].relevancy >> 1) + R[j] * (mostSignificantBit);
            if (R[j] == 1) printf("Page %d will be used on clock %d.\n", j, i);
        }
        printf("Memoria Virtual com relevancias aplicadas:\n");
        printMemory(virtualMemory, vmSize);
        for (int j = 0; j < vmSize; j++) {
            //For each page, randomly decides if it is going to be accessed during this clock
            if (R[j] == 1) {
                printf("Attempting to use page %d on clock number %d...\n", j, i);
                /*
                 * Checks if the page is already on the real memory. If it is, simply updates relevancy to register that it was requested.
                 * Otherwise, finds the least frequently used page on the memory and swaps it with the requested page on the swap memory.
                 */
                if (virtualMemory[j].positionInRealMemory == -1) {
                    page leastFrequentlyUsedPage = findLeastFrequentlyUsedPage(virtualMemory, vmSize, rmSize);
                    page auxiliarySwapPage = virtualMemory[j];
                    missCount++;

                    virtualMemory[j].positionInRealMemory = leastFrequentlyUsedPage.positionInRealMemory;
                    virtualMemory[j].positionInSwapMemory = -1;

                    virtualMemory[leastFrequentlyUsedPage.id].positionInRealMemory = -1;
                    virtualMemory[leastFrequentlyUsedPage.id].positionInSwapMemory = auxiliarySwapPage.positionInSwapMemory;
                    printf("Page %d swapped places with page %d, who was the LFU page.\n", j,
                           leastFrequentlyUsedPage.id);
                } else {
                    hitCount++;
                }
            }
        }
        printf("Memoria Virtual apos todos os swaps do clock:\n");
        printMemory(virtualMemory, vmSize);

        fprintf(f, "%d, %d, %d\n", i, hitCount, missCount);
    }
}

page findLeastFrequentlyUsedPage(page *virtualMemory, int vmSize, int rmSize) {
    page leastFrequentlyUsedPage = initializePage(vmSize, rmSize, vmSize - rmSize);
    leastFrequentlyUsedPage.relevancy = UINT8_MAX;
    for (int i = 0; i < vmSize; i++) {
        if (virtualMemory[i].positionInRealMemory >= 0 &&
            virtualMemory[i].relevancy < leastFrequentlyUsedPage.relevancy) {
            leastFrequentlyUsedPage = virtualMemory[i];
        }
    }
    return leastFrequentlyUsedPage;
}

void printMemory(page *memory, int size) {
    printf("================================================================================================\n");
    for (int i = 0; i < size; i++) {
        printPage(memory[i]);
    }
    printf("================================================================================================\n");
}

void printPage(page page) {
    printf("| ID: %-5d | Position in Real Memory: %-5d | Position in Swap Memory: %-5d | Relevancy: %-3d |\n", page.id,
           page.positionInRealMemory, page.positionInSwapMemory, page.relevancy);
}

void nfuaAlgorithmAnalysis(int vmSize, int rmSize, int clocks, int interactions) {
    int i;
    f = fopen("output.csv", "w");
    fprintf(f, "hitCount,missCount\n");
    for (i = 0; i < interactions; i++) {
        initializeNFUAlgorithm(vmSize, rmSize, clocks);
    }
}

int biasedRand(double relevancy) {
    int random = rand() % 255;
    if (random < relevancy + 1) {
        return 1;
    }
    return 0;
}
