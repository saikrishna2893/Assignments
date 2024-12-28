#include <vector>
#include <iostream>
#include <map>
#include <ctime>
#include <stdexcept>
#include <algorithm>
using namespace std;

//VehicleType: Represents different types of vehicles that can park.
enum class VehicleType {
    Motorcycle,
    Car,
    Truck
};
//ParkingSpotType: Represents different types of parking spots.
enum class ParkingSpotType {
    Small,
    Medium,
    Large
};
//TicketStatus: Represents the status of a parking ticket.
enum class TicketStatus {
    Active,
    Paid,
    Lost
};

//Vehicle: Represents a vehicle entering the parking lot.
class Vehicle {
private:
    string licensePlate;
    VehicleType type;

public:
    Vehicle(string licensePlate, VehicleType type) 
        : licensePlate(licensePlate), type(type) {}

    string getLicensePlate() const { return licensePlate; }
    VehicleType getType() const { return type; }
};

//ParkingSpot: Represents a parking spot in the parking lot.
class ParkingSpot {
private:
    int spotNumber;
    ParkingSpotType type;
    bool isOccupied;
    Vehicle* parkedVehicle;

public:
    ParkingSpot(int spotNumber, ParkingSpotType type)
        : spotNumber(spotNumber), type(type), isOccupied(false), parkedVehicle(nullptr) {}

    int getSpotNumber() const { return spotNumber; }
    ParkingSpotType getType() const { return type; }
    bool getIsOccupied() const { return isOccupied; }
    Vehicle* getParkedVehicle() const { return parkedVehicle; }
    void parkVehicle(Vehicle* vehicle) {
        if (isOccupied)
            throw runtime_error("Spot is already occupied.");
        parkedVehicle = vehicle;
        isOccupied = true;
    }
    void removeVehicle() {
        if (!isOccupied)
            throw runtime_error("No vehicle to remove.");
        parkedVehicle = nullptr;
        isOccupied = false;
    }
};

//ParkingTicket: Represents a parking ticket issued to a vehicle.
class ParkingTicket {
private:
    int ticketNumber;
    string licensePlate;
    time_t entryTime;
    TicketStatus status;

public:
    ParkingTicket()
        : ticketNumber(0), licensePlate(""), entryTime(0), status(TicketStatus::Active) {}

    ParkingTicket(int ticketNumber, string licensePlate, time_t entryTime)
        : ticketNumber(ticketNumber), licensePlate(licensePlate), entryTime(entryTime), status(TicketStatus::Active) {}

    int getTicketNumber() const { return ticketNumber; }
    string getLicensePlate() const { return licensePlate; }
    time_t getEntryTime() const { return entryTime; }
    TicketStatus getStatus() const { return status; }
    void markAsPaid() { status = TicketStatus::Paid; }
    void markAsLost() { status = TicketStatus::Lost; }
};

//ParkingLot: Manages the overall parking lot.
class ParkingLot {
private:
    vector<ParkingSpot> spots;
    int nextTicketNumber;
public:
    map<int, Vehicle*> vehicles;
    map<int, ParkingTicket> activeTickets;
    ParkingLot(int numSmallSpots, int numMediumSpots, int numLargeSpots) : nextTicketNumber(1) {
        int spotNumber = 1;
        for (int i = 0; i < numSmallSpots; ++i)
            spots.emplace_back(spotNumber++, ParkingSpotType::Small);
        for (int i = 0; i < numMediumSpots; ++i)
            spots.emplace_back(spotNumber++, ParkingSpotType::Medium);
        for (int i = 0; i < numLargeSpots; ++i)
            spots.emplace_back(spotNumber++, ParkingSpotType::Large);
    }
    vector<ParkingSpot> getSpots() const { return spots; }
    ParkingTicket issueTicket(Vehicle* vehicle) {
        time_t now = time(0);
        ParkingTicket ticket(nextTicketNumber++, vehicle->getLicensePlate(), now);
        activeTickets[ticket.getTicketNumber()] = ticket;
        return ticket;
    }
    void parkVehicle(ParkingTicket& ticket, Vehicle* vehicle) {
        ParkingSpot* spot = findAvailableSpot(vehicle->getType());
        if (!spot) {
            activeTickets.erase(ticket.getTicketNumber());
            throw runtime_error("No available parking spot for this vehicle type.");
        }
        spot->parkVehicle(vehicle);
    }
    void exitVehicle(ParkingTicket& ticket) {
        auto it = activeTickets.find(ticket.getTicketNumber());
        if (it == activeTickets.end())
            throw runtime_error("Invalid ticket.");
        ParkingSpot* spot = getVehicleSpot(vehicles[ticket.getTicketNumber()]);
        if (!spot)
            throw runtime_error("Vehicle not found in parking lot.");
        spot->removeVehicle();
        activeTickets.erase(it);
    }
    double calculateFee(ParkingTicket& ticket) const {
        time_t now = time(0);
        double duration = difftime(now, ticket.getEntryTime()) / 3600.0;
        double ratePerHour = 10.0;
        return duration * ratePerHour;
    }
    void payTicket(ParkingTicket& ticket)  {
        ticket.markAsPaid();
    }
    ParkingSpot* findAvailableSpot(VehicleType type) {
        ParkingSpotType requiredType;
        switch (type) {
            case VehicleType::Motorcycle: requiredType = ParkingSpotType::Small; break;
            case VehicleType::Car: requiredType = ParkingSpotType::Medium; break;
            case VehicleType::Truck: requiredType = ParkingSpotType::Large; break;
        }
        for (auto& spot : spots) {
            if (spot.getType() == requiredType && !spot.getIsOccupied())
                return &spot;
        }
        return nullptr;
    }
    ParkingSpot* getVehicleSpot(Vehicle* vehicle) {
        for (auto& spot : spots) {
            if (spot.getIsOccupied() && spot.getParkedVehicle() == vehicle)
                return &spot;
        }
        return nullptr;
    }
};

//PaymentProcessor: Handles payment for parking tickets.
class PaymentProcessor {
public:
    static bool processPayment(double amount) {
        cout << "Payment of $" << amount << " processed successfully." << endl;
        return true;
    }
};

int main() {
    int numSmallSpots = 10, numMediumSpots = 10, numLargeSpots = 5; // initialize the number of small, medium, and large spots
    ParkingLot parkingLot(numSmallSpots, numMediumSpots, numLargeSpots); // Initialize parking lot with small, medium, and large spots

    int choice = 0;
    do {
        cout << "\n--- Parking Lot Management ---\n";
        cout << "1. Park Vehicle\n";
        cout << "2. Exit Vehicle\n";
        cout << "3. View All Tickets\n";
        cout << "4. View Available spots\n";
        cout << "5. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: { // Park Vehicle
                string licensePlate;
                int vehicleTypeInt;
                cout << "Enter license plate: ";
                cin >> licensePlate;

                // if licenseplate already exists then throw error
                auto it = find_if(parkingLot.activeTickets.begin(), parkingLot.activeTickets.end(), [&](const auto& pair) {
                    return pair.second.getLicensePlate() == licensePlate;
                });
                if (it != parkingLot.activeTickets.end()) {
                    cout << "Vehicle with this license plate is already parked.\n";
                    break;
                }
                cout << "Select vehicle type (1: Motorcycle, 2: Car, 3: Truck): ";
                cin >> vehicleTypeInt;

                cout << endl;
                VehicleType type;
                if (vehicleTypeInt == 1)
                    type = VehicleType::Motorcycle;
                else if (vehicleTypeInt == 2)
                    type = VehicleType::Car;
                else if (vehicleTypeInt == 3)
                    type = VehicleType::Truck;
                else {
                    cout << "Invalid vehicle type.\n";
                    break;
                }

                try {
                    Vehicle* vehicle = new Vehicle(licensePlate, type);
                    ParkingTicket ticket = parkingLot.issueTicket(vehicle);
                    parkingLot.parkVehicle(ticket, vehicle);

                    parkingLot.vehicles[ticket.getTicketNumber()] = vehicle;
                    parkingLot.activeTickets[ticket.getTicketNumber()] = ticket;

                    cout << "Vehicle parked. Ticket number: " << ticket.getTicketNumber() << "\n";
                } catch (const exception& e) {
                    cout << "Error: " << e.what() << "\n";
                }
                break;
            }
            case 2: { // Exit Vehicle
                int ticketNumber;
                cout << "Enter ticket number: ";
                cin >> ticketNumber;

                cout << endl;
                if (parkingLot.activeTickets.find(ticketNumber) == parkingLot.activeTickets.end()) {
                    cout << "Invalid ticket number.\n";
                    break;
                }

                try {
                    ParkingTicket& ticket = parkingLot.activeTickets[ticketNumber];
                    double fee = parkingLot.calculateFee(ticket);
                    cout << "Parking fee: $" << fee << "\n";

                    if (PaymentProcessor::processPayment(fee)) {
                        parkingLot.payTicket(ticket);
                        parkingLot.exitVehicle(ticket);

                        // Clean up resources
                        delete parkingLot.vehicles[ticketNumber];
                        parkingLot.vehicles.erase(ticketNumber);
                        parkingLot.activeTickets.erase(ticketNumber);

                        cout << "Vehicle exited successfully.\n";
                    }
                } catch (const exception& e) {
                    cout << "Error: " << e.what() << "\n";
                }
                break;
            }
            case 3: { // View All Tickets
                if (parkingLot.activeTickets.empty()) {
                    cout << "No active tickets.\n";
                } else {
                    cout << "\nActive Tickets:\n";

                    for (const auto& pair : parkingLot.activeTickets) {
                        const ParkingTicket& ticket = pair.second;
                        time_t entryTime = ticket.getEntryTime();
                        
                        cout << "Ticket Number: " << ticket.getTicketNumber()
                             << " | License Plate: " << ticket.getLicensePlate()
                             << " | Entry Time: " << ctime(&entryTime)
                             << " | Status: " << (ticket.getStatus() == TicketStatus::Active ? "Active" : "Paid") << "\n";
                    }
                }
                break;
            }
            case 4: {// view count of available spots in parking lot
                int smallSpots = 0, mediumSpots = 0, largeSpots = 0;
                for (const auto& spot : parkingLot.getSpots()) {
                    if (!spot.getIsOccupied()) {
                        switch (spot.getType()) {
                            case ParkingSpotType::Small: smallSpots++; break;
                            case ParkingSpotType::Medium: mediumSpots++; break;
                            case ParkingSpotType::Large: largeSpots++; break;
                        }
                    }
                }
                cout << "\nAvailable Spots:\n";
                cout << "Small: " << smallSpots << " | Medium: " << mediumSpots << " | Large: " << largeSpots << "\n";
                break;
            }
            case 5: // Exit
                cout << "\nExiting program !\n";
                break;

            default:
                cout << "\nInvalid choice. Please try again.\n";
        }
    } while (choice != 5);

    return 0;
}