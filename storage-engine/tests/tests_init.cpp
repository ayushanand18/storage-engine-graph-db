#include <gtest/gtest.h>

#include "storage_engine.h"

class StorageEngineInitTest : public testing::Test {
protected:
    void SetUp() override {
        storage_engine_ = new storage_engine::StorageEngine();
    }

    void TearDown() override {
        delete storage_engine_;
    }
};

TEST_F(StorageEngineInitTest, TestMethod1) {
    int input_value;
    int result = storage_engine_->SomeMethod(input_value);

    EXPECT_EQ(result, 42);
}


int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
