/*
    Erastothenes Sieve speed up experiment -- Windows Version

    Trying to apply "divide and conquer" method, where a large task is split into smaller tasks.

    Author: Lukas Michnevic
    2023
*/

#include "pch.h"
#include <stdio.h>
#include <string>
#include <thread>
#include <vector>
#include <tuple>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <iterator>

#include <windows.h>
const int DEFAULT_N = 50;
const int DEFAULT_THREADS = 1;

HANDLE globMutex;

// Packed struct for somewhat simpler passing to a thread
struct threadConfig {
    std::vector<int>* n_nps;
    int n;
    int x;
    int mptTmp;
};

std::vector<int> findNonPrimeMultiples(int n, int x) {
    std::vector<int> nonPrimeMultiples;
    for (size_t i = 0; i < n; i++) {
        if (i == x || i == 0)
            continue;

        if (i % x == 0)
            nonPrimeMultiples.push_back((int)i);
    }

    return nonPrimeMultiples;
}

std::vector<int> erastothenesSerial(int n) {
    int maxMultiplePivot = (int)floor(sqrt(n));
    std::vector<int> result;
    std::vector<int> tmp(n - 1);
    std::iota(std::begin(tmp), std::end(tmp), 2);
    std::vector<int> nps;

    for (size_t i = 2; i < maxMultiplePivot; i++) {
        auto npMultiples = findNonPrimeMultiples(n, (int)i);
        for (auto i : npMultiples) {
            nps.push_back(i);
        }
    }

    std::sort(nps.begin(), nps.end());
    std::set_difference(tmp.begin(), tmp.end(), nps.begin(), nps.end(), std::inserter(result, result.begin()));

    return result;
}

std::vector<int> erastothenesParallel(int n) {

    globMutex = CreateMutex(NULL, FALSE, NULL);

    int maxMultiplePivot = (int)floor(sqrt(n));

    int multiplesPerThread = 1;

    
    if (maxMultiplePivot >= 4000) {
        // Program may crash if too many thread used. M2 Pro crashes somewhere around 4500
        // Make one thread handle two searches for multiples in this case to reduce the amount of threads.
        multiplesPerThread = 4;
    }

    if (maxMultiplePivot > 16000) {
        multiplesPerThread = 8;
    }

    if (maxMultiplePivot > 32000) {
        multiplesPerThread = 16;
    }

    printf("Max Multiple Pivot is: %d\n", maxMultiplePivot);
    printf("Multiples per thread: %d\n", multiplesPerThread);
    std::vector<HANDLE> searcherThreads;

    std::vector<int> result;
    std::vector<int> nps;

    for (size_t i = 2; i < maxMultiplePivot; i++) {
        int x = (int)i;
        int mptTmp = multiplesPerThread;
        if (i + mptTmp > maxMultiplePivot) {
            mptTmp = (int)i + mptTmp - maxMultiplePivot;
        }
        DWORD threadID;
        
        /*searcherThreads.emplace_back(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) [&nps, n, x, mptTmp] {
            }), NULL, 0, &threadID);

        searcherThreads.emplace_back(std::thread([&nps, n, x, mptTmp] {
            for (size_t mpt = 0; mpt < mptTmp; mpt++) {
                auto npMultiples = findNonPrimeMultiples(n, x);
                for (auto np : npMultiples) {
                    WaitForSingleObject(globMutex, INFINITE);
                    nps.push_back(np);
                    ReleaseMutex(globMutex);
                }
            }
            }));*/

        auto work = [](LPVOID data) -> DWORD {
            threadConfig* tConfig = (threadConfig*)data;
            for (size_t mpt = 0; mpt < tConfig->mptTmp; mpt++) {
                auto npMultiples = findNonPrimeMultiples(tConfig->n, tConfig->x);
                for (auto np : npMultiples) {
                    WaitForSingleObject(globMutex, INFINITE);
                    tConfig->n_nps->push_back(np);
                    ReleaseMutex(globMutex);
                }
            }

            free(tConfig);
            return 0;
        };
        threadConfig* thread_param = (threadConfig*)malloc(sizeof(threadConfig));//{&nps, n, x, mptTmp};
        if (thread_param == NULL) {
            printf("memory alloc went kaboom!");
            exit(1);
        }
        thread_param->n_nps = &nps;
        thread_param->n = n;
        thread_param->x = x;
        thread_param->mptTmp = mptTmp;

        auto thread = CreateThread(nullptr, 0, work, thread_param, 0, &threadID);
        searcherThreads.emplace_back(thread);

        if (multiplesPerThread > 1) {
            i += multiplesPerThread - 1;
        }
    }
    
    for (auto& t : searcherThreads) {
        WaitForSingleObject(t, INFINITE);
    }

    std::vector<int> tmp(n - 1);
    std::iota(std::begin(tmp), std::end(tmp), 2);
    std::sort(nps.begin(), nps.end());
    std::set_difference(tmp.begin(), tmp.end(), nps.begin(), nps.end(), std::inserter(result, result.begin()));

    //CloseHandle(globMutex);

    return result;
}

int main(int argc, char** argv) {
    
    int n = DEFAULT_N;
    //bool parallel = false;

    int parallel = 0;

    if (argc != 3) {
        printf("Usage: lab3 <n> <1 for parallel, 2 for serial>\n");
        return 1;
    }
    else {
        n = std::stoi(argv[1]);
        int is_parallel = std::stoi(argv[2]);//std::stoi(argv[2]) == 1;
        printf("is parallel %d\n", is_parallel);
        if (is_parallel == 1)
            parallel = 1;
        else
            parallel = 0;
    }

    std::vector<int> erast;
    if (parallel) {
        printf("\n======Running in Parallel!======\n");
        erast = erastothenesParallel(n);
    }
    else {
        printf("\n======Running in Series!======\n");
        erast = erastothenesSerial(n);
    }

    /*for (auto& i : erast) {
        printf("%d, ", i);
    }*/

    return 0;
}