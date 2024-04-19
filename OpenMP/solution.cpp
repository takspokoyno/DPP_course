#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <omp.h>

bool isPalindrome(int number) {
    std::string str = std::to_string(number);
    std::string reversed = str;
    std::reverse(reversed.begin(), reversed.end());
    return str == reversed;
}

int main() {
    std::ifstream inputFile("random_numbers.txt");
    std::ofstream outputFile("output.txt");

    if (!inputFile) {
        std::cerr << "Error: Could not open input file." << std::endl;
        return 1;
    }

    if (!outputFile) {
        std::cerr << "Error: Could not create output file." << std::endl;
        return 1;
    }

    std::vector<int> numbers;
    int number;
    while (inputFile >> number) {
        numbers.push_back(number);
    }

    int numThreads;
    std::cout << "Enter the number of threads: ";
    std::cin >> numThreads;

    double startTime = omp_get_wtime();
    omp_set_num_threads(numThreads);

    std::vector<int> palindromes;
    int threadNum, start, end;

    #pragma omp parallel private(threadNum, start, end)
    {
        threadNum = omp_get_thread_num();
        start = (threadNum * numbers.size()) / numThreads;
        end = ((threadNum + 1) * numbers.size()) / numThreads;

        for (int i = start; i < end; ++i) {
            if (isPalindrome(numbers[i])) {
                #pragma omp critical
                {
                    palindromes.push_back(numbers[i]);
                }
            }
        }
    }

    double endTime = omp_get_wtime();
    double executionTime = endTime - startTime;

    for (size_t i = 0; i < palindromes.size(); ++i) {
        outputFile << palindromes[i] << std::endl;
    }

    std::cout << "Palindromes found and written to output.txt." << std::endl;
    std::cout << "Execution time: " << executionTime << " seconds." << std::endl;

    return 0;
}
