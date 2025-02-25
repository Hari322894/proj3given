#include "CSVBusSystem.h"
#include "DSVReader.h"
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>

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
        if (index >= RouteStops.size()) {
            return CBusSystem::InvalidStopID;
        }
        return RouteStops[index];
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

    std::vector<std::string> row;
    if (stopsrc) {
        while (stopsrc->ReadRow(row)) {
            if (row.size() >= 2) {
                try {
                    auto stop = std::make_shared<SStop>();
                    stop->StopID = std::stoul(row[0]);
                    stop->NodeIDValue = std::stoul(row[1]);

                    DImplementation->Stops[stop->StopID] = stop;
                    DImplementation->StopsByIndex.push_back(stop);
                } catch (const std::exception&) {
                    // Handle error (optional logging or fallback behavior)
                }
            }
        }
    }

    if (routesrc) {
        std::unordered_map<std::string, std::shared_ptr<SRoute>> tempRoutes;

        while (routesrc->ReadRow(row)) {
            if (row.size() >= 2) {
                try {
                    std::string routeName = row[0];
                    TStopID stopID = std::stoul(row[1]);

                    auto& route = tempRoutes[routeName];
                    if (!route) {
                        route = std::make_shared<SRoute>();
                        route->RouteName = routeName;
                    }
                    route->RouteStops.push_back(stopID);
                } catch (const std::exception&) {
                    // Handle error (optional logging or fallback behavior)
                }
            }
        }

        for (const auto& pair : tempRoutes) {
            DImplementation->Routes[pair.first] = pair.second;
            DImplementation->RoutesByIndex.push_back(pair.second);
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
    if (it != DImplementation->Stops.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<CBusSystem::SRoute> CCSVBusSystem::RouteByIndex(std::size_t index) const noexcept {
    if (index < DImplementation->RoutesByIndex.size()) {
        return DImplementation->RoutesByIndex[index];
    }
    return nullptr;
}

std::shared_ptr<CBusSystem::SRoute> CCSVBusSystem::RouteByName(const std::string &name) const noexcept {
    auto it = DImplementation->Routes.find(name);
    if (it != DImplementation->Routes.end()) {
        return it->second;
    }
    return nullptr;
}


std::ostream &operator<<(std::ostream &os, const CCSVBusSystem &bussystem) {
    os << "StopCount: " + std::to_string(bussystem.StopCount()) + "\n";
    os << "RouteCount: " + std::to_string(bussystem.RouteCount()) + "\n";
    
    for (size_t i = 0; i < bussystem.StopCount(); ++i) {
        auto stop = bussystem.StopByIndex(i);
        if (stop) {
            os << "Index " + std::to_string(i) + " ID: " + std::to_string(stop->ID()) +
                  " NodeID: " + std::to_string(stop->NodeID()) + " ISSA: No\n";
        }
    }
    
    for (size_t i = 0; i < bussystem.RouteCount(); ++i) {
        auto route = bussystem.RouteByIndex(i);
        if (route) {
            os << "Route Index " + std::to_string(i) + " Name: " + route->Name() +
                  " StopCount: " + std::to_string(route->StopCount()) + "\n";
            
            for (size_t j = 0; j < route->StopCount(); ++j) {
                os << "RouteStop " + std::to_string(i) + "," + std::to_string(j) + ": " +
                      std::to_string(route->GetStopID(j)) + "\n";
            }
        }
    }
    
    return os;
}
