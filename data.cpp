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
                            "name TEXT, EmployeePin TEXT UNIQUE, StoreID TEXT UNIQUE);";

    string ScheduleTable = "CREATE TABLE IF NOT EXISTS schedule ("
                            "ScheduleID INTEGER PRIMARY KEY AUTOINCREMENT,"
                            "EmployeePin TEXT, Date INTEGER, start_time INTEGER, end_time INTEGER);";

    

    int result = sqlite3_exec(db, EmployeeTable.c_str(), NULL, 0, &errorMessage);
    int result2 = sqlite3_exec(db, ScheduleTable.c_str(), NULL, 0, &errorMessage);

    if (result != SQLITE_OK) {
        cerr << "Employee Table Error: " << errorMessage << endl;
        sqlite3_free(errorMessage);
    } else {
    }

    if (result2 != SQLITE_OK) {
        cerr << "Employee Table Error: " << errorMessage << endl;
        sqlite3_free(errorMessage);
    } else {
    }

    Employee Daniel("Daniel", "1", "0602");
    addEmployee(db, Daniel);

    addSchedule(db, "0602", 1024, 100, 500);
    // 4. Close the connection when done
    sqlite3_close(db);

    return 0;
}


