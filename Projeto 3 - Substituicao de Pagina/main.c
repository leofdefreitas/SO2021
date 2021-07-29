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

int main() {
    int vmSize, rmSize, clock;
    printf("To initialize the simulation of the Not Frequently Used Algorithm, please enter:\n- Size of the Virtual Memory (VM);\n- The size of the Real Memory (RM);\n- The amount of clocks you want the algorithm to run for.\n");
    printf("Please note that, in order for the algorithm to run properly, the following must be true:\n |VM| > |RM|.\n");
    scanf("%d %d %d", &vmSize, &rmSize, &clock);
    if (vmSize <= rmSize) {
        printf("Virtual Memory must be bigger than real memory. Please try again.\n");
        exit(0);
    }
    initializeNFUAlgorithm(vmSize, rmSize, clock);
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
    srand(time(NULL)); 
    page *virtualMemory = malloc(sizeof(page) * vmSize);
    int *R = malloc(sizeof(int) * vmSize);

    //Populates virtualMemory with newly created pages
    for (int i=0; i < vmSize; i++) {
        int positionInRealMemory = (i < rmSize) ? i : -1;
        int positionInSwapMemory = (i >= rmSize) ? i-rmSize : -1;
        virtualMemory[i] = initializePage(i, positionInRealMemory, positionInSwapMemory);
    }
    
    printf("Memoria Virtual montada:\n");
    printMemory(virtualMemory, vmSize);
    
    //Runs the Not Frequently Used Algorithm for as many times as the user wishes
    for (int i=0; i < clock; i++) {
        for (int j=0; j < vmSize; j++) {
            R[j] = rand() % 2;
            virtualMemory[j].relevancy = (virtualMemory[j].relevancy >> 1) + R[j]*(mostSignificantBit); 
            if (R[j] == 1) printf("Page %d will be used on clock %d.\n", j, i);
        }
        printf("Memoria Virtual com relevancias aplicadas:\n");
        printMemory(virtualMemory, vmSize);
        for (int j=0; j < vmSize; j++) {
            //For each page, randomly decides if it is going to be accessed during this clock
            if (R[j] == 1) {
                printf("Attempting to use page %d on clock number %d...\n", j, i);
                /*
                 * Checks if the page is already on the real memory. If it is, simply updates relevancy to register that it was requested.
                 * Otherwise, finds the least frequently used page on the memory and swaps it with the requested page on the swap memory.
                 */
                if (virtualMemory[j].positionInRealMemory == -1) {
                    /* virtualMemory[j].relevancy = (virtualMemory[j].relevancy >> 1) + UINT8_MAX/2;
                    printf("Page was already on real memory. It was succesfully used and its relevancy was updated.\n"); */
                //} else {
                    page leastFrequentlyUsedPage = findLeastFrequentlyUsedPage(virtualMemory, vmSize, rmSize);
                    printf("LFU Page:\n");
                    printPage(leastFrequentlyUsedPage);
                    page auxiliarySwapPage = virtualMemory[j];

                    virtualMemory[j].positionInRealMemory = leastFrequentlyUsedPage.positionInRealMemory;
                    virtualMemory[j].positionInSwapMemory = -1;

                    virtualMemory[leastFrequentlyUsedPage.id].positionInRealMemory = -1;
                    virtualMemory[leastFrequentlyUsedPage.id].positionInSwapMemory = auxiliarySwapPage.positionInSwapMemory;
                    
                    printf("Page %d swapped places with page %d, who was the LFU page.\n", j, leastFrequentlyUsedPage.id);
                }
            }
        }
        printf("Memoria Virtual apos todos os swaps do clock:\n");
        printMemory(virtualMemory, vmSize);
    }
}

page findLeastFrequentlyUsedPage(page *virtualMemory, int vmSize, int rmSize) {
    page leastFrequentlyUsedPage = initializePage(vmSize, rmSize, vmSize-rmSize);
    leastFrequentlyUsedPage.relevancy = UINT8_MAX;
    for (int i=0; i < vmSize; i++) {
        if (virtualMemory[i].positionInRealMemory >= 0 && virtualMemory[i].relevancy < leastFrequentlyUsedPage.relevancy) {
            leastFrequentlyUsedPage = virtualMemory[i];
        }
    }
    return leastFrequentlyUsedPage;
}

void printMemory(page *memory, int size) {
    printf("================================================================================================\n");
    for (int i=0; i<size; i++) {
        printPage(memory[i]);
    }
    printf("================================================================================================\n");
}

void printPage(page page) {
    printf("| ID: %-5d | Position in Real Memory: %-5d | Position in Swap Memory: %-5d | Relevancy: %-3d |\n", page.id, page.positionInRealMemory, page.positionInSwapMemory, page.relevancy);
}
