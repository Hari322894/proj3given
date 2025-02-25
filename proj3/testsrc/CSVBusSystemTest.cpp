#include "CSVBusSystem.h"
#include "DSVReader.h"
#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>

class MockDSVReader : public CDSVReader {
public:
    std::vector<std::vector<std::string>> Data;
    size_t CurrentRow;

    MockDSVReader() : CurrentRow(0) {}

    bool ReadRow(std::vector<std::string> &row) override {
        if (CurrentRow < Data.size()) {
            row = Data[CurrentRow];
            CurrentRow++;
            return true;
        }
        return false;
    }
};

class CSVBusSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code for CSVBusSystemTest
    }

    void TearDown() override {
        // Cleanup code for CSVBusSystemTest
    }
};

TEST_F(CSVBusSystemTest, TestStopCount) {
    auto stopReader = std::make_shared<MockDSVReader>();
    stopReader->Data = { {"1", "100"}, {"2", "200"}, {"3", "300"} };

    auto routeReader = std::make_shared<MockDSVReader>();
    routeReader->Data = { {"Route1", "1"}, {"Route1", "2"}, {"Route2", "3"} };

    CCSVBusSystem busSystem(stopReader, routeReader);
    EXPECT_EQ(busSystem.StopCount(), 3);
}

TEST_F(CSVBusSystemTest, TestRouteCount) {
    auto stopReader = std::make_shared<MockDSVReader>();
    stopReader->Data = { {"1", "100"}, {"2", "200"}, {"3", "300"} };

    auto routeReader = std::make_shared<MockDSVReader>();
    routeReader->Data = { {"Route1", "1"}, {"Route1", "2"}, {"Route2", "3"} };

    CCSVBusSystem busSystem(stopReader, routeReader);
    EXPECT_EQ(busSystem.RouteCount(), 2);
}