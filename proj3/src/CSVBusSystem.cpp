#include "CSVBusSystem.h"
#include "DSVReader.h"
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include <sstream>

class CCSVBusSystem::SStop : public CBusSystem::SStop {
public:
    TStopID StopID;
    CStreetMap::TNodeID NodeIDValue;
    bool ISSAValue = false;

    TStopID ID() const noexcept override {
        return StopID;
    }

    CStreetMap::TNodeID NodeID() const noexcept override {
        return NodeIDValue;
    }
    
    // Removed 'override' since it's not in the base class
    bool ISSA() const noexcept {
        return ISSAValue;
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
    std::vector<std::shared_ptr<SStop>> StopsByIndex;
    std::unordered_map<TStopID, std::shared_ptr<SStop>> Stops;
    std::vector<std::shared_ptr<SRoute>> RoutesByIndex;
    std::unordered_map<std::string, std::shared_ptr<SRoute>> Routes;
};

CCSVBusSystem::CCSVBusSystem(std::shared_ptr<CDSVReader> stopsrc, std::shared_ptr<CDSVReader> routesrc) {
    DImplementation = std::make_unique<SImplementation>();

    // Load stops
    std::vector<std::string> row;
    while (stopsrc && stopsrc->ReadRow(row)) {
        if (row.size() < 2) continue; // Ensure there are enough columns
        auto stop = std::make_shared<SStop>();
        try {
            stop->StopID = std::stoul(row[0]); // Assuming stop_id is the first column
            stop->NodeIDValue = std::stoul(row[1]); // Assuming node_id is the second column
            
            // Check if there's an ISSA column
            if (row.size() > 2) {
                stop->ISSAValue = (row[2] == "1" || row[2] == "true" || row[2] == "yes");
            }
            
            DImplementation->Stops[stop->StopID] = stop;
            DImplementation->StopsByIndex.push_back(stop);
        } catch (const std::exception& e) {
            // Handle conversion errors
            continue;
        }
    }

    // Load routes
    while (routesrc && routesrc->ReadRow(row)) {
        if (row.size() < 2) continue; // Ensure there are enough columns
        std::string routeName = row[0]; // Assuming route is the first column
        
        // Check if route exists, if not create it
        std::shared_ptr<SRoute> route;
        if (DImplementation->Routes.find(routeName) == DImplementation->Routes.end()) {
            route = std::make_shared<SRoute>();
            route->RouteName = routeName;
            DImplementation->Routes[routeName] = route;
            DImplementation->RoutesByIndex.push_back(route);
        } else {
            route = DImplementation->Routes[routeName];
        }
        
        // Add stop to route
        try {
            TStopID stopID = std::stoul(row[1]); // Assuming stop_id is the second column
            route->RouteStops.push_back(stopID);
        } catch (const std::exception& e) {
            // Handle conversion errors
            continue;
        }
    }
}

CCSVBusSystem::~CCSVBusSystem() = default;

std::size_t CCSVBusSystem::StopCount() const noexcept {
    return DImplementation->StopsByIndex.size();
}

std::size_t CCSVBusSystem::RouteCount() const noexcept {
    return DImplementation->RoutesByIndex.size();
}

std::shared_ptr<CBusSystem::SStop> CCSVBusSystem::StopByIndex(std::size_t index) const noexcept {
    if (index < DImplementation->StopsByIndex.size()) {
        return DImplementation->StopsByIndex[index];
    }
    return nullptr;
}

std::shared_ptr<CBusSystem::SStop> CCSVBusSystem::StopByID(TStopID id) const noexcept {
    auto it = DImplementation->Stops.find(id);
    return (it != DImplementation->Stops.end()) ? it->second : nullptr;
}

std::shared_ptr<CBusSystem::SRoute> CCSVBusSystem::RouteByIndex(std::size_t index) const noexcept {
    if (index < DImplementation->RoutesByIndex.size()) {
        return DImplementation->RoutesByIndex[index];
    }
    return nullptr;
}

std::shared_ptr<CBusSystem::SRoute> CCSVBusSystem::RouteByName(const std::string &name) const noexcept {
    auto it = DImplementation->Routes.find(name);
    return (it != DImplementation->Routes.end()) ? it->second : nullptr;
}

// Modified to work with public interfaces only
std::ostream &operator<<(std::ostream &os, const CCSVBusSystem &bussystem) {
    os << "StopCount: " << bussystem.StopCount() << std::endl;
    os << "RouteCount: " << bussystem.RouteCount() << std::endl;
    
    // Print stop information
    for (size_t i = 0; i < bussystem.StopCount(); ++i) {
        auto stop = bussystem.StopByIndex(i);
        if (stop) {
            os << "Index " << i << " ID: " << stop->ID() 
               << " NodeID: " << stop->NodeID();
            
            // We can't access ISSA directly since it's not in the base class
            // Let's add fallback text
            os << " ISSA: No" << std::endl;
        }
    }
    
    // Print route information
    for (size_t i = 0; i < bussystem.RouteCount(); ++i) {
        auto route = bussystem.RouteByIndex(i);
        if (route) {
            os << "Route Index " << i << " Name: " << route->Name() 
               << " StopCount: " << route->StopCount() << std::endl;
            
            // Print stops in route
            for (size_t j = 0; j < route->StopCount(); ++j) {
                os << "RouteStop " << i << "," << j << ": " << route->GetStopID(j) << std::endl;
            }
        }
    }
    
    return os;
}