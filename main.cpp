#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <limits>

using namespace std;

class Car {
private:
    string licensePlate;
    string make;
    string model;
    int year;
    bool isRented;
    double rentalPrice;

public:
    Car(string lp, string mk, string mdl, int yr, double price)
        : licensePlate(lp), make(mk), model(mdl), year(yr), isRented(false), rentalPrice(price) {}

    void saveToCSV(ofstream& out) const {
        out << licensePlate << "," << make << "," << model << "," << year << "," << isRented << "," << rentalPrice << endl;
    }

    static Car loadFromCSV(const string& line) {
        string lp, mk, mdl;
        int yr;
        bool rented;
        double price;

        size_t pos = 0;
        size_t start = 0;

        // Load license plate
        pos = line.find(',', start);
        lp = line.substr(start, pos - start);
        start = pos + 1;

        // Load make
        pos = line.find(',', start);
        mk = line.substr(start, pos - start);
        start = pos + 1;

        // Load model
        pos = line.find(',', start);
        mdl = line.substr(start, pos - start);
        start = pos + 1;

        // Load year
        pos = line.find(',', start);
        yr = stoi(line.substr(start, pos - start));
        start = pos + 1;

        // Load rented status
        pos = line.find(',', start);
        rented = line.substr(start, pos - start) == "1";
        start = pos + 1;

        // Load rental price
        try {
            price = stod(line.substr(start));
        } catch (const invalid_argument&) {
            throw runtime_error("Invalid CSV format: rental price is not a number");
        }

        Car car(lp, mk, mdl, yr, price);
        car.isRented = rented; // Set the rented status
        return car;
    }

    string getLicensePlate() const { return licensePlate; }
    string getMake() const { return make; }
    string getModel() const { return model; }
    int getYear() const { return year; }
    bool getIsRented() const { return isRented; }
    double getRentalPrice() const { return rentalPrice; }
    void rent() { isRented = true; }
    void returnCar() { isRented = false; }
};

class Customer {
private:
    string name;
    string contactNumber;

public:
    Customer(string n, string cn) : name(n), contactNumber(cn) {}

    void saveToCSV(ofstream& out) const {
        out << name << "," << contactNumber << endl;
    }

    static Customer loadFromCSV(const string& line) {
        size_t pos = line.find(',');
        string n = line.substr(0, pos);
        string cn = line.substr(pos + 1);
        return Customer(n, cn);
    }

    string getName() const { return name; }
    string getContactNumber() const { return contactNumber; }
};

class RentalSystem {
private:
    vector<Car> cars;
    vector<Customer> customers;

    void loadCars() {
        ifstream inFile("cars.csv");
        if (!inFile) {
            cerr << "Error opening cars.csv. File may not exist." << endl;
            return;
        }

        string line;
        while (getline(inFile, line)) {
            if (!line.empty()) {
                cars.push_back(Car::loadFromCSV(line));
            }
        }
    }

    void loadCustomers() {
        ifstream inFile("customers.csv");
        if (!inFile) {
            cerr << "Error opening customers.csv. File may not exist." << endl;
            return;
        }

        string line;
        while (getline(inFile, line)) {
            if (!line.empty()) {
                customers.push_back(Customer::loadFromCSV(line));
            }
        }
    }

public:
    RentalSystem() {
        loadCars();
        loadCustomers();
    }

    void addCar(const Car& car) {
        cars.push_back(car);
        ofstream outFile("cars.csv", ios::app);
        if (!outFile) {
            cerr << "Error opening cars.csv for writing." << endl;
            return;
        }
        car.saveToCSV(outFile);
        cout << "Car added: " << car.getLicensePlate() << endl;
    }

    void addCustomer(const Customer& customer) {
        customers.push_back(customer);
        ofstream outFile("customers.csv", ios::app);
        if (!outFile) {
            cerr << "Error opening customers.csv for writing." << endl;
            return;
        }
        customer.saveToCSV(outFile);
        cout << "Customer added: " << customer.getName() << endl;
    }

    void rentCar(const string& licensePlate) {
        for (auto& car : cars) {
            if (car.getLicensePlate() == licensePlate && !car.getIsRented()) {
                car.rent();
                cout << "Car rented: " << licensePlate << " at price: $" << car.getRentalPrice() << endl;
                return;
            }
        }
        cout << "Car not available for rent." << endl;
    }

    void returnCar(const string& licensePlate) {
        for (auto& car : cars) {
            if (car.getLicensePlate() == licensePlate && car.getIsRented()) {
                car.returnCar();
                cout << "Car returned: " << licensePlate << endl;
                return;
            }
        }
        cout << "Rental record not found." << endl;
    }

    void displayCars() const {
        cout << "Available Cars: " << endl;
        for (const auto& car : cars) {
            if (!car.getIsRented()) {
                cout << car.getLicensePlate() << " - "
                     << car.getMake() << " "
                     << car.getModel() << " ("
                     << car.getYear() << ") - Price: $"
                     << car.getRentalPrice() << endl;
            }
        }
    }

    void modifyCarDetails(const string& licensePlate) {
        for (auto& car : cars) {
            if (car.getLicensePlate() == licensePlate) {
                string make, model;
                int year;
                double price;

                cout << "Enter new make: ";
                cin >> make;
                cout << "Enter new model: ";
                cin >> model;
                cout << "Enter new year: ";
                cin >> year;

                while (true) {
                    cout << "Enter new rental price: ";
                    cin >> price;
                    if (cin.fail()) {
                        cin.clear(); // Clear the error flag
                        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
                        cout << "Invalid price. Please enter a valid number." << endl;
                    } else {
                        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard the rest of the line
                        break; // Valid input, exit loop
                    }
                }

                // Update car details
                car = Car(licensePlate, make, model, year, price);
                cout << "Car details updated." << endl;
                return;
            }
        }
        cout << "Car not found." << endl;
    }

    void calculatePrice(const string& licensePlate, int rentalDays) {
        for (const auto& car : cars) {
            if (car.getLicensePlate() == licensePlate) {
                double totalPrice = car.getRentalPrice() * rentalDays;
                cout << "Total rental price for " << rentalDays << " days: $" << totalPrice << endl;
                return;
            }
        }
        cout << "Car not found." << endl;
    }
};

int main() {
    RentalSystem rentalSystem;
    int choice;

    while (true) {
        cout << "\nRental System Menu:" << endl;
        cout << "1. Add Car" << endl;
        cout << "2. Add Customer" << endl;
        cout << "3. Modify Car Details" << endl;
        cout << "4. Rent Car" << endl;
        cout << "5. Return Car" << endl;
        cout << "6. Display Available Cars" << endl;
        cout << "7. Calculate Rental Price" << endl;
        cout << "8. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                string licensePlate, make, model;
                int year;
                double price;

                cout << "Enter License Plate: ";
                cin >> licensePlate;
                cout << "Enter Make: ";
                cin >> make;
                cout << "Enter Model: ";
                cin >> model;
                cout << "Enter Year: ";
                cin >> year;

                while (true) {
                    cout << "Enter Rental Price: ";
                    cin >> price;
                    if (cin.fail()) {
                        cin.clear(); // Clear the error flag
                        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
                        cout << "Invalid price. Please enter a valid number." << endl;
                    } else {
                        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard the rest of the line
                        break; // Valid input, exit loop
                    }
                }

                rentalSystem.addCar(Car(licensePlate, make, model, year, price));
                break;
            }
            case 2: {
                string name, contactNumber;
                cout << "Enter Name: ";
                cin.ignore(); // Clear newline
                getline(cin, name);
                cout << "Enter Contact Number: ";
                cin >> contactNumber;

                rentalSystem.addCustomer(Customer(name, contactNumber));
                break;
            }
            case 3: {
                string licensePlate;
                cout << "Enter License Plate of car to modify: ";
                cin >> licensePlate;

                rentalSystem.modifyCarDetails(licensePlate);
                break;
            }
            case 4: {
                string licensePlate;
                cout << "Enter License Plate to rent: ";
                cin >> licensePlate;

                rentalSystem.rentCar(licensePlate);
                break;
            }
            case 5: {
                string licensePlate;
                cout << "Enter License Plate to return: ";
                cin >> licensePlate;

                rentalSystem.returnCar(licensePlate);
                break;
            }
            case 6: {
                rentalSystem.displayCars();
                break;
            }
            case 7: {
                string licensePlate;
                int rentalDays;
                cout << "Enter License Plate for price calculation: ";
                cin >> licensePlate;
                cout << "Enter number of rental days: ";
                cin >> rentalDays;

                rentalSystem.calculatePrice(licensePlate, rentalDays);
                break;
            }
            case 8:
                cout << "Exiting the system." << endl;
                return 0;
            default:
                cout << "Invalid choice, please try again." << endl;
        }
    }

    return 0;
}
