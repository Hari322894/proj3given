#include "OpenStreetMap.h"
#include "XMLReader.h"
#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>

class OpenStreetMapTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code for OpenStreetMapTest
    }

    void TearDown() override {
        // Cleanup code for OpenStreetMapTest
    }
};

TEST_F(OpenStreetMapTest, TestNodeCount) {
    auto xmlReader = std::make_shared<CXMLReader>("map.osm");
    COpenStreetMap osmMap(xmlReader);
    EXPECT_EQ(osmMap.NodeCount(), 10); // Example expected count
}

TEST_F(OpenStreetMapTest, TestWayCount) {
    auto xmlReader = std::make_shared<CXMLReader>("map.osm");
    COpenStreetMap osmMap(xmlReader);
    EXPECT_EQ(osmMap.WayCount(), 5); // Example expected count
}