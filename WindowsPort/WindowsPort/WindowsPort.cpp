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

#include <chrono>

#include <windows.h>
const int DEFAULT_N = 50;
const int DEFAULT_THREADS = 1;

CRITICAL_SECTION globCS;

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
    double maxMultiplePivot = floor(sqrt(n));
    std::vector<int> result;
    std::vector<int> tmp(n - 1);
    std::iota(std::begin(tmp), std::end(tmp), 2);
    std::vector<int> nps;

    for (int i = 2; i < maxMultiplePivot; i++) {
        auto npMultiples = findNonPrimeMultiples(n, i);
        for (auto i : npMultiples) {
            nps.push_back(i);
        }
    }

    std::sort(nps.begin(), nps.end());
    std::set_difference(tmp.begin(), tmp.end(), nps.begin(), nps.end(), std::inserter(result, result.begin()));

    return result;
}

std::vector<int> erastothenesParallel(int n) {

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
    std::vector<std::thread> searcherThreads;

    std::vector<int> result;
    std::vector<int> nps;

    for (size_t i = 2; i < maxMultiplePivot; i++) {
        int x = (int)i;
        int mptTmp = multiplesPerThread;
        if (i + mptTmp > maxMultiplePivot) {
            mptTmp = (int)i + mptTmp - maxMultiplePivot;
        }
        
        searcherThreads.emplace_back(std::thread([&nps, n, x, mptTmp] {
            for (size_t mpt = 0; mpt < mptTmp; mpt++) {
                auto npMultiples = findNonPrimeMultiples(n, x);
                for (auto np : npMultiples) {
                    EnterCriticalSection(&globCS);
                    nps.push_back(np);
                    LeaveCriticalSection(&globCS);
                }
            }
        }));


        if (multiplesPerThread > 1) {
            i += multiplesPerThread - 1;
        }
    }
    
    for (auto &t : searcherThreads) {
        t.join();
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
    int parallel = 0;

    if (argc != 3) {
        printf("Usage: lab3 <n> <1 for parallel, 2 for serial>\n");
        return 1;
    }
    else {
        n = std::stoi(argv[1]);
        int is_parallel = std::stoi(argv[2]);//std::stoi(argv[2]) == 1;
        if (is_parallel == 1)
            parallel = 1;
        else
            parallel = 0;
    }


    std::vector<int> erast;
    if (parallel) {
        printf("\n======Running in Parallel!======\n");
        printf("Computing for %d\n", n);
        if (!InitializeCriticalSectionAndSpinCount(&globCS, 0x00000400))
            return 1;
        erast = erastothenesParallel(n);
    }
    else {
        printf("\n======Running in Series!======\n");
        printf("Computing for %d\n", n);
        erast = erastothenesSerial(n);
    }

    /*for (auto& i : erast) {
        printf("%d, ", i);
    }*/

    return 0;
}