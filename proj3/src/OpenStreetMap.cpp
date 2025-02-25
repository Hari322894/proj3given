#include "OpenStreetMap.h"
#include "XMLReader.h"
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

// Create concrete implementations of the abstract SNode and SWay
class COpenStreetMap::SImplementation::SNodeImpl : public CStreetMap::SNode {
public:
    TNodeID NodeID;
    TLocation NodeLocation;
    std::unordered_map<std::string, std::string> Attributes;

    TNodeID ID() const noexcept override {
        return NodeID;
    }

    TLocation Location() const noexcept override {
        return NodeLocation;
    }

    std::size_t AttributeCount() const noexcept override {
        return Attributes.size();
    }

    std::string GetAttributeKey(std::size_t index) const noexcept override {
        if (index < Attributes.size()) {
            auto it = Attributes.begin();
            std::advance(it, index);
            return it->first;
        }
        return "";
    }

    bool HasAttribute(const std::string &key) const noexcept override {
        return Attributes.find(key) != Attributes.end();
    }

    std::string GetAttribute(const std::string &key) const noexcept override {
        auto it = Attributes.find(key);
        if (it != Attributes.end()) {
            return it->second;
        }
        return "";
    }
};

class COpenStreetMap::SImplementation::SWayImpl : public CStreetMap::SWay {
public:
    TWayID WayID;
    std::vector<TNodeID> NodeIDs;
    std::unordered_map<std::string, std::string> Attributes;

    TWayID ID() const noexcept override {
        return WayID;
    }

    std::size_t NodeCount() const noexcept override {
        return NodeIDs.size();
    }

    TNodeID GetNodeID(std::size_t index) const noexcept override {
        if (index < NodeIDs.size()) {
            return NodeIDs[index];
        }
        return CStreetMap::InvalidNodeID;
    }

    std::size_t AttributeCount() const noexcept override {
        return Attributes.size();
    }

    std::string GetAttributeKey(std::size_t index) const noexcept override {
        if (index < Attributes.size()) {
            auto it = Attributes.begin();
            std::advance(it, index);
            return it->first;
        }
        return "";
    }

    bool HasAttribute(const std::string &key) const noexcept override {
        return Attributes.find(key) != Attributes.end();
    }

    std::string GetAttribute(const std::string &key) const noexcept override {
        auto it = Attributes.find(key);
        if (it != Attributes.end()) {
            return it->second;
        }
        return "";
    }
};

struct COpenStreetMap::SImplementation {
    std::vector<std::shared_ptr<SNodeImpl>> Nodes;
    std::vector<std::shared_ptr<SWayImpl>> Ways;
    
    // Define nested classes here
    class SNodeImpl;
    class SWayImpl;
};

COpenStreetMap::COpenStreetMap(std::shared_ptr<CXMLReader> src) {
    DImplementation = std::make_unique<SImplementation>();
    
    // Parse XML and load nodes and ways
    // You'll need to adjust this based on your actual CXMLReader interface
    while (src->ReadNode()) {
        if (src->GetNodeType() == "node") {
            auto node = std::make_shared<SImplementation::SNodeImpl>();
            
            // Parse node attributes from XML
            node->NodeID = std::stoull(src->GetAttribute("id"));
            
            // Parse latitude and longitude
            double lat = std::stod(src->GetAttribute("lat"));
            double lon = std::stod(src->GetAttribute("lon"));
            node->NodeLocation = CStreetMap::TLocation(lat, lon);
            
            // Read all attributes
            for (size_t i = 0; i < src->GetAttributeCount(); ++i) {
                std::string key = src->GetAttributeKey(i);
                if (key != "id" && key != "lat" && key != "lon") {
                    node->Attributes[key] = src->GetAttribute(key);
                }
            }
            
            DImplementation->Nodes.push_back(node);
        } 
        else if (src->GetNodeType() == "way") {
            auto way = std::make_shared<SImplementation::SWayImpl>();
            
            // Parse way ID
            way->WayID = std::stoull(src->GetAttribute("id"));
            
            // Read all attributes
            for (size_t i = 0; i < src->GetAttributeCount(); ++i) {
                std::string key = src->GetAttributeKey(i);
                if (key != "id") {
                    way->Attributes[key] = src->GetAttribute(key);
                }
            }
            
            // Parse node references (will need to be adapted based on XML structure)
            while (src->ReadNode() && src->GetNodeType() != "way") {
                if (src->GetNodeType() == "nd") {
                    TNodeID refID = std::stoull(src->GetAttribute("ref"));
                    way->NodeIDs.push_back(refID);
                }
            }
            
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