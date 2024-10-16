#include <mpi.h>
#include <stdio.h>
#include <math.h>

// Function to evaluate the curve (y = f(x))
float f(float x) {
    return x * x; // Example: y = x^2
}

// Function to compute the area of a trapezoid and count steps
float trapezoid_area(float a, float b, float d, int* step_count) {
    float area = 0;
    for (float x = a; x < b; x += d) {
        area += f(x) + f(x + d);
        *step_count += 2; // Increment step count for each function call
    }
    return area * d / 2.0f;
}

int main(int argc, char** argv) {
    int rank, size;
    float a = 0.0f, b = 1.0f;  // Limits of integration
    int n;
    float start, end, local_area, total_area;
    int local_steps = 0, total_steps = 0;

    MPI_Init(&argc, &argv); // Initialize MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Get rank of the process
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Get number of processes

    if (rank == 0) {
        // Get the number of intervals from the user
        printf("Enter the number of intervals: ");
        scanf("%d", &n);
    }


    // Broadcast the number of intervals to all processes
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    start = MPI_Wtime();

    // Calculate the interval size for each process
    float d = (b - a) / n; // delta
    float region = (b - a) / size;

    // Calculate local bounds for each process
    float start_local = a + rank * region;
    float end_local = start_local + region;



    // Each process calculates the area of its subinterval and counts steps
    local_area = trapezoid_area(start_local, end_local, d, &local_steps);

    // Reduce all local areas to the total area on the root process
    MPI_Reduce(&local_area, &total_area, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

    // Reduce all local step counts to the total step count on the root process
    MPI_Reduce(&local_steps, &total_steps, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    end = MPI_Wtime();

    // Print the result and execution time on the root process
    if (rank == 0) {
        printf("The total area under the curve is: %f\n", total_area);
        printf("Total computational steps: %d\n", total_steps);
        printf("Execution time: %f seconds\n", end - start);
    }

    MPI_Finalize(); // Finalize MPI
    return 0;
}
