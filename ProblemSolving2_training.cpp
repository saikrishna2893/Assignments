#include <vector>
#include <iostream>
#include <map>
#include <ctime>
#include <stdexcept>
using namespace std;

// Design an automated parking lot.
// What we need as “design”:
// • classes / enums, etc.
// • fields, properties, functions
// • signatures of functions
// • instantiation
// • interaction-sequence

// Classes and Enums
// VehicleType: Represents different types of vehicles that can park.
enum class VehicleType
{
    Motorcycle,
    Car,
    Truck
};
// ParkingSpotType: Represents different types of parking spots.
enum class ParkingSpotType
{
    Small,
    Medium,
    Large
};
// TicketStatus: Represents the status of a parking ticket.
enum class TicketStatus
{
    Active,
    Paid,
    Lost
};
// Classes
// Vehicle: Represents a vehicle entering the parking lot.
class Vehicle
{
private:
    std::string licensePlate;
    VehicleType type;

public:
    Vehicle(std::string licensePlate, VehicleType type)
        : licensePlate(licensePlate), type(type) {}

    std::string getLicensePlate() const { return licensePlate; }
    VehicleType getType() const { return type; }
};
// ParkingSpot: Represents a parking spot in the parking lot.
class ParkingSpot
{
private:
    int spotNumber;
    ParkingSpotType type;
    bool isOccupied;
    Vehicle *parkedVehicle;

public:
    ParkingSpot(int spotNumber, ParkingSpotType type)
        : spotNumber(spotNumber), type(type), isOccupied(false), parkedVehicle(nullptr) {}

    int getSpotNumber() const { return spotNumber; }
    ParkingSpotType getType() const { return type; }
    bool getIsOccupied() const { return isOccupied; }
    void parkVehicle(Vehicle *vehicle)
    {
        if (isOccupied)
            throw runtime_error("Spot is already occupied.");
        parkedVehicle = vehicle;
        isOccupied = true;
    }
    void removeVehicle()
    {
        if (!isOccupied)
            throw runtime_error("No vehicle to remove.");
        parkedVehicle = nullptr;
        isOccupied = false;
    }
};
// ParkingTicket: Represents a parking ticket issued to a vehicle.
class ParkingTicket
{
private:
    int ticketNumber;
    std::string licensePlate;
    time_t entryTime;
    TicketStatus status;

public:
    ParkingTicket()
        : ticketNumber(0), licensePlate(""), entryTime(0), status(TicketStatus::Active) {}

    ParkingTicket(int ticketNumber, std::string licensePlate, time_t entryTime)
        : ticketNumber(ticketNumber), licensePlate(licensePlate), entryTime(entryTime), status(TicketStatus::Active) {}

    int getTicketNumber() const { return ticketNumber; }
    std::string getLicensePlate() const { return licensePlate; }
    time_t getEntryTime() const { return entryTime; }
    TicketStatus getStatus() const { return status; }
    void markAsPaid() { status = TicketStatus::Paid; }
    void markAsLost() { status = TicketStatus::Lost; }

    void setEntryTime(time_t newEntryTime) { entryTime = newEntryTime; }
};
// ParkingLot: Manages the overall parking lot.
class ParkingLot
{
private:
    std::vector<ParkingSpot> spots;
    std::map<int, ParkingTicket> activeTickets;
    int nextTicketNumber;

public:
    ParkingLot(int numSmallSpots, int numMediumSpots, int numLargeSpots) : nextTicketNumber(1)
    {
        int spotNumber = 1;
        for (int i = 0; i < numSmallSpots; ++i)
            spots.emplace_back(spotNumber++, ParkingSpotType::Small);
        for (int i = 0; i < numMediumSpots; ++i)
            spots.emplace_back(spotNumber++, ParkingSpotType::Medium);
        for (int i = 0; i < numLargeSpots; ++i)
            spots.emplace_back(spotNumber++, ParkingSpotType::Large);
    }
    ParkingTicket issueTicket(Vehicle *vehicle)
    {
        time_t now = time(0);
        ParkingTicket ticket(nextTicketNumber++, vehicle->getLicensePlate(), now);
        activeTickets[ticket.getTicketNumber()] = ticket;
        return ticket;
    }
    void parkVehicle(ParkingTicket &ticket, Vehicle *vehicle)
    {
        ParkingSpot *spot = findAvailableSpot(vehicle->getType());
        if (!spot)
            throw runtime_error("No available parking spot for this vehicle type.");
        spot->parkVehicle(vehicle);
    }
    void exitVehicle(ParkingTicket &ticket)
    {
        auto it = activeTickets.find(ticket.getTicketNumber());
        if (it == activeTickets.end())
            throw runtime_error("Invalid ticket.");
        activeTickets.erase(it);
    }
    double calculateFee(ParkingTicket &ticket) const
    {
        time_t now = time(0);
        double duration = difftime(now, ticket.getEntryTime()) / 3600.0; // Hours
        double ratePerHour = 10.0;                                       // Flat rate
        return duration * ratePerHour;
    }

    void payTicket(ParkingTicket &ticket)
    {
        ticket.markAsPaid();
    }
    ParkingSpot *findAvailableSpot(VehicleType type)
    {
        ParkingSpotType requiredType;
        switch (type)
        {
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
        for (auto &spot : spots)
        {
            if (spot.getType() == requiredType && !spot.getIsOccupied())
                return &spot;
        }
        return nullptr;
    }
};

// PaymentProcessor: Handles payment for parking tickets.
class PaymentProcessor
{
public:
    static bool processPayment(double amount)
    {
        cout << "Payment of $" << amount << " processed successfully." << endl;
        return true;
    }
};
void searchByLicensePlate(const std::string &licensePlate, const std::map<int, ParkingTicket> &activeTickets)
{
    bool found = false;

    std::cout << "\n--- Search Results ---\n";

    for (const auto &entry : activeTickets)
    {
        const ParkingTicket &ticket = entry.second;
        if (ticket.getLicensePlate() == licensePlate)
        {
            std::cout << "Ticket Number: " << ticket.getTicketNumber()
                      << ", License Plate: " << ticket.getLicensePlate()
                      << ", Status: " << (ticket.getStatus() == TicketStatus::Paid ? "Paid" : "Active")
                      << "\n";
            found = true;
        }
    }

    if (!found)
    {
        std::cout << "No active tickets found for license plate: " << licensePlate << "\n";
    }
}

VehicleType getVehicleTypeFromInput(const std::string &input)
{
    if (input == "Motorcycle")
    {
        return VehicleType::Motorcycle;
    }
    else if (input == "Car")
    {
        return VehicleType::Car;
    }
    else if (input == "Truck")
    {
        return VehicleType::Truck;
    }
    else
    {
        throw std::invalid_argument("Invalid vehicle type.");
    }
}

void displayMenu()
{
    std::cout << "\n--- Parking Lot Management System ---\n";
    std::cout << "1. Issue a Ticket\n";
    std::cout << "2. View All Active Tickets\n";
    std::cout << "3. Park Vehicle\n";
    std::cout << "4. Exit Vehicle\n";
    std::cout << "5. Exit Program\n";
    std::cout << "6. Search by License Plate\n";
    std::cout << "Enter your choice: ";
}

int main()
{
    ParkingLot parkingLot(10, 10, 5);
    std::map<int, ParkingTicket> activeTickets;
    int choice;

    while (true)
    {
        displayMenu();
        std::cin >> choice;

        switch (choice)
        {
        case 1:
        {
            // Issue a ticket
            std::string vehicleTypeInput, licensePlate;

            std::cout << "Enter the vehicle's license plate: ";
            std::cin >> licensePlate;

            std::cout << "Enter the vehicle type (Motorcycle, Car, Truck): ";
            std::cin >> vehicleTypeInput;

            VehicleType vehicleType;
            try
            {
                vehicleType = getVehicleTypeFromInput(vehicleTypeInput);
            }
            catch (const std::invalid_argument &e)
            {
                std::cout << "Error: " << e.what() << std::endl;
                break;
            }

            Vehicle vehicle(licensePlate, vehicleType);
            ParkingTicket ticket = parkingLot.issueTicket(&vehicle);
            activeTickets[ticket.getTicketNumber()] = ticket;

            std::cout << "Ticket issued. Ticket Number: " << ticket.getTicketNumber() << std::endl;
            break;
        }
        case 2:
        {
            // View all active tickets
            if (activeTickets.empty())
            {
                std::cout << "No active tickets.\n";
            }
            else
            {
                std::cout << "\n--- Active Tickets ---\n";
                for (const auto &entry : activeTickets)
                {
                    const ParkingTicket &ticket = entry.second;
                    std::cout << "Ticket Number: " << ticket.getTicketNumber()
                              << ", License Plate: " << ticket.getLicensePlate()
                              << ", Status: " << (ticket.getStatus() == TicketStatus::Paid ? "Paid" : "Active")
                              << "\n";
                }
            }
            break;
        }
        case 3:
        {
            // Park a vehicle
            int ticketNumber;
            std::cout << "Enter the ticket number to park the vehicle: ";
            std::cin >> ticketNumber;

            auto it = activeTickets.find(ticketNumber);
            if (it == activeTickets.end())
            {
                std::cout << "Invalid ticket number.\n";
            }
            else
            {
                Vehicle *vehicle = new Vehicle(it->second.getLicensePlate(), VehicleType::Car); // You can set the correct type here
                parkingLot.parkVehicle(it->second, vehicle);
                std::cout << "Vehicle parked successfully.\n";
            }
            break;
        }
        case 4:
        {
            // Exit a vehicle and calculate fee
            int ticketNumber;
            std::cout << "Enter the ticket number to exit the vehicle: ";
            std::cin >> ticketNumber;

            auto it = activeTickets.find(ticketNumber);
            if (it == activeTickets.end())
            {
                std::cout << "Invalid ticket number.\n";
            }
            else
            {
                // Simulate vehicle exit
                // int parkedHours;
                // std::cout << "Enter the number of hours the vehicle has been parked: ";
                // std::cin >> parkedHours;

                // // Calculate the fee based on the number of hours
                ParkingTicket &ticket = it->second;
                // time_t entryTime = time(0) - parkedHours * 3600; // Subtract the hours (converted to seconds)
                // ticket.setEntryTime(entryTime);

                // Calculate and process payment
                double fee = parkingLot.calculateFee(ticket);
                std::cout << "Fee :" << fee << std::endl;
                // std::cout << "Fee for " << parkedHours << " hours: $" << fee << std::endl;
                if (PaymentProcessor::processPayment(fee))
                {
                    parkingLot.payTicket(ticket);
                    std::cout << "Payment processed successfully.\n";
                    activeTickets.erase(it); // Remove ticket after payment
                }
            }
            break;
        }
        case 5:
            // Exit program
            std::cout << "Exiting Parking Lot Management System. Goodbye!" << std::endl;
            return 0;

        case 6: // Search by License Plate
{
    std::string licensePlate;
    std::cout << "Enter the license plate to search: ";
    std::cin >> licensePlate;

    searchByLicensePlate(licensePlate, activeTickets);
    break;
}
        default:
            std::cout << "Invalid choice. Please try again.\n";
            break;
        }
    }

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

// Instantiation
//  A vehicle arrives at the parking lot and a ticket is issued.

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
// payTicket(ParkingTicket& ticket) marks the ticket as paid.
