#include "gtest/gtest.h"
#include "OpenStreetMap.h"
#include "XMLReader.h"

// Test for COpenStreetMap class
TEST(StreetMapTest, NodeAndWayCount) {
    auto xmlReader = std::make_shared<CXMLReader>("davis.osm");
    COpenStreetMap streetMap(xmlReader);

    EXPECT_GT(streetMap.NodeCount(), 0);
    EXPECT_GT(streetMap.WayCount(), 0);
}

TEST(StreetMapTest, NodeByID) {
    auto xmlReader = std::make_shared<CXMLReader>("davis.osm");
    COpenStreetMap streetMap(xmlReader);

    auto node = streetMap.NodeByID(2849810514); // Adjust as per your data
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->ID, 2849810514);
}

TEST(StreetMapTest, WayByID) {
    auto xmlReader = std::make_shared<CXMLReader>("davis.osm");
    COpenStreetMap streetMap(xmlReader);

    auto way = streetMap.WayByID(123456); // Adjust as per your data
    ASSERT_NE(way, nullptr);
    EXPECT_EQ(way->ID, 123456);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}