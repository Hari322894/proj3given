#include "CSVBusSystem.h"
#include "DSVReader.h"
#include <memory>
#include <vector>
#include <string>
#include <iostream>

struct CCSVBusSystem::SImplementation {
    std::vector<std::shared_ptr<SStop>> Stops;
    std::vector<std::shared_ptr<SRoute>> Routes;
};

CCSVBusSystem::CCSVBusSystem(std::shared_ptr<CDSVReader> stopsrc, std::shared_ptr<CDSVReader> routesrc) {
    DImplementation = std::make_unique<SImplementation>();

    // Load stops
    std::vector<std::string> row;
    while (stopsrc->ReadRow(row)) {
        auto stop = std::make_shared<SStop>();
        stop->ID = std::stoul(stopsrc->GetString(row, "stop_id"));
        stop->NodeID = std::stoul(stopsrc->GetString(row, "node_id"));
        DImplementation->Stops.push_back(stop);
    }

    // Load routes
    while (routesrc->ReadRow(row)) {
        std::string routeName = routesrc->GetString(row, "route");
        auto route = std::make_shared<SRoute>();
        route->Name = routeName;
        
        for (auto& stop : DImplementation->Stops) {
            if (stop->ID == std::stoul(routesrc->GetString(row, "stop_id"))) {
                route->Stops.push_back(stop);
            }
        }
        DImplementation->Routes.push_back(route);
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
        return DImplementation->Stops[index];
    }
    return nullptr;
}

std::shared_ptr<CBusSystem::SStop> CCSVBusSystem::StopByID(TStopID id) const noexcept {
    for (auto& stop : DImplementation->Stops) {
        if (stop->ID == id) {
            return stop;
        }
    }
    return nullptr;
}

std::shared_ptr<CBusSystem::SRoute> CCSVBusSystem::RouteByIndex(std::size_t index) const noexcept {
    if (index < DImplementation->Routes.size()) {
        return DImplementation->Routes[index];
    }
    return nullptr;
}

std::shared_ptr<CBusSystem::SRoute> CCSVBusSystem::RouteByName(const std::string &name) const noexcept {
    for (auto& route : DImplementation->Routes) {
        if (route->Name == name) {
            return route;
        }
    }
    return nullptr;
}