#ifndef RESOURCE_REGISTRY_H
#define RESOURCE_REGISTRY_H

#include <unordered_map>
#include <vector>
#include <memory>
#include <algorithm>
#include "entities.h"

// ---------------------------------------------------------------
// Resource registry: ambulances and hospitals are stored in hash
// tables (std::unordered_map) keyed by their ID, so a lookup by ID
// takes O(1) on average instead of scanning a whole list.
// The registry owns the objects (unique_ptr), so no manual delete.
// ---------------------------------------------------------------
class ResourceRegistry {
private:
    std::unordered_map<int, std::unique_ptr<Ambulance> > ambulances;
    std::unordered_map<int, std::unique_ptr<Hospital> > hospitals;

public:
    void addAmbulance(std::unique_ptr<Ambulance> a) {
        int id = a->getId();
        ambulances[id] = std::move(a);
    }

    void addHospital(std::unique_ptr<Hospital> h) {
        int id = h->getId();
        hospitals[id] = std::move(h);
    }

    // returns nullptr if the ID does not exist
    Ambulance* findAmbulance(int id) const {
        auto it = ambulances.find(id);
        return (it == ambulances.end()) ? nullptr : it->second.get();
    }

    Hospital* findHospital(int id) const {
        auto it = hospitals.find(id);
        return (it == hospitals.end()) ? nullptr : it->second.get();
    }

    // Sorted by ID so that tie-breaking (and the output) is repeatable
    std::vector<Ambulance*> allAmbulances() const {
        std::vector<Ambulance*> list;
        for (const auto& pair : ambulances) list.push_back(pair.second.get());
        std::sort(list.begin(), list.end(),
                  [](Ambulance* a, Ambulance* b) { return a->getId() < b->getId(); });
        return list;
    }

    std::vector<Hospital*> allHospitals() const {
        std::vector<Hospital*> list;
        for (const auto& pair : hospitals) list.push_back(pair.second.get());
        std::sort(list.begin(), list.end(),
                  [](Hospital* a, Hospital* b) { return a->getId() < b->getId(); });
        return list;
    }
};

#endif
