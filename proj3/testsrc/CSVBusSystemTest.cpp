#include "CSVBusSystem.h"
#include "StringDataSource.h"
#include "StringDataSink.h"
#include "gtest/gtest.h"
#include <string>
#include <vector>
#include <memory>

// Mock DSVReader class
class MockDSVReader : public CDSVReader {
private:
    size_t CurrentRow;
    std::vector<std::vector<std::string>> MockData;
    
public:
    // Fix 1: Properly initialize the base class with required parameters
    MockDSVReader(std::vector<std::vector<std::string>> data) 
        : CDSVReader(std::make_shared<CStringDataSource>(""), ','), 
          CurrentRow(0),
          MockData(data) {}
    
    // Fix 2: Make sure the signature matches exactly what's in the base class
    bool ReadRow(std::vector<std::string> &row) override {
        if (CurrentRow < MockData.size()) {
            row = MockData[CurrentRow];
            CurrentRow++;
            return true;
        }
        return false;
    }
    
    // Reset the mock for reuse
    void Reset() {
        CurrentRow = 0;
    }
};

// Test fixture for CSVBusSystem
class CSVBusSystemTest : public ::testing::Test {
protected:
    std::vector<std::vector<std::string>> TestBusData = {
        {"BusID", "Name", "Type", "Latitude", "Longitude"},
        {"1", "Downtown", "Station", "34.052235", "-118.243683"},
        {"2", "Uptown", "Station", "34.055862", "-118.249559"},
        {"3", "Downtown-Uptown", "Bus", "", ""}
    };
    
    std::vector<std::vector<std::string>> TestRouteData = {
        {"RouteID", "BusID", "StopNumber"},
        {"1", "3", "1"},
        {"1", "1", "2"},
        {"1", "3", "3"},
        {"1", "2", "4"}
    };
};

// Test loading a bus system
TEST_F(CSVBusSystemTest, LoadBusSystem) {
    // Create mock readers
    auto busReader = std::make_shared<MockDSVReader>(TestBusData);
    auto routeReader = std::make_shared<MockDSVReader>(TestRouteData);
    
    // Create bus system
    CCSVBusSystem busSystem;
    
    // Load data
    EXPECT_TRUE(busSystem.Load(busReader, routeReader));
    
    // Test bus count
    EXPECT_EQ(busSystem.BusCount(), 3);
    
    // Test route count
    EXPECT_EQ(busSystem.RouteCount(), 1);
    
    // Test specific bus details
    auto bus = busSystem.FindBus("1");
    ASSERT_NE(bus, nullptr);
    EXPECT_EQ(bus->ID(), "1");
    EXPECT_EQ(bus->Name(), "Downtown");
    EXPECT_EQ(bus->Type(), "Station");
    EXPECT_DOUBLE_EQ(bus->Latitude(), 34.052235);
    EXPECT_DOUBLE_EQ(bus->Longitude(), -118.243683);
}

// Test route finding
TEST_F(CSVBusSystemTest, FindRoute) {
    // Create mock readers
    auto busReader = std::make_shared<MockDSVReader>(TestBusData);
    auto routeReader = std::make_shared<MockDSVReader>(TestRouteData);
    
    // Create bus system
    CCSVBusSystem busSystem;
    
    // Load data
    EXPECT_TRUE(busSystem.Load(busReader, routeReader));
    
    // Test finding route
    auto route = busSystem.FindRoute("1");
    ASSERT_NE(route, nullptr);
    EXPECT_EQ(route->ID(), "1");
    
    // Test stops in route
    auto stops = route->Stops();
    ASSERT_EQ(stops.size(), 4);
    EXPECT_EQ(stops[0]->ID(), "3");
    EXPECT_EQ(stops[1]->ID(), "1");
    EXPECT_EQ(stops[2]->ID(), "3");
    EXPECT_EQ(stops[3]->ID(), "2");
}