#include "OpenStreetMap.h"
#include "XMLReader.h"
#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>

class MockXMLReader : public CXMLReader {
public:
    struct SXMLEntity {
        enum class EType { StartElement, EndElement };
        EType DType;
        std::string DNameData;
        std::unordered_map<std::string, std::string> DAttributes;
    };

    std::vector<SXMLEntity> Data;
    size_t CurrentIndex;

    // Fix: Call CXMLReader's constructor explicitly
    MockXMLReader() : CXMLReader(nullptr), CurrentIndex(0) {}

    bool ReadEntity(SXMLEntity &entity, bool skipcdata = false) override {
        if (CurrentIndex < Data.size()) {
            entity = Data[CurrentIndex++];
            return true;
        }
        return false;
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
    
    xmlReader->Data = {
        { MockXMLReader::SXMLEntity::EType::StartElement, "node", {{"id", "1"}} },
        { MockXMLReader::SXMLEntity::EType::StartElement, "node", {{"id", "2"}} },
        { MockXMLReader::SXMLEntity::EType::StartElement, "node", {{"id", "3"}} }
    };

    COpenStreetMap osmMap(xmlReader);
    EXPECT_EQ(osmMap.NodeCount(), 3);
}

TEST_F(OpenStreetMapTest, TestWayCount) {
    auto xmlReader = std::make_shared<MockXMLReader>();

    xmlReader->Data = {
        { MockXMLReader::SXMLEntity::EType::StartElement, "way", {{"id", "10"}} },
        { MockXMLReader::SXMLEntity::EType::StartElement, "way", {{"id", "11"}} }
    };

    COpenStreetMap osmMap(xmlReader);
    EXPECT_EQ(osmMap.WayCount(), 2);
}
