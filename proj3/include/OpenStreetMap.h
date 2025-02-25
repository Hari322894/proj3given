#ifndef OPEN_STREET_MAP_H
#define OPEN_STREET_MAP_H

#include "StreetMap.h"
#include "XMLReader.h"
#include <memory>
#include <vector>
#include <string>

class COpenStreetMap : public CStreetMap {
public:
    COpenStreetMap(std::shared_ptr<CXMLReader> src);
    ~COpenStreetMap();

    std::size_t NodeCount() const noexcept override;
    std::size_t WayCount() const noexcept override;
    std::shared_ptr<CStreetMap::SNode> NodeByIndex(std::size_t index) const noexcept override;
    std::shared_ptr<CStreetMap::SNode> NodeByID(TNodeID id) const noexcept override;
    std::shared_ptr<CStreetMap::SWay> WayByIndex(std::size_t index) const noexcept override;
    std::shared_ptr<CStreetMap::SWay> WayByID(TWayID id) const noexcept override;

private:
    struct SImplementation;
    std::unique_ptr<SImplementation> DImplementation;
};

#endif // OPEN_STREET_MAP_H