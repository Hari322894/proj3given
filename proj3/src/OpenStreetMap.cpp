#include "OpenStreetMap.h"
#include "XMLReader.h"
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

// Define implementation structure first
struct COpenStreetMap::SImplementation {
    // Forward declarations of implementation classes
    class SNodeImpl;
    class SWayImpl;

    std::vector<std::shared_ptr<SNodeImpl>> Nodes;
    std::vector<std::shared_ptr<SWayImpl>> Ways;
};

// Now define the implementation classes
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

COpenStreetMap::COpenStreetMap(std::shared_ptr<CXMLReader> src) {
    DImplementation = std::make_unique<SImplementation>();
    
    SXMLEntity entity;
    std::shared_ptr<SImplementation::SNodeImpl> currentNode = nullptr;
    std::shared_ptr<SImplementation::SWayImpl> currentWay = nullptr;
    
    // Parse the XML file
    while (src->ReadEntity(entity)) {
        if (entity.DType == SXMLEntity::EType::StartElement) {
            if (entity.DNameData == "node") {
                // Create a new node
                currentNode = std::make_shared<SImplementation::SNodeImpl>();
                currentWay = nullptr;
                
                // Process node attributes
                for (const auto& attr : entity.DAttributes) {
                    if (attr.first == "id") {
                        currentNode->NodeID = std::stoull(attr.second);
                    } 
                    else if (attr.first == "lat") {
                        double lat = std::stod(attr.second);
                        if (currentNode->NodeLocation.first == 0.0) { // Initialize with lat first
                            currentNode->NodeLocation.first = lat;
                        }
                    } 
                    else if (attr.first == "lon") {
                        double lon = std::stod(attr.second);
                        if (currentNode->NodeLocation.second == 0.0) { // Then lon
                            currentNode->NodeLocation.second = lon;
                        }
                    } 
                    else {
                        // Store other attributes
                        currentNode->Attributes[attr.first] = attr.second;
                    }
                }
            } 
            else if (entity.DNameData == "way") {
                // Create a new way
                currentWay = std::make_shared<SImplementation::SWayImpl>();
                currentNode = nullptr;
                
                // Process way attributes
                for (const auto& attr : entity.DAttributes) {
                    if (attr.first == "id") {
                        currentWay->WayID = std::stoull(attr.second);
                    } 
                    else {
                        // Store other attributes
                        currentWay->Attributes[attr.first] = attr.second;
                    }
                }
            } 
            else if (entity.DNameData == "nd" && currentWay) {
                // Process node reference inside a way
                for (const auto& attr : entity.DAttributes) {
                    if (attr.first == "ref") {
                        TNodeID nodeID = std::stoull(attr.second);
                        currentWay->NodeIDs.push_back(nodeID);
                    }
                }
            } 
            else if (entity.DNameData == "tag") {
                // Process tag element for both nodes and ways
                std::string key, value;
                for (const auto& attr : entity.DAttributes) {
                    if (attr.first == "k") {
                        key = attr.second;
                    } 
                    else if (attr.first == "v") {
                        value = attr.second;
                    }
                }
                
                if (!key.empty()) {
                    if (currentNode) {
                        currentNode->Attributes[key] = value;
                    } 
                    else if (currentWay) {
                        currentWay->Attributes[key] = value;
                    }
                }
            }
        } 
        else if (entity.DType == SXMLEntity::EType::EndElement) {
            if (entity.DNameData == "node" && currentNode) {
                // Finish processing a node
                DImplementation->Nodes.push_back(currentNode);
                currentNode = nullptr;
            } 
            else if (entity.DNameData == "way" && currentWay) {
                // Finish processing a way
                DImplementation->Ways.push_back(currentWay);
                currentWay = nullptr;
            }
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
        // Return the implementation as its base class
        return std::static_pointer_cast<CStreetMap::SNode>(DImplementation->Nodes[index]);
    }
    return nullptr;
}

std::shared_ptr<CStreetMap::SNode> COpenStreetMap::NodeByID(TNodeID id) const noexcept {
    for (auto& node : DImplementation->Nodes) {
        if (node->ID() == id) {
            // Return the implementation as its base class
            return std::static_pointer_cast<CStreetMap::SNode>(node);
        }
    }
    return nullptr;
}

std::shared_ptr<CStreetMap::SWay> COpenStreetMap::WayByIndex(std::size_t index) const noexcept {
    if (index < DImplementation->Ways.size()) {
        // Return the implementation as its base class
        return std::static_pointer_cast<CStreetMap::SWay>(DImplementation->Ways[index]);
    }
    return nullptr;
}

std::shared_ptr<CStreetMap::SWay> COpenStreetMap::WayByID(TWayID id) const noexcept {
    for (auto& way : DImplementation->Ways) {
        if (way->ID() == id) {
            // Return the implementation as its base class
            return std::static_pointer_cast<CStreetMap::SWay>(way);
        }
    }
    return nullptr;
}