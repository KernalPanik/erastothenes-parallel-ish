# Description of a problem

The problem to be solved is an old algorithm for finding all primes before number N -- an Erastothenes Sieve. This approach relies of filtering out definite non-prime numbers (the ones that are divisible by anything other than 1 or themselves.). 

So, imagine a number 10 as an N. Erastothenes Sieve suggests that you go through each number before 10 and try dividing it by every other number in the same range. If it's divisible by something else than 1 on that number itself, then it's definitely not a prime and should be removed from the result list. 

# How was algorithm optimized and parallelized

First optimization was to check divisibility against 2 -- floor(sqrt(N)). This is because each further value to be checked is a derivative of those first numbers. Again if N == 10, in the base case we would have to divide by numbers like 4, 5, 6, 7, 8, 9, 10. But many of those would've been already crossed out by their square roots, like 2 and 3, leaving us with 5 & 7 -- primes.

## Parallelization

Parallelization was done by running multiplier (factor) search for each separate number (2; sqrt(N)) in a separate thread. Those multipliers were later crossed with the Initial Result Vector, containing <2;N>. Single Mutex was enough to synchronize access to the final non-prime number vector.

NOTE: with large numbers (half of the Int32.Max value) some systems crashed due to over-allocation of threads. The issue was mitigated by adding more numbers to search per thread.

# Experiment

Experiment was conducted on M2 Pro chip with 6 efficient cores and 4 non-efficient cores. The Laptop was connected to the power, allowing for maximum power output. 

The other machine tried was a computer with Ryzen 7 3700x (8 Cores)

## Data set

Numbers used were powers of 2, up until 2^28. On larger numbers, the runtime was expected to be in Hours rather than Minutes. 28 data points were considered dense enough to see the trend.