#include <iostream>
#include <vector>
#include <map>
#include <ctime>
#include <sstream>
#include <stdexcept>
using namespace std;

// Enums
enum class VehicleType
{
    Motorcycle,
    Car,
    Truck
};

enum class ParkingSpotType
{
    Small,
    Medium,
    Large
};

enum class TicketStatus
{
    Active,
    Paid,
    Lost
};

// Vehicle Class
class Vehicle
{
private:
    string licensePlate;
    VehicleType type;

public:
    Vehicle(string licensePlate, VehicleType type) : licensePlate(licensePlate), type(type) {}

    string getLicensePlate() const { return licensePlate; }
    VehicleType getType() const { return type; }
};

// ParkingSpot Class
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
        {
            cout << "Error: Spot " << spotNumber << " is already occupied." << endl;
            return;
        }

        isOccupied = true;
        parkedVehicle = vehicle;
        cout << "Vehicle " << vehicle->getLicensePlate() << " parked at spot " << spotNumber << endl;
    }

    void removeVehicle()
    {
        if (!isOccupied || !parkedVehicle)
        {
            cout << "Error: No vehicle is parked in this spot." << endl;
            return;
        }

        isOccupied = false;
        parkedVehicle = nullptr;
        cout << "Vehicle removed from spot " << spotNumber << endl;
    }
};

// ParkingTicket Class
class ParkingTicket
{
private:
    int ticketNumber;
    string licensePlate;
    time_t entryTime;
    time_t exitTime;
    TicketStatus status;

public:
    ParkingTicket() : ticketNumber(0), licensePlate(""), entryTime(0), exitTime(0), status(TicketStatus::Active) {}

    ParkingTicket(int ticketNumber, string licensePlate, time_t entryTime)
        : ticketNumber(ticketNumber), licensePlate(licensePlate), entryTime(entryTime), exitTime(0), status(TicketStatus::Active) {}

    int getTicketNumber() const { return ticketNumber; }
    string getLicensePlate() const { return licensePlate; }
    time_t getEntryTime() const { return entryTime; }
    time_t getExitTime() const { return exitTime; }
    TicketStatus getStatus() const { return status; }

    void markAsPaid()
    {
        status = TicketStatus::Paid;
        cout << "Ticket " << ticketNumber << " marked as paid." << endl;
    }

    void markAsLost()
    {
        status = TicketStatus::Lost;
        cout << "Ticket " << ticketNumber << " marked as lost." << endl;
    }

    void setExitTime(time_t exitTime)
    {
        this->exitTime = exitTime;
    }

    string formatTime(time_t time) const
    {
        struct tm *tm_info;
        char buffer[80];
        tm_info = localtime(&time);
        strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", tm_info);
        return string(buffer);
    }

    string getParkedDuration() const
    {
        if (entryTime == 0)
        {
            return "Error: Entry time is not set.";
        }
        time_t now = time(0);
        double duration = difftime(now, entryTime);
        int hours = static_cast<int>(duration / 3600);
        int minutes = static_cast<int>((duration - (hours * 3600)) / 60);
        stringstream durationStream;
        durationStream << hours << " hours " << minutes << " minutes";
        return durationStream.str();
    }
};

// ParkingLot Class
class ParkingLot
{
private:
    vector<ParkingSpot> spots;
    map<int, ParkingTicket> activeTickets;
    unsigned long long nextTicketNumber;

    ParkingSpotType getSpotTypeForVehicle(VehicleType type)
    {
        switch (type)
        {
        case VehicleType::Motorcycle:
            return ParkingSpotType::Small;
        case VehicleType::Car:
            return ParkingSpotType::Medium;
        case VehicleType::Truck:
            return ParkingSpotType::Large;
        default:
            throw invalid_argument("Unknown vehicle type.");
        }
    }

public:
    ParkingLot(int numSmallSpots, int numMediumSpots, int numLargeSpots)
        : nextTicketNumber(1)
    {
        for (int i = 0; i < numSmallSpots; i++)
            spots.push_back(ParkingSpot(i + 1, ParkingSpotType::Small));
        for (int i = 0; i < numMediumSpots; i++)
            spots.push_back(ParkingSpot(i + 1 + numSmallSpots, ParkingSpotType::Medium));
        for (int i = 0; i < numLargeSpots; i++)
            spots.push_back(ParkingSpot(i + 1 + numSmallSpots + numMediumSpots, ParkingSpotType::Large));
    }

    map<int, ParkingTicket> &getActiveTickets()
    {
        return activeTickets;
    }

    ParkingTicket issueTicket(Vehicle *vehicle)
    {
        for (const auto &entry : activeTickets)
        {
            if (entry.second.getLicensePlate() == vehicle->getLicensePlate() && entry.second.getStatus() == TicketStatus::Active)
            {
                cout << "Error: Vehicle with license plate " << vehicle->getLicensePlate() << " already has an active ticket." << endl;
                throw runtime_error("Duplicate ticket error");
            }
        }

        ParkingTicket ticket(nextTicketNumber++, vehicle->getLicensePlate(), time(0));
        activeTickets[ticket.getTicketNumber()] = ticket;
        cout << "Ticket " << ticket.getTicketNumber() << " issued for vehicle " << vehicle->getLicensePlate() << endl;
        return ticket;
    }

    void exitVehicle(ParkingTicket &ticket)
    {
        if (ticket.getStatus() != TicketStatus::Paid)
        {
            cout << "Error: Payment is required before exiting. Please pay the fee." << endl;
            return;
        }

        for (auto &spot : spots)
        {
            if (spot.getIsOccupied())
            {
                spot.removeVehicle();
                ticket.setExitTime(time(0)); // Store exit time
                cout << "Vehicle " << ticket.getLicensePlate() << " exited.\n";
                cout << "Start Time: " << ticket.formatTime(ticket.getEntryTime()) << endl;
                cout << "End Time: " << ticket.formatTime(ticket.getExitTime()) << endl;
                return;
            }
        }
        cout << "Error: No matching spot found for exiting vehicle." << endl;
    }

    void showTickets()
    {
        for (const auto &ticket : activeTickets)
        {
            cout << "Ticket Number: " << ticket.second.getTicketNumber()
                 << ", Vehicle License Plate: " << ticket.second.getLicensePlate()
                 << ", Status: " << (ticket.second.getStatus() == TicketStatus::Paid ? "Paid" : "Active")
                 << ", Parked Duration: " << ticket.second.getParkedDuration() << endl;
        }
    }

    void parkVehicle(ParkingTicket &ticket, Vehicle *vehicle)
    {
        ParkingSpot *spot = findAvailableSpot(vehicle->getType());
        if (!spot)
        {
            cout << "No available spot for vehicle " << vehicle->getLicensePlate() << endl;
            return;
        }
        spot->parkVehicle(vehicle);
    }

    double calculateFee(ParkingTicket &ticket) const
    {
        if (ticket.getEntryTime() == 0)
        {
            cout << "Error: Entry time is not set for ticket " << ticket.getTicketNumber() << "." << endl;
            return 0.0;
        }
        time_t now = time(0);
        double duration = difftime(now, ticket.getEntryTime());
        double hours = duration / 3600;
        double fee = hours * 10; // Example: $10 per hour for all vehicle types
        cout << "Fee for ticket " << ticket.getTicketNumber() << ": " << fee << " USD" << endl;
        return fee;
    }

    void payTicket(ParkingTicket &ticket)
    {
        ticket.markAsPaid();
    }

    ParkingSpot *findAvailableSpot(VehicleType type)
    {
        ParkingSpotType requiredSpotType = getSpotTypeForVehicle(type);
        for (auto &spot : spots)
        {
            if (!spot.getIsOccupied() && spot.getType() == requiredSpotType)
            {
                return &spot;
            }
        }
        return nullptr;
    }

    void showAvailableSpots()
    {
        for (const auto &spot : spots)
        {
            if (!spot.getIsOccupied())
            {
                cout << "Spot " << spot.getSpotNumber() << " is available." << endl;
            }
        }
    }
};

// PaymentProcessor Class
class PaymentProcessor
{
public:
    static bool processPayment(double amount)
    {
        cout << "Processing payment of " << amount << " USD." << endl;
        return true; // Assume payment is always successful
    }
};

// Main Function
int main()
{
    ParkingLot parkingLot(10, 10, 5);

    int choice;
    while (true)
    {
        cout << "\nMenu:\n";
        cout << "1. Park a vehicle\n";
        cout << "2. View tickets\n";
        cout << "3. Exit a vehicle\n";
        cout << "4. Exit the system\n";
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1)
        {
            string licensePlate;
            int vehicleType;
            cout << "Enter license plate number: ";
            cin >> licensePlate;
            cout << "Enter vehicle type (0: Motorcycle, 1: Car, 2: Truck): ";
            cin >> vehicleType;

            if (vehicleType < 0 || vehicleType > 2)
            {
                cout << "Invalid vehicle type. Please enter 0 (Motorcycle), 1 (Car), or 2 (Truck)." << endl;
                continue;
            }

            Vehicle vehicle(licensePlate, (VehicleType)vehicleType);
            try
            {
                ParkingTicket ticket = parkingLot.issueTicket(&vehicle);
                parkingLot.parkVehicle(ticket, &vehicle);
            }
            catch (const exception &e)
            {
                cout << e.what() << endl;
            }
        }
        else if (choice == 2)
        {
            parkingLot.showTickets();
        }
        else if (choice == 3)
        {
            int ticketNumber;
            cout << "Enter ticket number: ";
            cin >> ticketNumber;

            if (parkingLot.getActiveTickets().find(ticketNumber) == parkingLot.getActiveTickets().end())
            {
                cout << "Invalid ticket number. Please try again." << endl;
                continue;
            }

            ParkingTicket &ticket = parkingLot.getActiveTickets()[ticketNumber];

            if (ticket.getStatus() == TicketStatus::Paid)
            {
                parkingLot.exitVehicle(ticket);
            }
            else
            {
                double fee = parkingLot.calculateFee(ticket);
                if (PaymentProcessor::processPayment(fee))
                {
                    parkingLot.payTicket(ticket);
                    parkingLot.exitVehicle(ticket);
                }
                else
                {
                    cout << "Payment failed. Please try again." << endl;
                }
            }
        }
        else if (choice == 4)
        {
            cout << "Exiting the system. Thank you!" << endl;
            break;
        }
        else
        {
            cout << "Invalid choice. Please try again." << endl;
        }
    }

    return 0;
}
