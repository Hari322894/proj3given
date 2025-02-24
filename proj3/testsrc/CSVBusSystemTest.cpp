#include "gtest/gtest.h"
#include "CSBusSystem.h"
#include "DSVReader.h"

// Test for CCSVBusSystem class
TEST(BusSystemTest, StopAndRouteCount) {
    auto stopReader = std::make_shared<CDSVReader>("stops.csv");
    auto routeReader = std::make_shared<CDSVReader>("routes.csv");
    CCSVBusSystem busSystem(stopReader, routeReader);

    EXPECT_EQ(busSystem.StopCount(), 25); // Adjust as per your data
    EXPECT_EQ(busSystem.RouteCount(), 1); // Adjust as per your data
}

TEST(BusSystemTest, StopByID) {
    auto stopReader = std::make_shared<CDSVReader>("stops.csv");
    auto routeReader = std::make_shared<CDSVReader>("routes.csv");
    CCSVBusSystem busSystem(stopReader, routeReader);

    auto stop = busSystem.StopByID(22043); // Adjust as per your data
    ASSERT_NE(stop, nullptr);
    EXPECT_EQ(stop->ID, 22043);
    EXPECT_EQ(stop->NodeID, 2849810514);
}

TEST(BusSystemTest, RouteByName) {
    auto stopReader = std::make_shared<CDSVReader>("stops.csv");
    auto routeReader = std::make_shared<CDSVReader>("routes.csv");
    CCSVBusSystem busSystem(stopReader, routeReader);

    auto route = busSystem.RouteByName("A"); // Adjust as per your data
    ASSERT_NE(route, nullptr);
    EXPECT_EQ(route->Name, "A");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}