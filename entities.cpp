#include "entities.h"

// ---------------- Location ----------------
Location::Location(int id, const std::string& name) : id(id), name(name) {}
int Location::getId() const { return id; }
std::string Location::getName() const { return name; }

// ---------------- Emergency ----------------
Emergency::Emergency(int id, const std::string& description,
                     int severity, int waitMinutes, int locationId)
    : id(id), description(description), severity(severity),
      waitMinutes(waitMinutes), locationId(locationId) {}

int Emergency::getId() const { return id; }
std::string Emergency::getDescription() const { return description; }
int Emergency::getSeverity() const { return severity; }
int Emergency::getWaitMinutes() const { return waitMinutes; }
int Emergency::getLocationId() const { return locationId; }

int Emergency::priorityScore() const {
    return severity * 100 + waitMinutes;
}

// ---------------- Ambulance ----------------
Ambulance::Ambulance(int id, int locationId)
    : id(id), locationId(locationId), available(true) {}

int Ambulance::getId() const { return id; }
int Ambulance::getLocationId() const { return locationId; }
bool Ambulance::isAvailable() const { return available; }
void Ambulance::setAvailable(bool value) { available = value; }

// ---------------- BasicAmbulance ----------------
BasicAmbulance::BasicAmbulance(int id, int locationId)
    : Ambulance(id, locationId) {}

bool BasicAmbulance::canHandle(int severity) const {
    return severity <= 3;
}

std::string BasicAmbulance::typeName() const { return "Basic"; }

// ---------------- ICUAmbulance ----------------
ICUAmbulance::ICUAmbulance(int id, int locationId)
    : Ambulance(id, locationId) {}

bool ICUAmbulance::canHandle(int severity) const {
    (void)severity;   // an ICU unit can handle every severity
    return true;
}

std::string ICUAmbulance::typeName() const { return "ICU"; }

// ---------------- Hospital ----------------
Hospital::Hospital(int id, const std::string& name, int locationId, int totalBeds)
    : id(id), name(name), locationId(locationId),
      totalBeds(totalBeds), freeBeds(totalBeds) {}

int Hospital::getId() const { return id; }
std::string Hospital::getName() const { return name; }
int Hospital::getLocationId() const { return locationId; }
int Hospital::getTotalBeds() const { return totalBeds; }
int Hospital::getFreeBeds() const { return freeBeds; }

bool Hospital::hasFreeBed() const { return freeBeds > 0; }

bool Hospital::admitPatient() {
    if (freeBeds <= 0) return false;
    freeBeds--;
    return true;
}
