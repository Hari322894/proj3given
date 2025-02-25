#include "OpenStreetMap.h"
#include "XMLReader.h"
#include <memory>
#include <vector>
#include <string>

struct COpenStreetMap::SImplementation {
    std::vector<std::shared_ptr<SNode>> Nodes;
    std::vector<std::shared_ptr<SWay>> Ways;
};

COpenStreetMap::COpenStreetMap(std::shared_ptr<CXMLReader> src) {
    DImplementation = std::make_unique<SImplementation>();

    // Parse XML and load nodes and ways
    while (src->Read()) {
        if (src->Name() == "node") {
            auto node = std::make_shared<SNode>();
            // Parse node attributes and add to Nodes
            DImplementation->Nodes.push_back(node);
        } else if (src->Name() == "way") {
            auto way = std::make_shared<SWay>();
            // Parse way attributes and add to Ways
            DImplementation->Ways.push_back(way);
        }
    }
}

COpenStreetMap::~COpenStreetMap() = default;

std::size_t COpenStreetMap::NodeCount() const noexcept {
    return DImplementation->Nodes.size();
}

std::size_t COpenStreetMap::WayCount() const noexcept {
    return DImplementation->Ways.size();
}

std::shared_ptr<CStreetMap::SNode> COpenStreetMap::NodeByIndex(std::size_t index) const noexcept {
    if (index < DImplementation->Nodes.size()) {
        return DImplementation->Nodes[index];
    }
    return nullptr;
}

std::shared_ptr<CStreetMap::SNode> COpenStreetMap::NodeByID(TNodeID id) const noexcept {
    for (auto& node : DImplementation->Nodes) {
        if (node->ID() == id) {
            return node;
        }
    }
    return nullptr;
}

std::shared_ptr<CStreetMap::SWay> COpenStreetMap::WayByIndex(std::size_t index) const noexcept {
    if (index < DImplementation->Ways.size()) {
        return DImplementation->Ways[index];
    }
    return nullptr;
}

std::shared_ptr<CStreetMap::SWay> COpenStreetMap::WayByID(TWayID id) const noexcept {
    for (auto& way : DImplementation->Ways) {
        if (way->ID() == id) {
            return way;
        }
    }
    return nullptr;
}