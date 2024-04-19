#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <mpi.h>

bool isPalindrome(int number) {
    std::string str = std::to_string(number);
    std::reverse(str.begin(), str.end());
    return std::to_string(number) == str;
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    double startTime = MPI_Wtime();

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int ROOT = 0;
    std::vector<int> numbers;

    if (rank == ROOT) {
        std::ifstream inputFile("random_numbers.txt");
        if (!inputFile) {
            std::cerr << "Error: Could not open input file." << std::endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        int number;
        while (inputFile >> number) {
            numbers.push_back(number);
        }

        inputFile.close();
    }

    int numNumbers = numbers.size();
    MPI_Bcast(&numNumbers, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

    int localSize = numNumbers / size;
    int remainder = numNumbers % size;

    std::vector<int> localBuffer(localSize);
    if (rank == size - 1) {
        localBuffer.resize(localSize + remainder);
    }

    MPI_Scatter(numbers.data(), localBuffer.size(), MPI_INT, localBuffer.data(), localBuffer.size(), MPI_INT, ROOT, MPI_COMM_WORLD);

    double threadStartTime = MPI_Wtime(); // Start time for each thread

    std::vector<int> localPalindromes;
    for (int num : localBuffer) {
        if (isPalindrome(num)) {
            localPalindromes.push_back(num);
        }
    }
    int localPalindromesSize = localPalindromes.size();

    double threadEndTime = MPI_Wtime();
    double threadExecutionTime = threadEndTime - threadStartTime;

    std::cout << "Thread " << rank << ": " << threadExecutionTime << " seconds" << std::endl;

    std::vector<int> allPalindromes;
    std::vector<int> recvCounts(size);
    std::vector<int> displacements(size);

    MPI_Gather(&localPalindromesSize, 1, MPI_INT, recvCounts.data(), 1, MPI_INT, ROOT, MPI_COMM_WORLD);

    if (rank == ROOT) {
        int totalPalindromes = 0;
        for (int i = 0; i < size; ++i) {
            displacements[i] = totalPalindromes;
            totalPalindromes += recvCounts[i];
        }
        allPalindromes.resize(totalPalindromes);
    }

    MPI_Gatherv(localPalindromes.data(), localPalindromes.size(), MPI_INT,
                allPalindromes.data(), recvCounts.data(), displacements.data(), MPI_INT, ROOT, MPI_COMM_WORLD);

    if (rank == ROOT) {
        std::ofstream outputFile("output.txt");
        if (!outputFile) {
            std::cerr << "Error: Could not create output file." << std::endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        for (int palindrome : allPalindromes) {
            outputFile << palindrome << std::endl;
        }
        outputFile.close();

        double endTime = MPI_Wtime();
        std::cout << "Total execution time: " << endTime - startTime << " seconds" << std::endl;
    }

    MPI_Finalize();

    return 0;
}
