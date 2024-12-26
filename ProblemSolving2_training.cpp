#include <vector>
#include <iostream>
#include <map>
using namespace std;
#include <ctime>

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
    string licensePlate;
    VehicleType type;

public:
    Vehicle(string licensePlate, VehicleType type) 
        : licensePlate(licensePlate), type(type) {}

    string getLicensePlate() const {return licensePlate;}
    VehicleType getType() const{return type;}
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

    int getSpotNumber() const{return spotNumber;}
    ParkingSpotType getType() const{return type;}
    bool getIsOccupied() const{
        return isOccupied;
    }
    void parkVehicle(Vehicle* vehicle){
        if(isOccupied){
            parkedVehicle=nullptr;
            isOccupied=true;
        }
    }
    void removeVehicle(){
        if(isOccupied){
            parkedVehicle=nullptr;
            isOccupied=false;
        }
    }
    Vehicle* getVehicle() const {
        return parkedVehicle;
    }
};
//ParkingTicket: Represents a parking ticket issued to a vehicle.
class ParkingTicket {
private:
    int ticketNumber;
    string licensePlate;
    time_t entryTime;
    bool isPaid;

public:
    ParkingTicket() : ticketNumber(0), licensePlate(""), entryTime(0) {}
    
    ParkingTicket(int ticketNumber, string licensePlate, time_t entryTime)
        : ticketNumber(ticketNumber), licensePlate(licensePlate), entryTime(entryTime) {}

    int getTicketNumber() const { return ticketNumber; }
    string getLicensePlate() const { return licensePlate; }
    time_t getEntryTime() const { return entryTime; }
    void markAsPaid() {isPaid=true;}
};

//ParkingLot: Manages the overall parking lot.
class ParkingLot {
private:
    vector<ParkingSpot> spots;
    map<int, ParkingTicket> activeTickets;
    int nextTicketNumber;

    ParkingSpotType mapVehicalToSpot(VehicleType type){
        if(type == VehicleType::Motorcycle)return ParkingSpotType::Small;
        if(type == VehicleType::Car)return ParkingSpotType::Medium;
        return ParkingSpotType::Large;
    }

public:
    ParkingLot(int numSmallSpots, int numMediumSpots, int numLargeSpots):nextTicketNumber(1){
        for(int i=0;i<numSmallSpots;i++){
            spots.emplace_back(spots.size(),ParkingSpotType::Small);
        }
        for(int i=0;i<numMediumSpots;i++){
            spots.emplace_back(spots.size(),ParkingSpotType::Medium);
        }
        for(int i=0;i<numLargeSpots;i++){
            spots.emplace_back(spots.size(),ParkingSpotType::Large);
        }
    }
    ParkingTicket issueTicket(Vehicle* vehicle){
        time_t entryTime=time(nullptr);
        ParkingTicket ticket(nextTicketNumber++,vehicle->getLicensePlate(),entryTime);
        activeTickets[ticket.getTicketNumber()]=ticket;
        return ticket;
    }
    void parkVehicle(ParkingTicket& ticket, Vehicle* vehicle){
        ParkingSpot* spot=findAvailableSpot(vehicle->getType());
        if(spot){
            spot->parkVehicle(vehicle);
            cout << "Vehicle parked at spot " << spot->getSpotNumber() << endl;}
        else { 
            cout<<"insuficient!"<<endl;
        }
    }
    void exitVehicle(ParkingTicket& ticket) {
    for (auto& spot : spots) {
        if (spot.getIsOccupied() && spot.getVehicle() != nullptr && spot.getVehicle()->getLicensePlate() == ticket.getLicensePlate()) {
            spot.removeVehicle();
            cout << "Vehicle with license plate " << ticket.getLicensePlate()
                 << " exited from spot " << spot.getSpotNumber() << endl;
            return;
        }
    }
    cout << "Vehicle with license plate " << ticket.getLicensePlate() << " not found in any spot!" << endl;
}
    double calculateFee(ParkingTicket& ticket) const{
        time_t currentTime=time(nullptr);
        double hoursParked = difftime(currentTime, ticket.getEntryTime()) / 3600.0;
        return hoursParked *10.0;

    }
    void payTicket(ParkingTicket& ticket){
        ticket.markAsPaid();
        activeTickets.erase(ticket.getTicketNumber());
        cout << "Ticket paid for vehicle with license plate: " << ticket.getLicensePlate() << endl;
    }
    ParkingSpot* findAvailableSpot(VehicleType type){
        ParkingSpotType requiredSpotType=mapVehicalToSpot(type);
        for(auto& spot:spots){
            if(!spot.getIsOccupied()&& spot.getType()==requiredSpotType){
                return &spot;
            }
        }
        return nullptr;
    }
};

//PaymentProcessor: Handles payment for parking tickets.
class PaymentProcessor {
public:
    static bool processPayment(double amount){
        cout << "Processing payment of Rupees : "<<amount << endl;
        return true;
    }
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
    if(PaymentProcessor::processPayment(fee)){
    parkingLot.payTicket(ticket);}

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
