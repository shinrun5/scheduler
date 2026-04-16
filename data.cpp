#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <iostream>
#include <vector>
#include <string>

using namespace std;


struct hours {
    int Day;
    int StartTime;
    int EndTime;
};

class Employee {
public: 
    string name;
    string EmployeeID;
    string StoreID;
    vector<hours> Hours;
    int maxHours;
    int proficiency;
    int currentHours;
    string pin;

    Employee(string NAME, string STORE, string PIN, int ID = -1)
    {
        name = NAME;
        StoreID = STORE;
        pin = PIN;
        EmployeeID = ID;

        maxHours = 0;
        currentHours = 0;
        proficiency = 0;
    }
};

void addEmployee(sqlite3* db, Employee &emp) {
    char* errorMessage = nullptr;

    // 1. Build the SQL "Sentence"
    // Note: name needs ' ' around it because it's text!
    string sql = "INSERT INTO employees (name, EmployeePin, StoreID) VALUES ('" 
                 + emp.name + "', '" 
                 + emp.pin + "', '" 
                 + emp.StoreID + "');";

    // 2. "Speak" the SQL to the database
    int result = sqlite3_exec(db, sql.c_str(), NULL, 0, &errorMessage);

    // 3. Check if it worked
    if (result != SQLITE_OK) {
        cerr << "Error adding employee: " << errorMessage << endl;
        sqlite3_free(errorMessage);
    } else {
        cout << emp.name << " was added to the database!" << endl;
    }
}

//Takes values passed from loadStaff and create the employee object using the data so any computation can be done much faster
int employeeCallback(void* data, int argc, char** argv, char** azColName)
{
    vector<Employee>* staff = (vector<Employee>*)data;
    if (argv[1] && argv[3] && argv[2]) {
        staff->push_back(Employee(argv[1], argv[3], argv[2]));
    }
    return 0;
}

//takes all data from the employee database and put it into allstaff which is then passed into employeeCallBack
void loadStaff(sqlite3* db, vector<Employee> &allStaff)
{
    char* errorMessage = nullptr;
    int result = sqlite3_exec(db, "SELECT * FROM employees;", employeeCallback, &allStaff, nullptr);
    if (result != SQLITE_OK) {
        cerr << "Error loading staff: " << errorMessage << endl;
        sqlite3_free(errorMessage);
    }
}

void addSchedule(sqlite3* db, string pin, int Day, int start, int end)
{
    char* errorMessage = nullptr;

    //add hours for specific days into the schedule
    string sql = "INSERT INTO schedule (EmployeePin, Date, start_time, end_time) VALUES ('"
                 + pin + "', "
                 + to_string(Day) + ", "
                 + to_string(start) + ", "
                 + to_string(end) + ");";

    // 2. "Speak" the SQL to the database
    int result = sqlite3_exec(db, sql.c_str(), NULL, 0, &errorMessage);

    // 3. Check if it worked
    if (result != SQLITE_OK) {
        cerr << "Error adding Schedule: " << errorMessage << endl;
        sqlite3_free(errorMessage);
    } else {
        cout << "Schedule added for PIN: " << pin << endl;
    }
}

int scheduleCallBack(void* data, int argc, char** argv, char** azColName)
{
    vector<Employee>* staff = (vector<Employee>*)data;
    
    string pinFromDB = argv[1];
    for (auto& emp : *staff) 
    { 
    
        if (emp.pin == pinFromDB) 
        { 
            hours newShift = { stoi(argv[2]), stoi(argv[3]), stoi(argv[4]) };
            emp.Hours.push_back(newShift); 
        
            break; 
        }
    }
    return 0;

}

void loadSchedule(sqlite3* db, vector<Employee> &allStaff)
{
    char* errorMessage = nullptr;

    int result = sqlite3_exec(db, "SELECT * FROM schedule", scheduleCallBack, &allStaff, nullptr);
    if (result != SQLITE_OK) {
        cerr << "Error loading schedule: " << errorMessage << endl;
        sqlite3_free(errorMessage);
    }
}



int main() {
    sqlite3* db;
    char* errorMessage = nullptr;

    // 1. Open the Database
    if (sqlite3_open("schedule.db", &db) != SQLITE_OK) {
        cerr << "Could not open database." << endl;
        return -1;
    }

    // 2. Define your SQL table blueprint
    string EmployeeTable = "CREATE TABLE IF NOT EXISTS employees ("
                            "EmployeeID INTEGER PRIMARY KEY AUTOINCREMENT,"
                            "name TEXT, EmployeePin TEXT UNIQUE, StoreID TEXT);";

    string ScheduleTable = "CREATE TABLE IF NOT EXISTS schedule ("
                            "ScheduleID INTEGER PRIMARY KEY AUTOINCREMENT,"
                            "EmployeePin TEXT, Date INTEGER, start_time INTEGER, end_time INTEGER);";

    

    int result = sqlite3_exec(db, EmployeeTable.c_str(), NULL, 0, &errorMessage);
    int result2 = sqlite3_exec(db, ScheduleTable.c_str(), NULL, 0, &errorMessage);

    if (result != SQLITE_OK) 
    {
        cerr << "Employee Table Error: " << errorMessage << endl;
        sqlite3_free(errorMessage);
    } else {
    }

    if (result2 != SQLITE_OK) 
    {
        cerr << "Employee Table Error: " << errorMessage << endl;
        sqlite3_free(errorMessage);
    } else 
    {
    }

    // --- TEST 1: ADD DATA ---
    cout << "--- Testing: Adding Employees ---" << endl;
    Employee dan("Daniel", "Mango Mango", "0602");
    Employee dan2("Danny", "Ciao Poke", "0000");
    Employee testUser("Alex", "Ciao Poke", "1234");
    
    addEmployee(db, dan);
    addEmployee(db, dan2);
    addEmployee(db, testUser);


    /// --- TEST 2: ADD SCHEDULE ---
    addSchedule(db, "0602", 1024, 690, 1410);
    addSchedule(db, "0602", 1025, 690, 1060);
    addSchedule(db, "1234", 1025, 850, 1410);

    // --- TEST 3: LOAD DATA ---
    cout << "--- Testing: Loading Data from DB to C++ ---" << endl;
    vector<Employee> currentStaff;
    loadStaff(db, currentStaff);
    loadSchedule(db, currentStaff);

    // --- TEST 4: VERIFY ---
    cout << "Found " << currentStaff.size() << " employees in database:" << endl;
    for (const auto& emp : currentStaff) 
    {
        cout << " - Name: " << emp.name << " | Store: " << emp.StoreID << " | PIN: " << emp.pin << endl;
        if (emp.Hours.empty())
        {
            cout << "NO SHIFT ASSIGNED" << endl;
        }
        else
        {
            for(const auto& shift: emp.Hours)
            {
                cout << "  - Date: " << shift.Day 
                     << " | " << shift.StartTime / 60 << ":" << (shift.StartTime % 60 < 10 ? "0" : "") << shift.StartTime % 60 
                     << " to " << shift.EndTime / 60 << ":" << (shift.EndTime % 60 < 10 ? "0" : "") << shift.EndTime % 60 << endl;
            }
        }
    } 


    sqlite3_close(db);


    return 0;
}


