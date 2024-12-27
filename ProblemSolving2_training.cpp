#include <iostream>
#include <vector>
#include <memory>
#include <map>
#include <ctime>
#include <thread> // For sleep
#include <chrono> // For duration
using namespace std;

// Enum for vehicle type
enum class VehicleType {
    Motorcycle,
    Car,
    Truck
};

// Enum for parking spot type
enum class ParkingSpotType {
    Small,
    Medium,
    Large
};

// Enum for ticket status
enum class TicketStatus {
    Active,
    Paid,
    Lost
};

// Vehicle class
class Vehicle {
private:
    string licensePlate;
    VehicleType type;

public:
    Vehicle(string plate, VehicleType vType) : licensePlate(plate), type(vType) {}

    string getLicensePlate() const { return licensePlate; }
    VehicleType getType() const { return type; }
};

// ParkingSpot class
class ParkingSpot {
private:
    int spotNumber;
    ParkingSpotType type;
    bool isOccupied;
    shared_ptr<Vehicle> parkedVehicle;

public:
    ParkingSpot(int number, ParkingSpotType spotType) 
        : spotNumber(number), type(spotType), isOccupied(false), parkedVehicle(nullptr) {}

    int getSpotNumber() const { return spotNumber; }
    ParkingSpotType getType() const { return type; }
    bool getIsOccupied() const { return isOccupied; }

    void parkVehicle(shared_ptr<Vehicle> vehicle) {
        if (isOccupied) {
            cout << "Spot already occupied!\n";
            return;
        }
        parkedVehicle = vehicle;
        isOccupied = true;
        cout << "Vehicle " << vehicle->getLicensePlate() << " parked in spot " << spotNumber << ".\n";
    }

    void removeVehicle() {
        if (!isOccupied) {
            cout << "No vehicle to remove!\n";
            return;
        }
        parkedVehicle = nullptr;
        isOccupied = false;
        cout << "Spot " << spotNumber << " is now free.\n";
    }
};

// ParkingTicket class
class ParkingTicket {
private:
    int ticketNumber;
    string licensePlate;
    VehicleType vehicleType;
    time_t entryTime;
    TicketStatus status;

public:
    ParkingTicket(int number, string plate, VehicleType type) 
        : ticketNumber(number), licensePlate(plate), vehicleType(type), entryTime(time(nullptr)), status(TicketStatus::Active) {}

    int getTicketNumber() const { return ticketNumber; }
    string getLicensePlate() const { return licensePlate; }
    VehicleType getVehicleType() const { return vehicleType; }
    time_t getEntryTime() const { return entryTime; }
    TicketStatus getStatus() const { return status; }

    void markAsPaid() { status = TicketStatus::Paid; }
    void markAsLost() { status = TicketStatus::Lost; }
};

// ParkingLot class
class ParkingLot {
private:
    vector<shared_ptr<ParkingSpot>> spots;
    map<int, shared_ptr<ParkingTicket>> activeTickets;
    int nextTicketNumber;

public:
    ParkingLot(int smallSpots, int mediumSpots, int largeSpots) : nextTicketNumber(1) {
        int spotNumber = 1;
        for (int i = 0; i < smallSpots; i++)
            spots.push_back(shared_ptr<ParkingSpot>(new ParkingSpot(spotNumber++, ParkingSpotType::Small)));
        for (int i = 0; i < mediumSpots; i++)
            spots.push_back(shared_ptr<ParkingSpot>(new ParkingSpot(spotNumber++, ParkingSpotType::Medium)));
        for (int i = 0; i < largeSpots; i++)
            spots.push_back(shared_ptr<ParkingSpot>(new ParkingSpot(spotNumber++, ParkingSpotType::Large)));
    }

    shared_ptr<ParkingSpot> findAvailableSpot(VehicleType type) {
        ParkingSpotType requiredType;
        switch (type) {
            case VehicleType::Motorcycle:
                requiredType = ParkingSpotType::Small;
                break;
            case VehicleType::Car:
                requiredType = ParkingSpotType::Medium;
                break;
            case VehicleType::Truck:
                requiredType = ParkingSpotType::Large;
                break;
        }

        for (auto& spot : spots) {
            if (!spot->getIsOccupied() && spot->getType() == requiredType) {
                return spot;
            }
        }

        return nullptr;
    }

    shared_ptr<ParkingTicket> issueTicket(shared_ptr<Vehicle> vehicle) {
        auto spot = findAvailableSpot(vehicle->getType());
        if (!spot) {
            cout << "No available spot for vehicle " << vehicle->getLicensePlate() << ".\n";
            return nullptr;
        }

        auto ticket = shared_ptr<ParkingTicket>(new ParkingTicket(nextTicketNumber++, vehicle->getLicensePlate(), vehicle->getType()));
        activeTickets[ticket->getTicketNumber()] = ticket;
        spot->parkVehicle(vehicle);
        return ticket;
    }

    void exitVehicle(shared_ptr<ParkingTicket> ticket) {
        if (!ticket || ticket->getStatus() != TicketStatus::Active) {
            cout << "Invalid or already processed ticket!\n";
            return;
        }

        // Mark the ticket as paid and remove it from active tickets
        ticket->markAsPaid();
        activeTickets.erase(ticket->getTicketNumber());

        // Calculate and display the fee
        double fee = calculateFee(ticket);
        cout << "------------------------\n";
        cout << "Exit Details:\n";
        cout << "Ticket Number: " << ticket->getTicketNumber() << "\n";
        cout << "Vehicle Type: " << getVehicleTypeString(ticket->getVehicleType()) << "\n";
        cout << "Vehicle License Plate: " << ticket->getLicensePlate() << "\n";
        cout << "Total Payment: " << fee << " rupees\n";
        cout << "------------------------\n";
        cout << "Vehicle with ticket " << ticket->getTicketNumber() << " has exited.\n";
    }

    double calculateFee(shared_ptr<ParkingTicket> ticket) const {
        time_t currentTime = time(nullptr);
        double hoursParked = difftime(currentTime, ticket->getEntryTime()) / 3600;
        return hoursParked * 10.0; // 10 rupees per hour
    }

    string getVehicleTypeString(VehicleType type) const {
        switch (type) {
            case VehicleType::Motorcycle: return "Motorcycle";
            case VehicleType::Car: return "Car";
            case VehicleType::Truck: return "Truck";
            default: return "Unknown";
        }
    }
};

// PaymentProcessor class
class PaymentProcessor {
public:
    static bool processPayment(double amount) {
        cout << "Processing payment of " << amount << " rupees...\n";
        return true; // Simulate successful payment
    }
};

int main() {
    // Create a parking lot with 1 small, 1 medium, and 1 large spot
    ParkingLot parkingLot(1, 1, 1);

    // Create vehicles
    auto motorcycle = shared_ptr<Vehicle>(new Vehicle("MOTO123", VehicleType::Motorcycle));
    auto car = shared_ptr<Vehicle>(new Vehicle("CAR456", VehicleType::Car));
    auto truck = shared_ptr<Vehicle>(new Vehicle("TRUCK789", VehicleType::Truck));
    auto extraCar = shared_ptr<Vehicle>(new Vehicle("CAR999", VehicleType::Car));

    // Test case 1: No vehicle parked
    cout << "--- Test Case 1: No vehicle parked ---\n";
    cout << "All spots are free.\n";

    // Test case 2: Park vehicles
    cout << "\n--- Test Case 2: Parking Vehicles ---\n";
    auto motoTicket = parkingLot.issueTicket(motorcycle);
    auto carTicket = parkingLot.issueTicket(car);
    auto truckTicket = parkingLot.issueTicket(truck);

    // Test case 3: No available spot
    cout << "\n--- Test Case 3: No available spot ---\n";
    auto extraCarTicket = parkingLot.issueTicket(extraCar);

    // Introduce a delay to simulate parked duration
    cout << "\nVehicles parked. Simulating time passage...\n";
    this_thread::sleep_for(chrono::seconds(5)); // Simulate a 5-second parking duration

    // Test case 4: Exit vehicles and process payments
    cout << "\n--- Test Case 4: Exiting Vehicles ---\n";
    if (motoTicket) {
        double fee = parkingLot.calculateFee(motoTicket);
        if (PaymentProcessor::processPayment(fee)) {
            parkingLot.exitVehicle(motoTicket);
        }
    }

    if (carTicket) {
        double fee = parkingLot.calculateFee(carTicket);
        if (PaymentProcessor::processPayment(fee)) {
            parkingLot.exitVehicle(carTicket);
        }
    }

    if (truckTicket) {
        double fee = parkingLot.calculateFee(truckTicket);
        if (PaymentProcessor::processPayment(fee)) {
            parkingLot.exitVehicle(truckTicket);
        }
    }

    return 0;
}
