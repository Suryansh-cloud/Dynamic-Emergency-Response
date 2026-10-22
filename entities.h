#ifndef ENTITIES_H
#define ENTITIES_H

#include <string>

// ---------------------------------------------------------------
// Location: a named node of the city road network
// ---------------------------------------------------------------
class Location {
private:
    int id;
    std::string name;

public:
    Location(int id, const std::string& name);
    int getId() const;
    std::string getName() const;
};

// ---------------------------------------------------------------
// Emergency: one incoming call
// Priority score = severity * 100 + minutes already waited
// (severity dominates; waiting time breaks ties and slowly lifts
//  lower-severity cases)
// ---------------------------------------------------------------
class Emergency {
private:
    int id;
    std::string description;
    int severity;      // 1 (minor) .. 5 (critical)
    int waitMinutes;   // time already spent waiting
    int locationId;    // node in the road graph

public:
    Emergency(int id, const std::string& description,
              int severity, int waitMinutes, int locationId);

    int getId() const;
    std::string getDescription() const;
    int getSeverity() const;
    int getWaitMinutes() const;
    int getLocationId() const;

    int priorityScore() const;
};

// ---------------------------------------------------------------
// Ambulance: abstract base class (inheritance + polymorphism)
// ---------------------------------------------------------------
class Ambulance {
protected:
    int id;
    int locationId;   // where the ambulance is parked
    bool available;

public:
    Ambulance(int id, int locationId);
    virtual ~Ambulance() {}

    int getId() const;
    int getLocationId() const;
    bool isAvailable() const;
    void setAvailable(bool value);

    // Pure virtual: each ambulance type decides what it can handle
    virtual bool canHandle(int severity) const = 0;
    virtual std::string typeName() const = 0;
};

// Basic life support: only low / medium severity (1-3)
class BasicAmbulance : public Ambulance {
public:
    BasicAmbulance(int id, int locationId);
    bool canHandle(int severity) const override;
    std::string typeName() const override;
};

// Intensive care unit ambulance: can handle every severity (1-5)
class ICUAmbulance : public Ambulance {
public:
    ICUAmbulance(int id, int locationId);
    bool canHandle(int severity) const override;
    std::string typeName() const override;
};

// ---------------------------------------------------------------
// Hospital: has a fixed number of beds
// ---------------------------------------------------------------
class Hospital {
private:
    int id;
    std::string name;
    int locationId;
    int totalBeds;
    int freeBeds;

public:
    Hospital(int id, const std::string& name, int locationId, int totalBeds);

    int getId() const;
    std::string getName() const;
    int getLocationId() const;
    int getTotalBeds() const;
    int getFreeBeds() const;

    bool hasFreeBed() const;
    bool admitPatient();   // returns false if no bed is free
};

#endif
