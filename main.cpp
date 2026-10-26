#include <iomanip>
#include <iostream>
#include <string>

const int MAX_STATIONS = 10;

// Abstraction: common blueprint for every charging station.
class ChargingStation {
private:
    std::string id;
    std::string location;
    double pricePerKWh;
    bool available;

protected:
    double price() const { return pricePerKWh; }

public:
    ChargingStation(const std::string& stationId, const std::string& stationLocation,
                    double rate)
        : id(stationId), location(stationLocation), pricePerKWh(rate),
          available(true) {}
    virtual ~ChargingStation() {}

    std::string getId() const { return id; }
    bool isAvailable() const { return available; }
    void startCharging() { available = false; }
    void stopCharging() { available = true; }

    virtual std::string type() const = 0;
    virtual double calculateBill(double units) const = 0;

    void show() const {
        std::cout << id << " | " << type() << " | " << location
                  << " | Rs. " << std::fixed << std::setprecision(2)
                  << pricePerKWh << "/kWh | "
                  << (available ? "Available" : "Busy") << "\n";
    }
};

// Inheritance: AC station gets common data from ChargingStation.
class ACStation : public ChargingStation {
public:
    ACStation(const std::string& id, const std::string& location, double rate)
        : ChargingStation(id, location, rate) {}

    std::string type() const { return "AC Charger"; }
    double calculateBill(double units) const {
        return units * price();
    }
};

// Inheritance: Fast station has an extra flat service charge.
class FastStation : public ChargingStation {
private:
    double serviceCharge;

public:
    FastStation(const std::string& id, const std::string& location, double rate,
                double extraCharge)
        : ChargingStation(id, location, rate), serviceCharge(extraCharge) {}

    std::string type() const { return "DC Fast Charger"; }
    double calculateBill(double units) const {
        return (units * price()) + serviceCharge;
    }
};

ChargingStation* findStation(ChargingStation* stations[], int count,
                             const std::string& id) {
    for (int i = 0; i < count; ++i) {
        if (stations[i]->getId() == id) return stations[i];
    }
    return 0;
}

void listStations(ChargingStation* stations[], int count) {
    std::cout << "\n--- Stations ---\n";
    for (int i = 0; i < count; ++i) stations[i]->show();
}

// Polymorphism: calculateBill() selects AC or Fast billing at runtime.
void chargeVehicle(ChargingStation* stations[], int count) {
    std::string id;
    double units = 0.0;

    std::cout << "Station ID: ";
    std::getline(std::cin >> std::ws, id);
    ChargingStation* station = findStation(stations, count, id);

    if (station == 0 || !station->isAvailable()) {
        std::cout << "Station is not available.\n";
        return;
    }

    std::cout << "Energy used (kWh): ";
    std::cin >> units;
    if (units <= 0) {
        std::cout << "Enter a positive value.\n";
        return;
    }

    station->startCharging();
    std::cout << "Charging complete. Bill: Rs. " << std::fixed
              << std::setprecision(2) << station->calculateBill(units) << "\n";
    station->stopCharging();
}

void addStation(ChargingStation* stations[], int& count) {
    if (count == MAX_STATIONS) {
        std::cout << "Station limit reached.\n";
        return;
    }

    int choice = 0;
    std::string id, location;
    double rate = 0.0;

    std::cout << "1. AC Charger  2. DC Fast Charger: ";
    std::cin >> choice;
    std::cout << "Station ID: ";
    std::getline(std::cin >> std::ws, id);
    std::cout << "Location: ";
    std::getline(std::cin, location);
    std::cout << "Price per kWh: ";
    std::cin >> rate;

    if (choice == 1) {
        stations[count++] = new ACStation(id, location, rate);
        std::cout << "AC station added.\n";
    } else if (choice == 2) {
        double serviceCharge;
        std::cout << "Fast-charge service fee: ";
        std::cin >> serviceCharge;
        stations[count++] = new FastStation(id, location, rate, serviceCharge);
        std::cout << "Fast station added.\n";
    } else {
        std::cout << "Invalid station type.\n";
    }
}

int main() {
    // Fixed-size array: no vector is used.
    ChargingStation* stations[MAX_STATIONS];
    int count = 2;

    stations[0] = new ACStation("AC-01", "Mall Parking", 12.50);
    stations[1] = new FastStation("DC-01", "Highway Stop", 16.00, 50.00);

    int option = 0;
    do {
        std::cout << "\nEV Charging Station Management System\n"
                  << "1. Show stations\n"
                  << "2. Charge vehicle\n"
                  << "3. Add station\n"
                  << "0. Exit\n"
                  << "Choose: ";
        std::cin >> option;

        if (option == 1) listStations(stations, count);
        else if (option == 2) chargeVehicle(stations, count);
        else if (option == 3) addStation(stations, count);
        else if (option != 0) std::cout << "Invalid option.\n";
    } while (option != 0);

    for (int i = 0; i < count; ++i) delete stations[i];
    return 0;
}
