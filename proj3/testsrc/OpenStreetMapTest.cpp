#include "OpenStreetMap.h"
#include "XMLReader.h"
#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>

class MockXMLReader : public CXMLReader {
public:
    std::vector<std::string> Data;
    size_t CurrentIndex;

    // Fix: Call CXMLReader's constructor explicitly
    MockXMLReader() : CXMLReader(nullptr), CurrentIndex(0) {}

    // Fix: Ensure Read() method properly overrides the base class method
    bool Read() override {
        if (CurrentIndex < Data.size()) {
            CurrentIndex++;
            return true;
        }
        return false;
    }

    // Fix: Ensure Name() properly overrides the base class method
    std::string Name() const override {
        if (CurrentIndex == 0 || CurrentIndex > Data.size()) {
            return "";
        }
        return Data[CurrentIndex - 1];
    }
};

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
    auto xmlReader = std::make_shared<MockXMLReader>();
    xmlReader->Data = { "node", "node", "node" };

    COpenStreetMap osmMap(xmlReader);
    EXPECT_EQ(osmMap.NodeCount(), 3);
}

TEST_F(OpenStreetMapTest, TestWayCount) {
    auto xmlReader = std::make_shared<MockXMLReader>();
    xmlReader->Data = { "way", "way" };

    COpenStreetMap osmMap(xmlReader);
    EXPECT_EQ(osmMap.WayCount(), 2);
}
