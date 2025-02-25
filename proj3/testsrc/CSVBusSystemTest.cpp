#include "CSVBusSystem.h"
#include "DSVReader.h"
#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>

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
    auto stopReader = std::make_shared<CDSVReader>("stops.csv");
    auto routeReader = std::make_shared<CDSVReader>("routes.csv");
    CCSVBusSystem busSystem(stopReader, routeReader);
    EXPECT_EQ(busSystem.StopCount(), 5); // Example expected count
}

TEST_F(CSVBusSystemTest, TestRouteCount) {
    auto stopReader = std::make_shared<CDSVReader>("stops.csv");
    auto routeReader = std::make_shared<CDSVReader>("routes.csv");
    CCSVBusSystem busSystem(stopReader, routeReader);
    EXPECT_EQ(busSystem.RouteCount(), 3); // Example expected count
}