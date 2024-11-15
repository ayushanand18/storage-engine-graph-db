#include <gtest/gtest.h>
#include "storage_engine.h"

using namespace storage_engine;

class StorageEngineTest : public ::testing::Test {
protected:
    StorageEngine engine;

    void SetUp() override {
        // Any setup needed before each test
    }

    void TearDown() override {
        // Clean up after each test
    }
};

// Test node creation
TEST_F(StorageEngineTest, CreateNode) {
    std::vector<unsigned char> node_data = {'d', 'a', 't', 'a'};
    std::string node_id = engine.create_node(node_data);
    
    ASSERT_FALSE(node_id.empty());
    ASSERT_TRUE(engine.node_id_index_->exists(node_id)); // Assuming node_id_index_ is accessible for testing
}

// Test connection addition
TEST_F(StorageEngineTest, AddConnection) {
    std::vector<unsigned char> node_data1 = {'n', 'o', 'd', 'e', '1'};
    std::string node1_id = engine.create_node(node_data1);
    
    std::vector<unsigned char> node_data2 = {'n', 'o', 'd', 'e', '2'};
    std::string node2_id = engine.create_node(node_data2);
    
    engine.add_connection(node1_id, node2_id);
    
    // Check if the connection exists
    auto connections = engine.match_connections(node1_id, "");
    ASSERT_NE(std::find(connections.begin(), connections.end(), node2_id), connections.end());
}

// Test connection deletion
TEST_F(StorageEngineTest, DeleteConnection) {
    std::vector<unsigned char> node_data1 = {'n', 'o', 'd', 'e', '1'};
    std::string node1_id = engine.create_node(node_data1);
    
    std::vector<unsigned char> node_data2 = {'n', 'o', 'd', 'e', '2'};
    std::string node2_id = engine.create_node(node_data2);
    
    engine.add_connection(node1_id, node2_id);
    engine.delete_connection(node1_id, node2_id);
    
    // Check if the connection no longer exists
    auto connections = engine.match_connections(node1_id, "");
    ASSERT_EQ(std::find(connections.begin(), connections.end(), node2_id), connections.end());
}

// Test error when adding connection with non-existent nodes
TEST_F(StorageEngineTest, AddConnectionWithNonExistentNodes) {
    EXPECT_THROW(engine.add_connection("non_existent_1", "non_existent_2"), std::invalid_argument);
}

// Test deleting a non-existent node
TEST_F(StorageEngineTest, DeleteNonExistentNode) {
    EXPECT_THROW(engine.delete_node("non_existent_node"), std::invalid_argument);
}

// Test sanitization of prefix for node IDs
TEST_F(StorageEngineTest, SanitizePrefix) {
    std::string valid_prefix = "valid";
    EXPECT_NO_THROW(engine._sanitize_prefix_for_node_id(valid_prefix));
    
    std::string invalid_prefix = "invalid123";
    EXPECT_THROW(engine._sanitize_prefix_for_node_id(invalid_prefix), std::invalid_argument);
    
    invalid_prefix = "invalid@prefix";
    EXPECT_THROW(engine._sanitize_prefix_for_node_id(invalid_prefix), std::invalid_argument);
}

// Test active state of the storage engine
TEST_F(StorageEngineTest, IsActive) {
    ASSERT_TRUE(engine.isActive());
}

// Test getting active memtable size
TEST_F(StorageEngineTest, GetActiveMemtableSize) {
    size_t size = engine.getActiveMemtableSize();
    ASSERT_GE(size, 0); // Size should never be negative
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}