#include "CSVBusSystem.h"
#include "DSVReader.h"
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

class CCSVBusSystem::SStop : public CBusSystem::SStop {
public:
    TStopID StopID;
    CStreetMap::TNodeID NodeIDValue;

    TStopID ID() const noexcept override {
        return StopID;
    }

    CStreetMap::TNodeID NodeID() const noexcept override {
        return NodeIDValue;
    }
};

class CCSVBusSystem::SRoute : public CBusSystem::SRoute {
public:
    std::string RouteName;
    std::vector<TStopID> RouteStops;

    std::string Name() const noexcept override {
        return RouteName;
    }

    std::size_t StopCount() const noexcept override {
        return RouteStops.size();
    }

    TStopID GetStopID(std::size_t index) const noexcept override {
        if (index < RouteStops.size()) {
            return RouteStops[index];
        }
        return CBusSystem::InvalidStopID;
    }
};

struct CCSVBusSystem::SImplementation {
    std::unordered_map<TStopID, std::shared_ptr<SStop>> Stops;
    std::unordered_map<std::string, std::shared_ptr<SRoute>> Routes;
};

CCSVBusSystem::CCSVBusSystem(std::shared_ptr<CDSVReader> stopsrc, std::shared_ptr<CDSVReader> routesrc) {
    DImplementation = std::make_unique<SImplementation>();

    // Load stops
    std::vector<std::string> row;
    while (stopsrc->ReadRow(row)) {
        auto stop = std::make_shared<SStop>();
        stop->StopID = std::stoul(row[0]); // Assuming stop_id is the first column
        stop->NodeIDValue = std::stoul(row[1]); // Assuming node_id is the second column
        DImplementation->Stops[stop->StopID] = stop;
    }

    // Load routes
    while (routesrc->ReadRow(row)) {
        std::string routeName = row[0]; // Assuming route is the first column
        if (DImplementation->Routes.find(routeName) == DImplementation->Routes.end()) {
            DImplementation->Routes[routeName] = std::make_shared<SRoute>();
            DImplementation->Routes[routeName]->RouteName = routeName;
        }
        TStopID stopID = std::stoul(row[1]); // Assuming stop_id is the second column
        DImplementation->Routes[routeName]->RouteStops.push_back(stopID);
    }
}

CCSVBusSystem::~CCSVBusSystem() = default;

std::size_t CCSVBusSystem::StopCount() const noexcept {
    return DImplementation->Stops.size();
}

std::size_t CCSVBusSystem::RouteCount() const noexcept {
    return DImplementation->Routes.size();
}

std::shared_ptr<CBusSystem::SStop> CCSVBusSystem::StopByIndex(std::size_t index) const noexcept {
    if (index < DImplementation->Stops.size()) {
        auto it = DImplementation->Stops.begin();
        std::advance(it, index);
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<CBusSystem::SStop> CCSVBusSystem::StopByID(TStopID id) const noexcept {
    if (DImplementation->Stops.find(id) != DImplementation->Stops.end()) {
        return DImplementation->Stops[id];
    }
    return nullptr;
}

std::shared_ptr<CBusSystem::SRoute> CCSVBusSystem::RouteByIndex(std::size_t index) const noexcept {
    if (index < DImplementation->Routes.size()) {
        auto it = DImplementation->Routes.begin();
        std::advance(it, index);
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<CBusSystem::SRoute> CCSVBusSystem::RouteByName(const std::string &name) const noexcept {
    if (DImplementation->Routes.find(name) != DImplementation->Routes.end()) {
        return DImplementation->Routes[name];
    }
    return nullptr;
}