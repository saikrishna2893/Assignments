#include <vector>
#include <iostream>
#include <map>
#include <ctime>
#include <stdexcept>
using namespace std;

// VehicleType, ParkingSpotType, TicketStatus enums
enum class VehicleType { Motorcycle, Car, Truck };
enum class ParkingSpotType { Small, Medium, Large };
enum class TicketStatus { Active, Paid, Lost };

// Vehicle Class
class Vehicle {
private:
    std::string licensePlate;
    VehicleType type;

public:
    Vehicle(std::string licensePlate, VehicleType type)
        : licensePlate(licensePlate), type(type) {}

    std::string getLicensePlate() const { return licensePlate; }
    VehicleType getType() const { return type; }
};

// ParkingSpot Class
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
    
    void parkVehicle(Vehicle* vehicle) {
        if (isOccupied) throw runtime_error("Spot is already occupied.");
        parkedVehicle = vehicle;
        isOccupied = true;
    }
    
    void removeVehicle() {
        if (!isOccupied) throw runtime_error("No vehicle to remove.");
        parkedVehicle = nullptr;
        isOccupied = false;
    }
};

// ParkingTicket Class
class ParkingTicket {
private:
    int ticketNumber;
    std::string licensePlate;
    time_t entryTime;
    TicketStatus status;

public:
    ParkingTicket() : ticketNumber(0), licensePlate(""), entryTime(0), status(TicketStatus::Active) {}
    ParkingTicket(int ticketNumber, std::string licensePlate, time_t entryTime)
        : ticketNumber(ticketNumber), licensePlate(licensePlate), entryTime(entryTime), status(TicketStatus::Active) {}

    int getTicketNumber() const { return ticketNumber; }
    std::string getLicensePlate() const { return licensePlate; }
    time_t getEntryTime() const { return entryTime; }
    TicketStatus getStatus() const { return status; }
    
    void markAsPaid() { status = TicketStatus::Paid; }
    void markAsLost() { status = TicketStatus::Lost; }
};

// ParkingLot Class
class ParkingLot {
private:
    std::vector<ParkingSpot> spots;
    std::map<int, ParkingTicket> activeTickets;
    int nextTicketNumber;

public:

    ParkingLot(int numSmallSpots, int numMediumSpots, int numLargeSpots)
        : nextTicketNumber(1) {
        int spotNumber = 1;
        for (int i = 0; i < numSmallSpots; ++i) spots.emplace_back(spotNumber++, ParkingSpotType::Small);
        for (int i = 0; i < numMediumSpots; ++i) spots.emplace_back(spotNumber++, ParkingSpotType::Medium);
        for (int i = 0; i < numLargeSpots; ++i) spots.emplace_back(spotNumber++, ParkingSpotType::Large);
    }

    ParkingTicket issueTicket(Vehicle* vehicle) {
        time_t now = time(0);
        ParkingTicket ticket(nextTicketNumber++, vehicle->getLicensePlate(), now);
        activeTickets[ticket.getTicketNumber()] = ticket;
        return ticket;
    }

    void parkVehicle(ParkingTicket& ticket, Vehicle* vehicle) {
        ParkingSpot* spot = findAvailableSpot(vehicle->getType());
        if (!spot) throw runtime_error("No available parking spot for this vehicle type.");
        spot->parkVehicle(vehicle);
    }

    void exitVehicle(ParkingTicket& ticket) {
        auto it = activeTickets.find(ticket.getTicketNumber());
        if (it == activeTickets.end()) throw runtime_error("Invalid ticket.");
        activeTickets.erase(it);
    }

    double calculateFee(ParkingTicket& ticket) const {
        time_t now = time(0);
        double duration = difftime(now, ticket.getEntryTime()) / 3600.0;
        double ratePerHour = 10.0;
        return duration * ratePerHour;
    }

    void payTicket(ParkingTicket& ticket) { ticket.markAsPaid(); }

    ParkingSpot* findAvailableSpot(VehicleType type) {
        ParkingSpotType requiredType;
        switch (type) {
        case VehicleType::Motorcycle: requiredType = ParkingSpotType::Small; break;
        case VehicleType::Car: requiredType = ParkingSpotType::Medium; break;
        case VehicleType::Truck: requiredType = ParkingSpotType::Large; break;
        }
        for (auto& spot : spots) {
            if (spot.getType() == requiredType && !spot.getIsOccupied()) return &spot;
        }
        return nullptr;
    }
};

// PaymentProcessor Class
class PaymentProcessor {
public:
    static bool processPayment(double amount) {
        cout << "Payment of $" << amount << " processed successfully." << endl;
        return true;
    }
};

// Main Program
int main() {
    ParkingLot parkingLot(10, 10, 5);
    std::map<int, ParkingTicket> activeTickets;
    int choice;

    while (true) {
        cout << "\n1. Issue Ticket\n2. Park Vehicle\n3. Exit Vehicle\n4. Pay Ticket\n5. Exit\n";
        cin >> choice;

        switch (choice) {
        case 1:
            // Issue a ticket
            {
                string vehicleTypeInput, licensePlate;
                cout << "Enter the vehicle's license plate: ";
                cin >> licensePlate;

                cout << "Enter the vehicle type (Motorcycle, Car, Truck): ";
                cin >> vehicleTypeInput;

                VehicleType vehicleType;
                if (vehicleTypeInput == "Motorcycle") vehicleType = VehicleType::Motorcycle;
                else if (vehicleTypeInput == "Car") vehicleType = VehicleType::Car;
                else if (vehicleTypeInput == "Truck") vehicleType = VehicleType::Truck;
                else { cout << "Invalid vehicle type"; break; }

                Vehicle vehicle(licensePlate, vehicleType);
                ParkingTicket ticket = parkingLot.issueTicket(&vehicle);
                activeTickets[ticket.getTicketNumber()] = ticket;
                cout << "Ticket issued. Ticket Number: " << ticket.getTicketNumber() << endl;
            }
            break;
        case 2:
            // Park a vehicle
            {
                int ticketNumber;
                cout << "Enter ticket number: ";
                cin >> ticketNumber;

                auto it = activeTickets.find(ticketNumber);
                if (it == activeTickets.end()) {
                    cout << "Invalid ticket number." << endl;
                } else {
                    Vehicle vehicle(it->second.getLicensePlate(), VehicleType::Car);
                    parkingLot.parkVehicle(it->second, &vehicle);
                    cout << "Vehicle parked successfully." << endl;
                }
            }
            break;
        case 3:
            // Exit vehicle
            {
                int ticketNumber;
                cout << "Enter ticket number: ";
                cin >> ticketNumber;

                auto it = activeTickets.find(ticketNumber);
                if (it != activeTickets.end()) {
                    double fee = parkingLot.calculateFee(it->second);
                    cout << "Fee: $" << fee << endl;
                    if (PaymentProcessor::processPayment(fee)) {
                        parkingLot.payTicket(it->second);
                        activeTickets.erase(it);
                        cout << "Vehicle exited and ticket paid." << endl;
                    }
                } else {
                    cout << "Invalid ticket number." << endl;
                }
            }
            break;
        case 4:
            // Pay for a ticket
            {
                int ticketNumber;
                cout << "Enter ticket number: ";
                cin >> ticketNumber;

                auto it = activeTickets.find(ticketNumber);
                if (it != activeTickets.end()) {
                    double fee = parkingLot.calculateFee(it->second);
                    cout << "Fee: $" << fee << endl;
                    if (PaymentProcessor::processPayment(fee)) {
                        parkingLot.payTicket(it->second);
                        cout << "Ticket paid successfully." << endl;
                    }
                } else {
                    cout << "Invalid ticket number." << endl;
                }
            }
            break;
        case 5: //Exit system
            cout << "Exiting system." << endl;
            return 0;
        default:
            cout << "Invalid choice. Please try again." << endl;
        }
    }
}
