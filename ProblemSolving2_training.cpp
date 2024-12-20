#include <vector>
#include <iostream>
#include <map>
using namespace std;

// Design an automated parking lot.
// What we need as “design”:
// • classes / enums, etc.
// • fields, properties, functions
// • signatures of functions
// • instantiation
// • interaction-sequence

//Classes and Enums
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
//Classes
//Vehicle: Represents a vehicle entering the parking lot.
class Vehicle {
private:
    std::string licensePlate;
    VehicleType type;

public:
    Vehicle(std::string licensePlate, VehicleType type) 
        : licensePlate(licensePlate), type(type) {}

    std::string getLicensePlate() const;
    VehicleType getType() const;
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

    int getSpotNumber() const;
    ParkingSpotType getType() const;
    bool getIsOccupied() const;
    void parkVehicle(Vehicle* vehicle);
    void removeVehicle();
};
//ParkingTicket: Represents a parking ticket issued to a vehicle.
class ParkingTicket {
private:
    int ticketNumber;
    std::string licensePlate;
    time_t entryTime;
    TicketStatus status;

public:
    ParkingTicket(int ticketNumber, std::string licensePlate, time_t entryTime)
        : ticketNumber(ticketNumber), licensePlate(licensePlate), entryTime(entryTime), status(TicketStatus::Active) {}

    int getTicketNumber() const;
    std::string getLicensePlate() const;
    time_t getEntryTime() const;
    TicketStatus getStatus() const;
    void markAsPaid();
    void markAsLost();
};
//ParkingLot: Manages the overall parking lot.
class ParkingLot {
private:
    std::vector<ParkingSpot> spots;
    std::map<int, ParkingTicket> activeTickets;
    int nextTicketNumber;

public:
    ParkingLot(int numSmallSpots, int numMediumSpots, int numLargeSpots);
    ParkingTicket issueTicket(Vehicle* vehicle);
    void parkVehicle(ParkingTicket& ticket, Vehicle* vehicle);
    void exitVehicle(ParkingTicket& ticket);
    double calculateFee(ParkingTicket& ticket) const;
    void payTicket(ParkingTicket& ticket);
    ParkingSpot* findAvailableSpot(VehicleType type);
};

//PaymentProcessor: Handles payment for parking tickets.
class PaymentProcessor {
public:
    static bool processPayment(double amount);
};


int main() {
    // Create a parking lot with 10 small spots, 10 medium spots, and 5 large spots
    ParkingLot parkingLot(10, 10, 5);

    // Create a vehicle
    Vehicle car("ABC123", VehicleType::Car);

    // Issue a ticket to the vehicle
    ParkingTicket ticket = parkingLot.issueTicket(&car);

    // Park the vehicle
    parkingLot.parkVehicle(ticket, &car);

    // Simulate vehicle exit
    parkingLot.exitVehicle(ticket);

    // Calculate fee and pay
    double fee = parkingLot.calculateFee(ticket);
    PaymentProcessor::processPayment(fee);
    parkingLot.payTicket(ticket);

    return 0;
}

// Fields, Properties, and Functions
// Vehicle Class
// Fields:

// std::string licensePlate: License plate number of the vehicle.
// VehicleType type: Type of vehicle.
// Properties:

// std::string getLicensePlate() const: Returns the license plate.
// VehicleType getType() const: Returns the type of vehicle.
// ParkingSpot Class
// Fields:

// int spotNumber: Unique identifier for the parking spot.
// ParkingSpotType type: Type of parking spot (small, medium, large).
// bool isOccupied: Indicates if the spot is occupied.
// Vehicle* parkedVehicle: Pointer to the parked vehicle.
// Properties:

// int getSpotNumber() const: Returns the spot number.
// ParkingSpotType getType() const: Returns the type of parking spot.
// bool getIsOccupied() const: Returns the occupancy status.
// Functions:

// void parkVehicle(Vehicle* vehicle): Parks a vehicle in the spot.
// void removeVehicle(): Removes a vehicle from the spot.
// ParkingTicket Class
// Fields:

// int ticketNumber: Unique identifier for the ticket.
// std::string licensePlate: License plate of the vehicle.
// time_t entryTime: Time when the vehicle entered.
// TicketStatus status: Current status of the ticket.
// Properties:

// int getTicketNumber() const: Returns the ticket number.
// std::string getLicensePlate() const: Returns the license plate.
// time_t getEntryTime() const: Returns the entry time.
// TicketStatus getStatus() const: Returns the ticket status.
// Functions:

// void markAsPaid(): Marks the ticket as paid.
// void markAsLost(): Marks the ticket as lost.
// ParkingLot Class
// Fields:

// std::vector<ParkingSpot> spots: List of parking spots.
// std::map<int, ParkingTicket> activeTickets: Map of active tickets.
// int nextTicketNumber: Counter for the next ticket number.
// Functions:

// ParkingTicket issueTicket(Vehicle* vehicle): Issues a parking ticket.
// void parkVehicle(ParkingTicket& ticket, Vehicle* vehicle): Parks a vehicle using a ticket.
// void exitVehicle(ParkingTicket& ticket): Exits a vehicle from the lot.
// double calculateFee(ParkingTicket& ticket) const: Calculates the parking fee.
// void payTicket(ParkingTicket& ticket): Processes payment for the ticket.
// ParkingSpot* findAvailableSpot(VehicleType type): Finds an available spot for a vehicle type.
// PaymentProcessor Class
// Functions:
// static bool processPayment(double amount): Processes payment and returns success/failure.


//Instantiation
// A vehicle arrives at the parking lot and a ticket is issued.

// The ParkingLot object calls the issueTicket(Vehicle* vehicle) method.
// A ParkingTicket is generated with a unique ticket number and entry time.
// The vehicle enters the parking lot and parks in an available spot.

// findAvailableSpot(VehicleType type) is called to find a suitable spot.
// parkVehicle(ParkingTicket& ticket, Vehicle* vehicle) is called to park the vehicle in the spot.
// When the driver returns, they use the ticket to exit the parking lot.

// exitVehicle(ParkingTicket& ticket) is called to remove the vehicle from the spot.
// The parking fee is calculated based on the duration.

// calculateFee(ParkingTicket& ticket) is called to compute the fee.
// The driver pays the fee.

// PaymentProcessor::processPayment(double amount) is called to handle the payment.
// payTicket(ParkingTicket& ticket) marks the ticket as paid.
