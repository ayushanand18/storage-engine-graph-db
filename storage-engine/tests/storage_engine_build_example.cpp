#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>
#include "storage_engine.h" // Include your storage engine header

using namespace storage_engine;

std::string generate_random_string(size_t length) {
    const char characters[] = "abcdefghijklmnopqrstuvwxyz";
    std::string result;
    std::uniform_int_distribution<int> dist(0, sizeof(characters) - 2);
    std::default_random_engine generator;

    for (size_t i = 0; i < length; ++i) {
        result += characters[dist(generator)];
    }
    return result;
}

int main() {
    StorageEngine engine;

    const int num_nodes = 10000;
    const int num_connections = 10000;

    std::vector<std::chrono::duration<double>> node_creation_times;
    std::vector<std::chrono::duration<double>> connection_creation_times;

    // Generate and insert nodes
    for (int i = 0; i < num_nodes; ++i) {
        std::vector<unsigned char> node_data(generate_random_string(10).begin(), generate_random_string(10).end());

        auto start = std::chrono::high_resolution_clock::now();
        engine.create_node(node_data);
        auto end = std::chrono::high_resolution_clock::now();

        node_creation_times.push_back(end - start);
    }

    // Generate and insert connections
    for (int i = 0; i < num_connections; ++i) {
        std::string from_node_id = generate_random_string(10);
        std::string to_node_id = generate_random_string(10);

        auto start = std::chrono::high_resolution_clock::now();
        engine.add_connection(from_node_id, to_node_id);
        auto end = std::chrono::high_resolution_clock::now();

        connection_creation_times.push_back(end - start);
    }

    // Calculate percentiles
    auto calculate_percentiles = [](std::vector<std::chrono::duration<double>>& times) {
        std::sort(times.begin(), times.end());
        double p50 = times[times.size() * 0.50].count();
        double p90 = times[times.size() * 0.90].count();
        double p99 = times[times.size() * 0.99].count();
        return std::make_tuple(p50, p90, p99);
    };

    auto [node_p50, node_p90, node_p99] = calculate_percentiles(node_creation_times);
    auto [conn_p50, conn_p90, conn_p99] = calculate_percentiles(connection_creation_times);

    // Output results
    std::cout << "Node Creation Times (seconds):" << std::endl;
    std::cout << "P50: " << node_p50 << ", P90: " << node_p90 << ", P99: " << node_p99 << std::endl;

    std::cout << "Connection Creation Times (seconds):" << std::endl;
    std::cout << "P50: " << conn_p50 << ", P90: " << conn_p90 << ", P99: " << conn_p99 << std::endl;

    return 0;
}