#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "entities.h"
#include "emergency_heap.h"
#include "road_graph.h"
#include "resource_registry.h"

// "2 -> 4 -> 9"
static std::string routeToString(const std::vector<int>& path) {
    std::string s;
    for (std::size_t i = 0; i < path.size(); i++) {
        if (i > 0) s += " -> ";
        s += std::to_string(path[i]);
    }
    return s;
}

int main() {
    // ------------------------------------------------------------
    // 1. Sample city: 10 locations (node ids 1..10)
    // ------------------------------------------------------------
    std::vector<Location> city = {
        Location(1, "Central Station"), Location(2, "Clock Tower"),
        Location(3, "Market Square"),   Location(4, "Old Bridge"),
        Location(5, "University Gate"), Location(6, "Bus Terminal"),
        Location(7, "City Park"),       Location(8, "General Hospital Area"),
        Location(9, "Medical College Area"), Location(10, "Airport Road")
    };

    // ------------------------------------------------------------
    // 2. Road network (undirected, weight = km)
    // ------------------------------------------------------------
    RoadGraph graph(10);
    graph.addRoad(1, 2, 4);
    graph.addRoad(1, 3, 3);
    graph.addRoad(2, 3, 6);
    graph.addRoad(2, 4, 5);
    graph.addRoad(3, 5, 4);
    graph.addRoad(4, 6, 3);
    graph.addRoad(4, 9, 7);
    graph.addRoad(5, 6, 6);
    graph.addRoad(5, 7, 3);
    graph.addRoad(6, 9, 4);
    graph.addRoad(7, 8, 2);
    graph.addRoad(8, 9, 3);
    graph.addRoad(8, 10, 5);
    graph.addRoad(9, 10, 4);

    // ------------------------------------------------------------
    // 3. Resources stored in hash tables (by ID)
    // ------------------------------------------------------------
    ResourceRegistry registry;
    registry.addAmbulance(std::unique_ptr<Ambulance>(new BasicAmbulance(101, 1)));
    registry.addAmbulance(std::unique_ptr<Ambulance>(new ICUAmbulance(102, 8)));
    registry.addAmbulance(std::unique_ptr<Ambulance>(new BasicAmbulance(103, 3)));
    registry.addAmbulance(std::unique_ptr<Ambulance>(new ICUAmbulance(104, 10)));
    registry.addAmbulance(std::unique_ptr<Ambulance>(new ICUAmbulance(105, 4)));

    registry.addHospital(std::unique_ptr<Hospital>(new Hospital(201, "General Hospital", 8, 2)));
    registry.addHospital(std::unique_ptr<Hospital>(new Hospital(202, "Medical College", 9, 3)));

    // quick demonstration of the hash lookup
    Ambulance* sample = registry.findAmbulance(101);
    if (sample != nullptr)
        std::cout << "Ambulance 101 lookup: " << sample->typeName() << " ambulance found\n";

    // ------------------------------------------------------------
    // 4. Five simultaneous emergencies go into the max-heap
    //    Emergency(id, description, severity, waitMinutes, locationNode)
    // ------------------------------------------------------------
    EmergencyHeap queue;
    queue.push(Emergency(1, "Road accident",       5,  5, 2));
    queue.push(Emergency(2, "Heart emergency",     5,  0, 7));
    queue.push(Emergency(3, "Fracture",            2, 30, 3));
    queue.push(Emergency(4, "Breathing difficulty", 3, 15, 6));
    queue.push(Emergency(5, "Critical patient",    4, 10, 5));

    std::cout << "\n=== EMERGENCY DISPATCH SYSTEM ===\n";
    std::cout << "Pending emergencies: " << queue.size() << "\n";

    // ------------------------------------------------------------
    // 5. Dispatch loop: always serve the highest priority first
    // ------------------------------------------------------------
    while (!queue.empty()) {
        Emergency e = queue.pop();

        std::cout << "\nEmergency #" << e.getId() << ": " << e.getDescription() << "\n";
        std::cout << "Location: " << city[e.getLocationId() - 1].getName()
                  << " (node " << e.getLocationId() << ")\n";
        std::cout << "Severity: " << e.getSeverity()
                  << " | Waiting: " << e.getWaitMinutes() << " min"
                  << " | Priority score: " << e.priorityScore() << "\n";

        // ---- (a) nearest free ambulance that can handle this severity
        Ambulance* bestAmb = nullptr;
        PathResult toScene;
        for (Ambulance* a : registry.allAmbulances()) {
            if (!a->isAvailable() || !a->canHandle(e.getSeverity())) continue;
            PathResult r = graph.shortestPath(a->getLocationId(), e.getLocationId());
            if (!r.found) continue;
            if (bestAmb == nullptr || r.distance < toScene.distance) {
                bestAmb = a;
                toScene = r;
            }
        }
        if (bestAmb == nullptr) {
            std::cout << "No compatible free ambulance available.\n";
            continue;
        }

        // ---- (b) nearest hospital that still has a free bed
        Hospital* bestHosp = nullptr;
        PathResult toHospital;
        for (Hospital* h : registry.allHospitals()) {
            if (!h->hasFreeBed()) continue;
            PathResult r = graph.shortestPath(e.getLocationId(), h->getLocationId());
            if (!r.found) continue;
            if (bestHosp == nullptr || r.distance < toHospital.distance) {
                bestHosp = h;
                toHospital = r;
            }
        }
        if (bestHosp == nullptr) {
            std::cout << "No hospital with a free bed available.\n";
            continue;
        }

        // ---- (c) commit the allocation
        bestAmb->setAvailable(false);
        bestHosp->admitPatient();

        std::cout << "Assigned ambulance: " << bestAmb->getId()
                  << " (" << bestAmb->typeName() << ") from node "
                  << bestAmb->getLocationId() << "\n";
        std::cout << "Ambulance-to-emergency route: " << routeToString(toScene.path)
                  << " | Distance: " << toScene.distance << " km\n";
        std::cout << "Hospital: " << bestHosp->getName()
                  << " (ID " << bestHosp->getId() << ")\n";
        std::cout << "Emergency-to-hospital route: " << routeToString(toHospital.path)
                  << " | Distance: " << toHospital.distance << " km\n";
        std::cout << "Remaining hospital beds: " << bestHosp->getFreeBeds() << "\n";
    }

    std::cout << "\n=== DISPATCH SIMULATION FINISHED ===\n";
    std::cout << "Final bed status:\n";
    for (Hospital* h : registry.allHospitals())
        std::cout << "  " << h->getName() << ": " << h->getFreeBeds()
                  << " of " << h->getTotalBeds() << " beds free\n";

    int freeAmb = 0;
    for (Ambulance* a : registry.allAmbulances())
        if (a->isAvailable()) freeAmb++;
    std::cout << "Ambulances still available: " << freeAmb << " of 5\n";

    return 0;
}
