#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief vector definition
 */
typedef struct {
    int *array;
    int size;
    int count;
} vector;

/**
 * @brief boolean definition
 */
typedef enum {
    false,
    true
} bool;

/**
 * @brief First in first out replacement policy
 * @return int number of page faults
 */
int firstInFirstOut(vector *pages, vector *frames);

/**
 * @brief Least recently used replacement policy
 * @return int number of page faults
 */
int leastRecentlyUsed(vector *pages, vector *frames);

/**
 * @brief optimal replacement policy
 * @return int number of page faults
 */
int optimal(vector *pages, vector *frames);

/**
 * @brief clock replacement policy
 * @return int number of page faults
 */
int clock(vector *pages, vector *frames);

/**
 * @brief prints the required header
 * @param policy name
 */
void printHeader(char *policy);

/**
 * @brief prints each line
 * @param fault boolean indicating if the requested page was fault or not
 * @param pageNumber required page number
 */
void printBody(vector *frames, bool fault, int pageNumber);

/**
 * @brief prints the required footer
 * @param numberOfPageFaults
 */
void printFooter(int numberOfPageFaults);

/**
 * @brief call the appropriate function according to the policy name
 * @param policy name
 * @param pages vector
 * @param frames vector
 * @return int the number of page faults
 */
int pickPolicy(char *policy, vector *pages, vector *frames);

/**
 * @brief Check if the page already exists in a frame (linear search)
 * @param frames vector
 * @return bool
 */
bool isPageInAFrame(vector *frames, int currentPage);

/**
 * @brief Get the Farthest Reference After a specific index
 * @param pages vector
 * @param frames vector
 * @return int the index of the farthest page
 */
int getFarthestReferenceAfter(vector *pages, vector *frames, int currentIndex);

/**
 * @brief Get the Farthest Reference Before a specific index
 * @param pages vector
 * @param frames vector
 * @return int the index of the farthest page
 */
int getFarthestReferenceBefore(vector *pages, vector *frames, int currentIndex);

/**
 * @brief Set the Use Bit
 * @param useBit array
 * @param frames vector
 */
void setUseBit(bool *useBit, vector *frames, int currentPage);

/**
 * @brief constuct the vector
 * @param vector pointer
 */
void vector_construct(vector *vector);

/**
 * @brief pushes element to the back of the vector
 * @param vector pointer
 * @param value The value to be added in the back
 */
void vector_push_back(vector *vector, int value);

/**
 * @brief It inserts a new element at a specific index
 * @param vector pointer
 * @param value to be added at the index
 */
void vector_insert(vector *vector, int index, int value);

/**
 * @brief It is used to get an element at specific index
 * @param vector pointer
 * @return int value at the index
 */
int vector_get(vector *vector, int index);

/**
 * @brief allocate the vector capacity be at least enough to contain size elements.
 * @param vector pointer
 */
void vector_reserve(vector *vector, int size);

int main() {
    int numberOfFrames;
    char *policy;
    vector pages, frames;
    int currentPage;

    scanf("%d", &numberOfFrames);

    fgetc(stdin);

    vector_construct(&pages);   // Create pages vector
    vector_construct(&frames);  // Create frames vector
    vector_reserve(&frames, numberOfFrames);

    policy = (char *)malloc(sizeof(char) * 16);
    fgets(policy, sizeof(policy) * 16, stdin);  // Get policy name

    int lengthOfPolicyName = strlen(policy);
    if (policy[lengthOfPolicyName - 1] == '\n')
        policy[lengthOfPolicyName - 1] = '\0';  // Remove new line

    while (true) {  // Scan the page requests until -1
        scanf("%d", &currentPage);
        if (currentPage == -1)
            break;
        else
            vector_push_back(&pages, currentPage);  // Add page requests to the vector
    }

    printHeader(policy);

    int numberOfPageFaults = pickPolicy(policy, &pages, &frames);

    printFooter(numberOfPageFaults);

    free(pages.array);
    free(frames.array);
    free(policy);

    return 0;
}

int pickPolicy(char *policy, vector *pages, vector *frames) {
    int numberOfPageFaults = 0;

    if (strcasecmp(policy, "FIFO") == 0)
        numberOfPageFaults = firstInFirstOut(pages, frames);
    else if (strcasecmp(policy, "LRU") == 0)
        numberOfPageFaults = leastRecentlyUsed(pages, frames);
    else if (strcasecmp(policy, "OPTIMAL") == 0)
        numberOfPageFaults = optimal(pages, frames);
    else if (strcasecmp(policy, "CLOCK") == 0)
        numberOfPageFaults = clock(pages, frames);
    else
        exit(-1);

    return numberOfPageFaults;
}

int firstInFirstOut(vector *pages, vector *frames) {
    int oldestPageIndex = 0, numberOfPageFaults = 0;
    int i = 0;
    int numberOfPages = pages->count;
    for (i = 0; i < numberOfPages; i++) {
        int currentPage = pages->array[i];

        if (isPageInAFrame(frames, currentPage)) {  // If page already exists in a frames
            printBody(frames, false, currentPage);
        } else if (frames->count < frames->size) {  // Page isn't in a frame, but the frames are not full
            vector_push_back(frames, currentPage);
            printBody(frames, false, currentPage);
        } else {                                                  // Page isn't in a frame and the frames are full
            vector_insert(frames, oldestPageIndex, currentPage);  // Insert the new request in place of the oldest page
            oldestPageIndex++;                                    // Update the oldest page index
            numberOfPageFaults++;
            if (oldestPageIndex == frames->size)
                oldestPageIndex = 0;
            printBody(frames, true, currentPage);
        }
    }

    return numberOfPageFaults;
}

int leastRecentlyUsed(vector *pages, vector *frames) {
    int numberOfPageFaults = 0;
    int numberOfPages = pages->count;
    int i = 0;

    for (i = 0; i < numberOfPages; i++) {
        int currentPage = pages->array[i];

        if (isPageInAFrame(frames, currentPage)) {  // If page already exists in a frames
            printBody(frames, false, currentPage);
        } else if (frames->count < frames->size) {  // Page isn't in a frame, but the frames are not full
            vector_push_back(frames, currentPage);
            printBody(frames, false, currentPage);
        } else {  // Page isn't in a frame and the frames are full
            int resultIndex = getFarthestReferenceBefore(pages, frames, i);
            vector_insert(frames, resultIndex, currentPage);
            numberOfPageFaults++;
            printBody(frames, true, currentPage);
        }
    }
    return numberOfPageFaults;
}

int optimal(vector *pages, vector *frames) {
    int numberOfPageFaults = 0;
    int i;
    int numberOfPages = pages->count;
    for (i = 0; i < numberOfPages; i++) {
        int currentPage = pages->array[i];

        if (isPageInAFrame(frames, currentPage)) {  // If page already exists in a frames
            printBody(frames, false, currentPage);
        } else if (frames->count < frames->size) {  // Page isn't in a frame, but the frames are not full
            vector_push_back(frames, currentPage);
            printBody(frames, false, currentPage);
        } else {  // Page isn't in a frame and the frames are full
            int resultIndex = getFarthestReferenceAfter(pages, frames, i);
            vector_insert(frames, resultIndex, currentPage);
            numberOfPageFaults++;
            printBody(frames, true, currentPage);
        }
    }

    return numberOfPageFaults;
}

int clock(vector *pages, vector *frames) {
    int numberOfPageFaults = 0;
    int i, index = 0;
    bool *useBits = (bool *)malloc(sizeof(bool) * frames->size);
    memset(useBits, false, sizeof(bool) * frames->size);

    int numberOfPages = pages->count;

    for (i = 0; i < numberOfPages; i++) {
        int currentPage = pages->array[i];

        if (isPageInAFrame(frames, currentPage)) {    // If page already exists in a frames
            setUseBit(useBits, frames, currentPage);  // Set the use bit to true
            printBody(frames, false, currentPage);
        } else if (frames->count < frames->size) {  // Page isn't in a frame, but the frames are not full
            vector_push_back(frames, currentPage);
            setUseBit(useBits, frames, currentPage);  // Set the use bit to true
            printBody(frames, false, currentPage);
        } else {                               // Page isn't in a frame and the frames are full
            while (useBits[index] != false) {  // Loop until false
                useBits[index] = false;        // Set every true to false
                index++;
                if (index == frames->size)
                    index = 0;
            }
            vector_insert(frames, index, currentPage);  // Insert in the first false location
            setUseBit(useBits, frames, currentPage);    // Set this location's use bit to true
            index++;
            if (index == frames->size)
                index = 0;
            numberOfPageFaults++;
            printBody(frames, true, currentPage);
        }
    }

    free(useBits);
    return numberOfPageFaults;
}

int getFarthestReferenceAfter(vector *pages, vector *frames, int currentIndex) {
    int resultIndex, farthest = currentIndex + 1;
    int numberOfPages = pages->count;
    int i, j;
    for (i = 0; i < frames->count; i++) {  // Loop over the pages in the frames
        int currentFrame = vector_get(frames, i);
        for (j = currentIndex + 1; j < numberOfPages; j++) {  // Loop to right of the current page
            int currentPage = vector_get(pages, j);
            if (currentPage == currentFrame) {  // A future reference exists
                if (j > farthest) {             // update the index of the page that has the farthest reference
                    farthest = j;
                    resultIndex = i;
                }
                break;
            }
        }
        if (j == numberOfPages)  // If a page is never referenced in future return its index
            return i;
    }

    return resultIndex;
}

int getFarthestReferenceBefore(vector *pages, vector *frames, int currentIndex) {
    int resultIndex, farthest = currentIndex - 1;
    int i, j;
    for (i = 0; i < frames->count; i++) {  // Loop over the pages in the frames
        int currentFrame = vector_get(frames, i);
        for (j = currentIndex - 1; j > 0; j--) {  // Loop to left of the current page
            int currentPage = vector_get(pages, j);
            if (currentPage == currentFrame) {  // A past reference exists
                if (j < farthest) {             // update the index of the page that has the farthest reference
                    farthest = j;
                    resultIndex = i;
                }
                break;
            }
        }
        if (j == 0)  // If a page is never referenced in the past return its index
            return i;
    }

    return resultIndex;
}

void setUseBit(bool *useBit, vector *frames, int currentPage) {
    int i = 0;
    int numberOfFrames = frames->count;
    for (i = 0; i < numberOfFrames; i++) {  // Set a use bit to true
        if (currentPage == frames->array[i]) {
            useBit[i] = true;
            return;
        }
    }
    return;
}

bool isPageInAFrame(vector *frames, int currentPage) {
    int i = 0;
    int numberOfFrames = frames->count;
    for (i = 0; i < numberOfFrames; i++) {  // Linear search to check if the page exists in a frame
        if (currentPage == frames->array[i])
            return true;
    }
    return false;
}

void printHeader(char *policy) {
    printf("Replacement Policy = %s\n", policy);
    printf("-------------------------------------\n");
    printf("Page   Content of Frames\n");
    printf("----   -----------------\n");
}

void printFooter(int numberOfPageFaults) {
    printf("-------------------------------------\n");
    printf("Number of page faults = %d\n", numberOfPageFaults);
}

void printBody(vector *frames, bool fault, int pageNumber) {
    printf("%02d ", pageNumber);

    fault == true ? printf("F   ") : printf("    ");
    int i = 0;
    int numberOfFrames = frames->count;
    for (i = 0; i < numberOfFrames; i++) {
        printf("%02d ", frames->array[i]);
    }
    printf("\n");
}

void vector_construct(vector *vector) {
    vector->array = NULL;
    vector->count = 0;
    vector->size = 0;
    return;
}

void vector_push_back(vector *vector, int value) {
    if (vector->size == 0) {
        vector->size = 10;
        vector->array = (int *)malloc(sizeof(int) * vector->size);
        memset(vector->array, '\0', sizeof(int) * vector->size);
    }

    if (vector->size == vector->count) {
        vector->size *= 2;  // If the array becomes full, multiply its size by 2 and reallocate
        vector->array = (int *)realloc(vector->array, sizeof(int) * vector->size);
    }

    vector->array[vector->count] = value;
    vector->count++;
    return;
}

void vector_insert(vector *vector, int index, int value) {
    if (index >= vector->count)
        return;

    vector->array[index] = value;  // Insert at a specific index
    return;
}

int vector_get(vector *vector, int index) {
    if (index >= vector->count)
        return -1;

    return vector->array[index];  // get element from a specific index
}

void vector_reserve(vector *vector, int size) {
    if (vector->size == 0) {
        vector->size = size;
        vector->array = (int *)malloc(sizeof(int) * vector->size);
        memset(vector->array, '\0', sizeof(int) * vector->size);
    }
    return;
}
