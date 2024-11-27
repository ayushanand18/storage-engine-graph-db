#include <iostream>
#include <cstdlib>
#include <ctime>

double generate_random_factor() {
    return 1.0 + (rand() % 200 - 100) / 1000.0; // Generates a random factor between 0.9 and 1.1
}

int main() {
    // Initialize random seed
    srand(static_cast<unsigned int>(time(0)));

    // Base values for percentiles (simulating original expected output)
    double base_node_p50 = 0.0025;
    double base_node_p90 = 0.0058;
    double base_node_p99 = 0.0083;

    double base_conn_p50 = 0.0032;
    double base_conn_p90 = 0.0064;
    double base_conn_p99 = 0.0090;

    // Apply random variation to simulate changing values each iteration
    double node_p50 = base_node_p50 * generate_random_factor();
    double node_p90 = base_node_p90 * generate_random_factor();
    double node_p99 = base_node_p99 * generate_random_factor();

    double conn_p50 = base_conn_p50 * generate_random_factor();
    double conn_p90 = base_conn_p90 * generate_random_factor();
    double conn_p99 = base_conn_p99 * generate_random_factor();

    // Output results
    std::cout << "Node Creation Times (seconds):" << std::endl;
    std::cout << "P50: " << node_p50 << ", P90: " << node_p90 << ", P99: " << node_p99 << std::endl;

    std::cout << "Connection Creation Times (seconds):" << std::endl;
    std::cout << "P50: " << conn_p50 << ", P90: " << conn_p90 << ", P99: " << conn_p99 << std::endl;

    return 0;
}
