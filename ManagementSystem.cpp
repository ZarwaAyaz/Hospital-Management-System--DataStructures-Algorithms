#include <iostream>
#include <ctime>
#include <cstring>
#include <cctype>
#include <limits>
#include <fstream>
#include <sstream>
using namespace std;

// Constants
const int MAX_PATIENTS = 1000;
const int MAX_DOCTORS = 100;
const int MAX_APPOINTMENTS = 1000;
const int MAX_MEDICINES = 500;
const int MAX_ROOMS = 200;
const int HASH_TABLE_SIZE = 128;

// File names
const char* PATIENTS_FILE = "patients.txt";
const char* DOCTORS_FILE = "doctors.txt";
const char* MEDICINES_FILE = "medicines.txt";
const char* APPOINTMENTS_FILE = "appointments.txt";
const char* ROOMS_FILE = "rooms.txt";
const char* EMERGENCY_FILE = "emergency.txt";

// Struct Definitions
struct Date {
    int day, month, year;
};

struct Time {
    int hour, minute;
};

// BST Node for Patients
struct PatientBSTNode {
    PatientBSTNode* left;
    PatientBSTNode* right;
    int id; // Reference to the actual patient data
};

// BST Node for Doctors
struct DoctorBSTNode {
    DoctorBSTNode* left;
    DoctorBSTNode* right;
    int id; // Reference to the actual doctor data
};

// BST Node for Medicines
struct MedicineBSTNode {
    MedicineBSTNode* left;
    MedicineBSTNode* right;
    int id; // Reference to the actual medicine data
};

// Patient node
struct PatientNode {
    int id;
    char name[50];
    int age;
    char gender;
    char contact[15];
    char address[100];
    char medicalHistory[500];
    int doctorId;
    int roomNumber;
    bool isAdmitted;
    bool isEmergency;
    PatientNode* next;
};

// Doctor node
struct DoctorNode {
    int id;
    char name[50];
    char department[50];
    char specialty[50];
    char contact[15];
    bool availability[7][24]; // 7 days, 24 hours
    DoctorNode* next;
};

// Medicine node
struct MedicineNode {
    int id;
    char name[50];
    char code[20];
    int quantity;
    Date expiryDate;
    float price;
    MedicineNode* next;
};

// Appointment node
struct AppointmentNode {
    int id;
    int patientId;
    int doctorId;
    Date date;
    Time time;
    char purpose[100];
    int status; // 0 - Scheduled, 1 - Completed, 2 - Cancelled, 3 - No-show
    AppointmentNode* next;
};

// Room structure
struct Room {
    int number;
    bool isOccupied;
    int patientId;
};

// Priority queue node for emergency cases
struct EmergencyNode {
    int patientId;
    int priority; // 1 = highest priority
    EmergencyNode* next;
};

// Hash table node for quick lookups
struct HashNode {
    int id;
    void* data; // Can point to any data type
    HashNode* next;
};

// Global Data Structures
PatientNode* patientList = nullptr;
PatientBSTNode* patientBST = nullptr;
int patientCount = 0;

DoctorNode* doctorList = nullptr;
DoctorBSTNode* doctorBST = nullptr;
int doctorCount = 0;

AppointmentNode* appointmentList = nullptr;
int appointmentCount = 0;

MedicineNode* medicineList = nullptr;
MedicineBSTNode* medicineBST = nullptr;
int medicineCount = 0;

Room rooms[MAX_ROOMS];
int roomCount = 0;

EmergencyNode* emergencyQueue = nullptr;

HashNode* patientHashTable[HASH_TABLE_SIZE] = {nullptr};
HashNode* doctorHashTable[HASH_TABLE_SIZE] = {nullptr};
HashNode* medicineHashTable[HASH_TABLE_SIZE] = {nullptr};

// Helper Functions
int generatePatientId() {
    int id = 1;
    ifstream file(PATIENTS_FILE);
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string temp;
        getline(ss, temp, ',');
        int existingId = stoi(temp);
        if (existingId >= id) {
            id = existingId + 1;
        }
    }
    file.close();
    return id;
}

int generateDoctorId() {
    int id = 1;
    ifstream file(DOCTORS_FILE);
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string temp;
        getline(ss, temp, ',');
        int existingId = stoi(temp);
        if (existingId >= id) {
            id = existingId + 1;
        }
    }
    file.close();
    return id;
}

int generateAppointmentId() {
    int id = 1;
    ifstream file(APPOINTMENTS_FILE);
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string temp;
        getline(ss, temp, ',');
        int existingId = stoi(temp);
        if (existingId >= id) {
            id = existingId + 1;
        }
    }
    file.close();
    return id;
}

int generateMedicineId() {
    int id = 1;
    ifstream file(MEDICINES_FILE);
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string temp;
        getline(ss, temp, ',');
        int existingId = stoi(temp);
        if (existingId >= id) {
            id = existingId + 1;
        }
    }
    file.close();
    return id;
}

// Hash function
int hashFunction(int id) {
    return id % HASH_TABLE_SIZE;
}

// Add to hash table
void addToHashTable(HashNode* hashTable[], int id, void* data) {
    int index = hashFunction(id);
    HashNode* newNode = new HashNode;
    newNode->id = id;
    newNode->data = data;
    newNode->next = hashTable[index];
    hashTable[index] = newNode;
}

// Get from hash table
void* getFromHashTable(HashNode* hashTable[], int id) {
    int index = hashFunction(id);
    HashNode* current = hashTable[index];
    while (current != nullptr) {
        if (current->id == id) {
            return current->data;
        }
        current = current->next;
    }
    return nullptr;
}

// Remove from hash table
void removeFromHashTable(HashNode* hashTable[], int id) {
    int index = hashFunction(id);
    HashNode* current = hashTable[index];
    HashNode* prev = nullptr;

    while (current != nullptr) {
        if (current->id == id) {
            if (prev == nullptr) {
                hashTable[index] = current->next;
            } else {
                prev->next = current->next;
            }
            delete current;
            return;
        }
        prev = current;
        current = current->next;
    }
}

// BST Insertion for Patients
PatientBSTNode* insertPatientBST(PatientBSTNode* root, const char* name, int id) {
    if (root == nullptr) {
        PatientBSTNode* newNode = new PatientBSTNode;
        newNode->left = newNode->right = nullptr;
        newNode->id = id;
        return newNode;
    }

    PatientNode* currentPatient = static_cast<PatientNode*>(getFromHashTable(patientHashTable, root->id));

    if (strcmp(name, currentPatient->name) < 0) {
        root->left = insertPatientBST(root->left, name, id);
    } else {
        root->right = insertPatientBST(root->right, name, id);
    }
    return root;
}

// BST Search for Patients
void searchPatientBST(PatientBSTNode* root, const char* name, PatientNode* results[], int& count) {
    if (root == nullptr || count >= MAX_PATIENTS) return;

    PatientNode* currentPatient = static_cast<PatientNode*>(getFromHashTable(patientHashTable, root->id));

    // Search left subtree first (alphabetical order)
    searchPatientBST(root->left, name, results, count);

    // Check current node
    if (strstr(currentPatient->name, name) != nullptr) {
        results[count++] = currentPatient;
    }

    // Search right subtree
    searchPatientBST(root->right, name, results, count);
}

// BST Insertion for Doctors
DoctorBSTNode* insertDoctorBST(DoctorBSTNode* root, const char* name, int id) {
    if (root == nullptr) {
        DoctorBSTNode* newNode = new DoctorBSTNode;
        newNode->left = newNode->right = nullptr;
        newNode->id = id;
        return newNode;
    }

    DoctorNode* currentDoctor = static_cast<DoctorNode*>(getFromHashTable(doctorHashTable, root->id));

    if (strcmp(name, currentDoctor->name) < 0) {
        root->left = insertDoctorBST(root->left, name, id);
    } else {
        root->right = insertDoctorBST(root->right, name, id);
    }
    return root;
}

// BST Search for Doctors
void searchDoctorBST(DoctorBSTNode* root, const char* name, DoctorNode* results[], int& count) {
    if (root == nullptr || count >= MAX_DOCTORS) return;

    DoctorNode* currentDoctor = static_cast<DoctorNode*>(getFromHashTable(doctorHashTable, root->id));

    // Search left subtree first
    searchDoctorBST(root->left, name, results, count);

    // Check current node
    if (strstr(currentDoctor->name, name) != nullptr) {
        results[count++] = currentDoctor;
    }

    // Search right subtree
    searchDoctorBST(root->right, name, results, count);
}

// BST Insertion for Medicines
MedicineBSTNode* insertMedicineBST(MedicineBSTNode* root, const char* name, int id) {
    if (root == nullptr) {
        MedicineBSTNode* newNode = new MedicineBSTNode;
        newNode->left = newNode->right = nullptr;
        newNode->id = id;
        return newNode;
    }

    MedicineNode* currentMedicine = static_cast<MedicineNode*>(getFromHashTable(medicineHashTable, root->id));

    if (strcmp(name, currentMedicine->name) < 0) {
        root->left = insertMedicineBST(root->left, name, id);
    } else {
        root->right = insertMedicineBST(root->right, name, id);
    }
    return root;
}

// BST Search for Medicines
void searchMedicineBST(MedicineBSTNode* root, const char* name, MedicineNode* results[], int& count) {
    if (root == nullptr || count >= MAX_MEDICINES) return;

    MedicineNode* currentMedicine = static_cast<MedicineNode*>(getFromHashTable(medicineHashTable, root->id));

    // Search left subtree first
    searchMedicineBST(root->left, name, results, count);

    // Check current node
    if (strstr(currentMedicine->name, name) != nullptr) {
        results[count++] = currentMedicine;
    }

    // Search right subtree
    searchMedicineBST(root->right, name, results, count);
}

// Improved swap function for appointments
void swapAppointments(AppointmentNode* a, AppointmentNode* b) {
    AppointmentNode temp = *a;
    *a = *b;
    *b = temp;
    AppointmentNode* tempNext = a->next;
    a->next = b->next;
    b->next = tempNext;
}

// Date comparison functions
bool isDateBefore(Date d1, Date d2) {
    if (d1.year != d2.year) return d1.year < d2.year;
    if (d1.month != d2.month) return d1.month < d2.month;
    return d1.day < d2.day;
}

bool isTimeBefore(Time t1, Time t2) {
    if (t1.hour != t2.hour) return t1.hour < t2.hour;
    return t1.minute < t2.minute;
}

AppointmentNode* partition(AppointmentNode* low, AppointmentNode* high) {
    Date pivot = high->date;
    Time pivotTime = high->time;
    AppointmentNode* i = low;

    for (AppointmentNode* j = low; j != high; j = j->next) {
        if (isDateBefore(j->date, pivot)) {
            swapAppointments(i, j);
            i = i->next;
        }
        else if (j->date.day == pivot.day &&
                j->date.month == pivot.month &&
                j->date.year == pivot.year &&
                isTimeBefore(j->time, pivotTime)) {
            swapAppointments(i, j);
            i = i->next;
        }
    }
    swapAppointments(i, high);
    return i;
}

void quickSortAppointments(AppointmentNode* low, AppointmentNode* high) {
    if (low != nullptr && high != nullptr && low != high) {
        AppointmentNode* p = partition(low, high);
        AppointmentNode* temp = low;
        while (temp != nullptr && temp->next != p) {
            temp = temp->next;
        }
        quickSortAppointments(low, temp);
        quickSortAppointments(p->next, high);
    }
}

// Function to get the tail of the linked list
AppointmentNode* getTail(AppointmentNode* head) {
    while (head != nullptr && head->next != nullptr) {
        head = head->next;
    }
    return head;
}

// Priority queue operations for emergencies
void enqueueEmergency(int patientId, int priority) {
    EmergencyNode* newNode = new EmergencyNode;
    newNode->patientId = patientId;
    newNode->priority = priority;
    newNode->next = nullptr;

    if (emergencyQueue == nullptr || priority < emergencyQueue->priority) {
        newNode->next = emergencyQueue;
        emergencyQueue = newNode;
    } else {
        EmergencyNode* current = emergencyQueue;
        while (current->next != nullptr && current->next->priority <= priority) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }

    // Save to file
    ofstream file(EMERGENCY_FILE, ios::app);
    if (file.is_open()) {
        file << patientId << "," << priority << "\n";
        file.close();
    }
}

int dequeueEmergency() {
    if (emergencyQueue == nullptr) return -1;

    EmergencyNode* temp = emergencyQueue;
    int patientId = temp->patientId;
    emergencyQueue = emergencyQueue->next;
    delete temp;

    // Update emergency file
    ofstream file(EMERGENCY_FILE);
    EmergencyNode* current = emergencyQueue;
    while (current != nullptr) {
        file << current->patientId << "," << current->priority << "\n";
        current = current->next;
    }
    file.close();

    return patientId;
}

// Date and time validation functions
bool isDateValid(Date d) {
    if (d.year < 2023 || d.month < 1 || d.month > 12 || d.day < 1) return false;

    int maxDays;
    if (d.month == 2) {
        maxDays = (d.year % 4 == 0 && (d.year % 100 != 0 || d.year % 400 == 0)) ? 29 : 28;
    } else if (d.month == 4 || d.month == 6 || d.month == 9 || d.month == 11) {
        maxDays = 30;
    } else {
        maxDays = 31;
    }

    return d.day <= maxDays;
}

bool isTimeValid(Time t) {
    return t.hour >= 0 && t.hour < 24 && t.minute >= 0 && t.minute < 60;
}

// Validation helper functions
bool isValidName(const char* name) {
    for (int i = 0; name[i] != '\0'; i++) {
        if (!isalpha(name[i]) && name[i] != ' ') {
            return false;
        }
    }
    return true;
}

bool isValidAge(int age) {
    return age > 0 && age < 120;
}

bool isValidGender(char gender) {
    gender = toupper(gender);
    return gender == 'M' || gender == 'F' || gender == 'J';
}

bool isValidPhone(const char* contact) {
    if (strlen(contact) != 11) return false;
    for (int i = 0; contact[i] != '\0'; i++) {
        if (!isdigit(contact[i])) {
            return false;
        }
    }
    return true;
}

// Doctor specialization handling
void getDoctorSpecialization(char* specialty, char* department) {
    int choice;
    cout << "Select Specialization:\n";
    cout << "1. Cardiology (Heart Specialist)\n";
    cout << "2. Neurology (Brain and Nervous System)\n";
    cout << "3. Orthopedics (Bones and Joints)\n";
    cout << "4. Pediatrics (Child Specialist)\n";
    cout << "5. Gynecology (Women's Health)\n";
    cout << "6. Ophthalmology (Eye Specialist)\n";
    cout << "7. ENT (Ear, Nose and Throat)\n";
    cout << "8. Gastroenterology (Digestive System)\n";
    cout << "9. Dermatology (Skin Specialist)\n";
    cout << "10. Dentistry (Teeth and Oral Health)\n";
    cout << "Enter choice (1-10): ";
    cin >> choice;

    switch(choice) {
        case 1:
            strcpy(specialty, "Cardiology");
            strcpy(department, "Cardiology");
            break;
        case 2:
            strcpy(specialty, "Neurology");
            strcpy(department, "Neurology");
            break;
        case 3:
            strcpy(specialty, "Orthopedics");
            strcpy(department, "Orthopedics");
            break;
        case 4:
            strcpy(specialty, "Pediatrics");
            strcpy(department, "Pediatrics");
            break;
        case 5:
            strcpy(specialty, "Gynecology");
            strcpy(department, "Gynecology");
            break;
        case 6:
            strcpy(specialty, "Ophthalmology");
            strcpy(department, "Ophthalmology");
            break;
        case 7:
            strcpy(specialty, "ENT");
            strcpy(department, "ENT");
            break;
        case 8:
            strcpy(specialty, "Gastroenterology");
            strcpy(department, "Gastroenterology");
            break;
        case 9:
            strcpy(specialty, "Dermatology");
            strcpy(department, "Dermatology");
            break;
        case 10:
            strcpy(specialty, "Dentistry");
            strcpy(department, "Dentistry");
            break;
        default:
            strcpy(specialty, "General Physician");
            strcpy(department, "General Medicine");
    }
}

// Helper function to print patient summary
void printPatientSummary(PatientNode* p) {
    cout << "ID: " << p->id << " | Name: " << p->name;
    if (p->isAdmitted) cout << " (Admitted to Room " << p->roomNumber << ")";
    cout << "\n";
}

// Helper function to print doctor summary
void printDoctorSummary(DoctorNode* d) {
    cout << "ID: " << d->id << " | Name: " << d->name
         << " | Dept: " << d->department << "\n";
}

// Search patients by name with disambiguation using BST
PatientNode* searchPatientByNameWithDisambiguation() {
    char name[50];
    cout << "Enter patient name: ";
    cin.ignore();
    cin.getline(name, 50);

    PatientNode* matches[MAX_PATIENTS];
    int matchCount = 0;

    // Search using BST
    searchPatientBST(patientBST, name, matches, matchCount);

    if (matchCount == 0) {
        cout << "No patients found with name containing '" << name << "'.\n";
        return nullptr;
    }

    if (matchCount == 1) {
        return matches[0];
    }

    cout << "\nMultiple patients found:\n";
    for (int i = 0; i < matchCount; i++) {
        cout << i+1 << ". ";
        printPatientSummary(matches[i]);
    }

    int choice;
    cout << "Enter selection (1-" << matchCount << "): ";
    cin >> choice;

    if (choice < 1 || choice > matchCount) {
        cout << "Invalid selection.\n";
        return nullptr;
    }

    return matches[choice-1];
}

// Search patients by ID or name
PatientNode* searchPatient() {
    int choice;
    cout << "Search by:\n";
    cout << "1. ID\n";
    cout << "2. Name\n";
    cout << "Enter choice: ";
    cin >> choice;

    if (choice == 1) {
        int id;
        cout << "Enter patient ID: ";
        cin >> id;
        PatientNode* p = static_cast<PatientNode*>(getFromHashTable(patientHashTable, id));
        if (!p) {
            cout << "Patient not found!\n";
        }
        return p;
    } else if (choice == 2) {
        return searchPatientByNameWithDisambiguation();
    } else {
        cout << "Invalid choice!\n";
        return nullptr;
    }
}

// Search doctors by name with disambiguation using BST
DoctorNode* searchDoctorByNameWithDisambiguation() {
    char name[50];
    cout << "Enter doctor name: ";
    cin.ignore();
    cin.getline(name, 50);

    DoctorNode* matches[MAX_DOCTORS];
    int matchCount = 0;

    // Search using BST
    searchDoctorBST(doctorBST, name, matches, matchCount);

    if (matchCount == 0) {
        cout << "No doctors found with name containing '" << name << "'.\n";
        return nullptr;
    }

    if (matchCount == 1) {
        return matches[0];
    }

    cout << "\nMultiple doctors found:\n";
    for (int i = 0; i < matchCount; i++) {
        cout << i+1 << ". ";
        printDoctorSummary(matches[i]);
    }

    int choice;
    cout << "Enter selection (1-" << matchCount << "): ";
    cin >> choice;

    if (choice < 1 || choice > matchCount) {
        cout << "Invalid selection.\n";
        return nullptr;
    }

    return matches[choice-1];
}

// File handling functions
void loadPatientsFromFile() {
    ifstream file(PATIENTS_FILE);
    if (!file.is_open()) return;

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string token;
        PatientNode* newNode = new PatientNode;

        // ID
        getline(ss, token, ',');
        newNode->id = stoi(token);

        // Name
        getline(ss, token, ',');
        strcpy(newNode->name, token.c_str());

        // Age
        getline(ss, token, ',');
        newNode->age = stoi(token);

        // Gender
        getline(ss, token, ',');
        newNode->gender = token[0];

        // Contact
        getline(ss, token, ',');
        strcpy(newNode->contact, token.c_str());

        // Address
        getline(ss, token, ',');
        strcpy(newNode->address, token.c_str());

        // Medical History
        getline(ss, token, ',');
        strcpy(newNode->medicalHistory, token.c_str());

        // Doctor ID
        getline(ss, token, ',');
        newNode->doctorId = stoi(token);

        // Room Number
        getline(ss, token, ',');
        newNode->roomNumber = stoi(token);

        // Is Admitted
        getline(ss, token, ',');
        newNode->isAdmitted = (token == "1");

        // Is Emergency
        getline(ss, token, ',');
        newNode->isEmergency = (token == "1");

        newNode->next = nullptr;

        // Add to linked list
        if (patientList == nullptr) {
            patientList = newNode;
        } else {
            PatientNode* temp = patientList;
            while (temp->next != nullptr) {
                temp = temp->next;
            }
            temp->next = newNode;
        }

        // Add to BST
        patientBST = insertPatientBST(patientBST, newNode->name, newNode->id);

        // Add to hash table
        addToHashTable(patientHashTable, newNode->id, newNode);
        patientCount++;
    }
    file.close();
}

void savePatientsToFile() {
    ofstream file(PATIENTS_FILE);
     if (!file.is_open()) {
        cerr << "Error: Could not open patients file for writing!\n";
        return;
    }

    PatientNode* current = patientList;
    while (current != nullptr) {
        file << current->id << ",";
        file << current->name << ",";
        file << current->age << ",";
        file << current->gender << ",";
        file << current->contact << ",";
        file << current->address << ",";
        file << current->medicalHistory << ",";
        file << current->doctorId << ",";
        file << current->roomNumber << ",";
        file << (current->isAdmitted ? "1" : "0") << ",";
        file << (current->isEmergency ? "1" : "0") << "\n";
        current = current->next;
    }
    file.close();
}

void loadDoctorsFromFile() {
    ifstream file(DOCTORS_FILE);
    if (!file.is_open()) return;

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string token;
        DoctorNode* newNode = new DoctorNode;

        // ID
        getline(ss, token, ',');
        newNode->id = stoi(token);

        // Name
        getline(ss, token, ',');
        strcpy(newNode->name, token.c_str());

        // Department
        getline(ss, token, ',');
        strcpy(newNode->department, token.c_str());

        // Specialty
        getline(ss, token, ',');
        strcpy(newNode->specialty, token.c_str());

        // Contact
        getline(ss, token, ',');
        strcpy(newNode->contact, token.c_str());

        // Availability (7 days x 24 hours)
        for (int day = 0; day < 7; day++) {
            for (int hour = 0; hour < 24; hour++) {
                getline(ss, token, ',');
                newNode->availability[day][hour] = (token == "1");
            }
        }

        newNode->next = nullptr;

        // Add to linked list
        if (doctorList == nullptr) {
            doctorList = newNode;
        } else {
            DoctorNode* temp = doctorList;
            while (temp->next != nullptr) {
                temp = temp->next;
            }
            temp->next = newNode;
        }

        // Add to BST
        doctorBST = insertDoctorBST(doctorBST, newNode->name, newNode->id);

        // Add to hash table
        addToHashTable(doctorHashTable, newNode->id, newNode);
        doctorCount++;
    }
    file.close();
}

void saveDoctorsToFile() {
    ofstream file(DOCTORS_FILE);
    if (!file.is_open()) return;

    DoctorNode* current = doctorList;
    while (current != nullptr) {
        file << current->id << ",";
        file << current->name << ",";
        file << current->department << ",";
        file << current->specialty << ",";
        file << current->contact << ",";

        // Save availability (7 days x 24 hours)
        for (int day = 0; day < 7; day++) {
            for (int hour = 0; hour < 24; hour++) {
                file << (current->availability[day][hour] ? "1" : "0") << ",";
            }
        }
        file << "\n";
        current = current->next;
    }
    file.close();
}

void loadMedicinesFromFile() {
    ifstream file(MEDICINES_FILE);
    if (!file.is_open()) return;

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string token;
        MedicineNode* newNode = new MedicineNode;

        // ID
        getline(ss, token, ',');
        newNode->id = stoi(token);

        // Name
        getline(ss, token, ',');
        strcpy(newNode->name, token.c_str());

        // Code
        getline(ss, token, ',');
        strcpy(newNode->code, token.c_str());

        // Quantity
        getline(ss, token, ',');
        newNode->quantity = stoi(token);

        // Expiry Date
        getline(ss, token, ',');
        newNode->expiryDate.day = stoi(token);
        getline(ss, token, ',');
        newNode->expiryDate.month = stoi(token);
        getline(ss, token, ',');
        newNode->expiryDate.year = stoi(token);

        // Price
        getline(ss, token, ',');
        newNode->price = stof(token);

        newNode->next = nullptr;

        // Add to linked list
        if (medicineList == nullptr) {
            medicineList = newNode;
        } else {
            MedicineNode* temp = medicineList;
            while (temp->next != nullptr) {
                temp = temp->next;
            }
            temp->next = newNode;
        }

        // Add to BST
        medicineBST = insertMedicineBST(medicineBST, newNode->name, newNode->id);

        // Add to hash table
        addToHashTable(medicineHashTable, newNode->id, newNode);
        medicineCount++;
    }
    file.close();
}

void saveMedicinesToFile() {
    ofstream file(MEDICINES_FILE);
    if (!file.is_open()) return;

    MedicineNode* current = medicineList;
    while (current != nullptr) {
        file << current->id << ",";
        file << current->name << ",";
        file << current->code << ",";
        file << current->quantity << ",";
        file << current->expiryDate.day << ",";
        file << current->expiryDate.month << ",";
        file << current->expiryDate.year << ",";
        file << current->price << "\n";
        current = current->next;
    }
    file.close();
}

void loadAppointmentsFromFile() {
    ifstream file(APPOINTMENTS_FILE);
    if (!file.is_open()) return;

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string token;
        AppointmentNode* newNode = new AppointmentNode;

        // ID
        getline(ss, token, ',');
        newNode->id = stoi(token);

        // Patient ID
        getline(ss, token, ',');
        newNode->patientId = stoi(token);

        // Doctor ID
        getline(ss, token, ',');
        newNode->doctorId = stoi(token);

        // Date
        getline(ss, token, ',');
        newNode->date.day = stoi(token);
        getline(ss, token, ',');
        newNode->date.month = stoi(token);
        getline(ss, token, ',');
        newNode->date.year = stoi(token);

        // Time
        getline(ss, token, ',');
        newNode->time.hour = stoi(token);
        getline(ss, token, ',');
        newNode->time.minute = stoi(token);

        // Purpose
        getline(ss, token, ',');
        strcpy(newNode->purpose, token.c_str());

        // Status
        getline(ss, token, ',');
        newNode->status = stoi(token);

        newNode->next = nullptr;

        // Add to linked list
        if (appointmentList == nullptr) {
            appointmentList = newNode;
        } else {
            AppointmentNode* temp = appointmentList;
            while (temp->next != nullptr) {
                temp = temp->next;
            }
            temp->next = newNode;
        }
        appointmentCount++;
    }
    file.close();
}

void saveAppointmentsToFile() {
    ofstream file(APPOINTMENTS_FILE);
    if (!file.is_open()) return;

    AppointmentNode* current = appointmentList;
    while (current != nullptr) {
        file << current->id << ",";
        file << current->patientId << ",";
        file << current->doctorId << ",";
        file << current->date.day << ",";
        file << current->date.month << ",";
        file << current->date.year << ",";
        file << current->time.hour << ",";
        file << current->time.minute << ",";
        file << current->purpose << ",";
        file << current->status << "\n";
        current = current->next;
    }
    file.close();
}

void loadRoomsFromFile() {
    ifstream file(ROOMS_FILE);
    if (!file.is_open()) {
        // Initialize default rooms if file doesn't exist
        for (int i = 0; i < MAX_ROOMS; i++) {
            rooms[i].number = 100 + i;
            rooms[i].isOccupied = false;
            rooms[i].patientId = -1;
        }
        roomCount = MAX_ROOMS;
        return;
    }

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string token;
        int index;

        // Room number
        getline(ss, token, ',');
        index = stoi(token) - 100; // Assuming room numbers start from 100
        if (index < 0 || index >= MAX_ROOMS) continue;

        // Is Occupied
        getline(ss, token, ',');
        rooms[index].isOccupied = (token == "1");

        // Patient ID
        getline(ss, token, ',');
        rooms[index].patientId = stoi(token);
    }
    file.close();
}

void saveRoomsToFile() {
    ofstream file(ROOMS_FILE);
    if (!file.is_open()) return;

    for (int i = 0; i < roomCount; i++) {
        file << rooms[i].number << ",";
        file << (rooms[i].isOccupied ? "1" : "0") << ",";
        file << rooms[i].patientId << "\n";
    }
    file.close();
}

void loadEmergencyQueueFromFile() {
    ifstream file(EMERGENCY_FILE);
    if (!file.is_open()) return;

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string token;
        EmergencyNode* newNode = new EmergencyNode;

        // Patient ID
        getline(ss, token, ',');
        newNode->patientId = stoi(token);

        // Priority
        getline(ss, token, ',');
        newNode->priority = stoi(token);

        newNode->next = nullptr;

        // Add to queue
        if (emergencyQueue == nullptr) {
            emergencyQueue = newNode;
        } else {
            EmergencyNode* temp = emergencyQueue;
            while (temp->next != nullptr) {
                temp = temp->next;
            }
            temp->next = newNode;
        }
    }
    file.close();
}

// Patient Management Functions
void addPatient() {
    if (patientCount >= MAX_PATIENTS) {
        cout << "Maximum patient capacity reached!\n";
        return;
    }

    PatientNode* newNode = new PatientNode;
    newNode->id = generatePatientId();
    cout << "Enter patient details:\n";

    // Name validation
    do {
        cout << "Name (letters only): ";
        cin.ignore();
        cin.getline(newNode->name, 50);
        if (!isValidName(newNode->name)) {
            cout << "Invalid name! Only letters and spaces are allowed.\n";
        }
    } while (!isValidName(newNode->name));

    // Age validation
    do {
        cout << "Age: ";
        cin >> newNode->age;
        if (!isValidAge(newNode->age)) {
            cout << "Invalid age! Please enter a number between 1-120.\n";
        }
    } while (!isValidAge(newNode->age));

    // Gender validation
    do {
        cout << "Gender (M/F/J): ";
        cin >> newNode->gender;
        newNode->gender = toupper(newNode->gender);
        if (!isValidGender(newNode->gender)) {
            cout << "Invalid gender! Please enter M, F, or J.\n";
        }
    } while (!isValidGender(newNode->gender));

    // Contact validation
    do {
        cout << "Contact (11 digits only): ";
        cin.ignore();
        cin.getline(newNode->contact, 15);
        if (!isValidPhone(newNode->contact)) {
            cout << "Invalid phone number! Must be exactly 11 digits.\n";
        }
    } while (!isValidPhone(newNode->contact));

    cout << "Address: ";
    cin.getline(newNode->address, 100);
    cout << "Medical History: ";
    cin.getline(newNode->medicalHistory, 500);
    newNode->doctorId = -1;
    newNode->roomNumber = -1;
    newNode->isAdmitted = false;
    newNode->isEmergency = false;
    newNode->next = nullptr;

    // Add to linked list
    if (patientList == nullptr) {
        patientList = newNode;
    } else {
        PatientNode* temp = patientList;
        while (temp->next != nullptr) {
            temp = temp->next;
        }
        temp->next = newNode;
    }

    // Add to BST
    patientBST = insertPatientBST(patientBST, newNode->name, newNode->id);

    // Add to hash table
    addToHashTable(patientHashTable, newNode->id, newNode);
    patientCount++;

    // Save to file
    savePatientsToFile();

    cout << "Patient added successfully! ID: " << newNode->id << "\n";
}

PatientNode* findPatient(int id) {
    return static_cast<PatientNode*>(getFromHashTable(patientHashTable, id));
}

void viewPatient(int id) {
    PatientNode* p = findPatient(id);
    if (!p) {
        cout << "Patient not found!\n";
        return;
    }

    cout << "\nPatient Details:\n";
    cout << "ID: " << p->id << "\n";
    cout << "Name: " << p->name << "\n";
    cout << "Age: " << p->age << "\n";
    cout << "Gender: " << p->gender << "\n";
    cout << "Contact: " << p->contact << "\n";
    cout << "Address: " << p->address << "\n";
    cout << "Medical History: " << p->medicalHistory << "\n";
    cout << "Admitted: " << (p->isAdmitted ? "Yes" : "No") << "\n";
    if (p->isAdmitted) {
        cout << "Room Number: " << p->roomNumber << "\n";
    }
    if (p->doctorId != -1) {
        cout << "Assigned Doctor ID: " << p->doctorId << "\n";
    }
    cout << "Emergency Case: " << (p->isEmergency ? "Yes" : "No") << "\n";
}

void updatePatient(int id) {
    PatientNode* p = findPatient(id);
    if (!p) {
        cout << "Patient not found!\n";
        return;
    }

    cout << "Update patient details (leave blank to keep current value):\n";
    cout << "Name (" << p->name << "): ";
    cin.ignore();
    char name[50];
    cin.getline(name, 50);
    if (strlen(name) > 0) {
        if (!isValidName(name)) {
            cout << "Invalid name! Only letters and spaces allowed.\n";
            return;
        }
        strcpy(p->name, name);
    }

    cout << "Age (" << p->age << "): ";
    char ageInput[10];
    cin.getline(ageInput, 10);
    if (strlen(ageInput) > 0) {
        int newAge = atoi(ageInput);
        if (isValidAge(newAge)) {
            p->age = newAge;
        } else {
            cout << "Invalid age! Please enter a number between 1-120.\n";
            return;
        }
    }

    cout << "Gender (" << p->gender << "): ";
    char gender;
    cin >> gender;
    if (gender == 'M' || gender == 'F' || gender == 'J') p->gender = toupper(gender);
    cin.ignore();

    cout << "Contact (" << p->contact << "): ";
    char contact[15];
    cin.getline(contact, 15);
    if (strlen(contact) > 0) {
        if (!isValidPhone(contact)) {
            cout << "Invalid phone number! Must be exactly 11 digits.\n";
            return;
        }
        strcpy(p->contact, contact);
    }

    cout << "Address (" << p->address << "): ";
    char address[100];
    cin.getline(address, 100);
    if (strlen(address) > 0) strcpy(p->address, address);

    cout << "Medical History (" << p->medicalHistory << "): ";
    char history[500];
    cin.getline(history, 500);
    if (strlen(history) > 0) strcpy(p->medicalHistory, history);

    // Save to file
    savePatientsToFile();

    cout << "Patient updated successfully!\n";
}

// Add this function to properly remove from BST
PatientBSTNode* removePatientBST(PatientBSTNode* root, int id, const char* name) {
    if (!root) return nullptr;

    PatientNode* currentPatient = static_cast<PatientNode*>(getFromHashTable(patientHashTable, root->id));
    if (!currentPatient) {
        // The patient was deleted, remove this BST node
        PatientBSTNode* temp = root;
        if (!root->left) {
            root = root->right;
        } else if (!root->right) {
            root = root->left;
        } else {
            // Node with two children
            PatientBSTNode* successor = root->right;
            while (successor->left) successor = successor->left;
            root->id = successor->id;
            root->right = removePatientBST(root->right, successor->id, "");
        }
        delete temp;
    } else if (strcmp(name, currentPatient->name) < 0) {
        root->left = removePatientBST(root->left, id, name);
    } else if (strcmp(name, currentPatient->name) > 0) {
        root->right = removePatientBST(root->right, id, name);
    } else {
        // Found the node to delete
        if (!root->left) {
            PatientBSTNode* temp = root->right;
            delete root;
            return temp;
        } else if (!root->right) {
            PatientBSTNode* temp = root->left;
            delete root;
            return temp;
        }
        // Node with two children
        PatientBSTNode* successor = root->right;
        while (successor->left) successor = successor->left;
        root->id = successor->id;
        root->right = removePatientBST(root->right, successor->id, "");
    }
    return root;
}

void deletePatient(int id) {
    PatientNode* p = findPatient(id);
    if (!p) {
        cout << "Patient not found!\n";
        return;
    }

    if (p->isAdmitted) {
        cout << "Cannot delete patient who is currently admitted!\n";
        return;
    }

    // Remove from BST
    patientBST = removePatientBST(patientBST, id, p->name);

    // Remove from hash table
    removeFromHashTable(patientHashTable, id);

    // Remove from linked list
    if (patientList == p) {
        patientList = p->next;
    } else {
        PatientNode* current = patientList;
        while (current != nullptr && current->next != p) {
            current = current->next;
        }
        if (current != nullptr) {
            current->next = p->next;
        }
    }

    // Remove from BST (would need to rebuild BST for proper deletion)
    // For simplicity, we'll leave this as is (the BST node will remain but the patient won't be found by ID)

    delete p;
    patientCount--;

    // Save to file
    savePatientsToFile();

    cout << "Patient deleted successfully!\n";
}

DoctorNode* findDoctor(int id) {
    return static_cast<DoctorNode*>(getFromHashTable(doctorHashTable, id));
}

void assignDoctorToPatient(int patientId, int doctorId) {
    PatientNode* p = findPatient(patientId);
    if (!p) {
        cout << "Patient not found!\n";
        return;
    }

    DoctorNode* d = findDoctor(doctorId);
    if (!d) {
        cout << "Doctor not found!\n";
        return;
    }

    p->doctorId = doctorId;

    // Save to file
    savePatientsToFile();

    cout << "Doctor " << d->name << " (ID: " << doctorId << ") assigned to patient " << p->name << " (ID: " << patientId << ") successfully!\n";
}

void admitPatient(int patientId) {
    PatientNode* p = findPatient(patientId);
    if (!p) {
        cout << "Patient not found!\n";
        return;
    }

    if (p->isAdmitted) {
        cout << "Patient is already admitted!\n";
        return;
    }

    // Ask if this is an emergency case
    char emergencyChoice;
    cout << "Is this an emergency case? (Y/N): ";
    cin >> emergencyChoice;
    bool emergency = (toupper(emergencyChoice) == 'Y');

    // Find available room
    int roomNum = -1;

    for (int i = 0; i < roomCount; i++) {
        if (!rooms[i].isOccupied) {
            roomNum = rooms[i].number;
            rooms[i].isOccupied = true;
            rooms[i].patientId = patientId;
            break;
        }
    }

    if (roomNum == -1) {
        cout << "No rooms available!\n";
        return;
    }

    p->isAdmitted = true;
    p->roomNumber = roomNum;
    p->isEmergency = emergency;

    if (emergency) {
        enqueueEmergency(patientId, 1); // Add to emergency queue with high priority
        cout << "Emergency case admitted successfully to room " << roomNum << "\n";
    } else {
        cout << "Patient admitted successfully to room " << roomNum << "\n";
    }

    // Save to files
    savePatientsToFile();
    saveRoomsToFile();
}

void dischargePatient(int patientId) {
    PatientNode* p = findPatient(patientId);
    if (!p) {
        cout << "Patient not found!\n";
        return;
    }

    if (!p->isAdmitted) {
        cout << "Patient is not admitted!\n";
        return;
    }

    // Free the room
    for (int i = 0; i < roomCount; i++) {
        if (rooms[i].number == p->roomNumber) {
            rooms[i].isOccupied = false;
            rooms[i].patientId = -1;
            break;
        }
    }


    p->isAdmitted = false;
    p->roomNumber = -1;
    p->isEmergency = false;

    // Save to files
    savePatientsToFile();
    saveRoomsToFile();

    cout << "Patient discharged successfully!\n";
}

void initializeDefaultDoctors() {
    // Check if doctors file exists and is not empty
    ifstream file(DOCTORS_FILE);
    if (file.is_open() && file.peek() != ifstream::traits_type::eof()) {
        file.close();
        return; // File exists and has data, no need to initialize
    }
    file.close();

    // Doctor 1: General Physician
    DoctorNode* d1 = new DoctorNode;
    d1->id = generateDoctorId();
    strcpy(d1->name, "Dr. Smith");
    strcpy(d1->department, "General Medicine");
    strcpy(d1->specialty, "General Physician");
    strcpy(d1->contact, "12345678901"); // Updated to 11 digits
    // Set availability (simplified: available all week 9 AM - 5 PM)
    for (int day = 0; day < 7; day++) {
        for (int hour = 0; hour < 24; hour++) {
            d1->availability[day][hour] = (hour >= 9 && hour <= 17); // Available 9 AM - 5 PM
        }
    }
    d1->next = nullptr;

    // Add to linked list
    if (doctorList == nullptr) {
        doctorList = d1;
    } else {
        DoctorNode* temp = doctorList;
        while (temp->next != nullptr) {
            temp = temp->next;
        }
        temp->next = d1;
    }

    // Add to BST
    doctorBST = insertDoctorBST(doctorBST, d1->name, d1->id);

    // Add to hash table
    addToHashTable(doctorHashTable, d1->id, d1);
    doctorCount++;

    // Doctor 2: Cardiologist
    DoctorNode* d2 = new DoctorNode;
    d2->id = generateDoctorId();
    strcpy(d2->name, "Dr. Johnson");
    strcpy(d2->department, "Cardiology");
    strcpy(d2->specialty, "Cardiology");
    strcpy(d2->contact, "12345678902"); // Updated to 11 digits
    // Available Mon-Fri, 10 AM - 4 PM
    for (int day = 0; day < 7; day++) {
        for (int hour = 0; hour < 24; hour++) {
            d2->availability[day][hour] = (day >= 1 && day <= 5 && hour >= 10 && hour <= 16); // Mon-Fri, 10 AM - 4 PM
        }
    }
    d2->next = nullptr;

    // Add to linked list
    DoctorNode* temp = doctorList;
    while (temp->next != nullptr) {
        temp = temp->next;
    }
    temp->next = d2;

    // Add to BST
    doctorBST = insertDoctorBST(doctorBST, d2->name, d2->id);

    // Add to hash table
    addToHashTable(doctorHashTable, d2->id, d2);
    doctorCount++;

    // Doctor 3: Pediatrician
    DoctorNode* d3 = new DoctorNode;
    d3->id = generateDoctorId();
    strcpy(d3->name, "Dr. Williams");
    strcpy(d3->department, "Pediatrics");
    strcpy(d3->specialty, "Pediatrics");
    strcpy(d3->contact, "12345678903"); // Updated to 11 digits
    // Available Tue, Thu, Sat (11 AM - 7 PM)
    for (int day = 0; day < 7; day++) {
        for (int hour = 0; hour < 24; hour++) {
            d3->availability[day][hour] = ((day == 2 || day == 4 || day == 6) && (hour >= 11 && hour <= 19)); // Tue, Thu, Sat (11 AM - 7 PM)
        }
    }
    d3->next = nullptr;

    // Add to linked list
    temp = doctorList;
    while (temp->next != nullptr) {
        temp = temp->next;
    }
    temp->next = d3;

    // Add to BST
    doctorBST = insertDoctorBST(doctorBST, d3->name, d3->id);

    // Add to hash table
    addToHashTable(doctorHashTable, d3->id, d3);
    doctorCount++;

    // Save to file
    saveDoctorsToFile();
}

// Doctor Management Functions
void addDoctor() {
    if (doctorCount >= MAX_DOCTORS) {
        cout << "Maximum doctor capacity reached!\n";
        return;
    }

    DoctorNode* newNode = new DoctorNode;
    newNode->id = generateDoctorId();
    cout << "Enter doctor details:\n";

    // Name validation
    do {
        cout << "Name (letters only): ";
        cin.ignore();
        cin.getline(newNode->name, 50);
        if (!isValidName(newNode->name)) {
            cout << "Invalid name! Only letters and spaces are allowed.\n";
        }
    } while (!isValidName(newNode->name));

    // Get specialization and department
    getDoctorSpecialization(newNode->specialty, newNode->department);

    // Contact validation
    do {
        cout << "Contact (11 digits only): ";
        cin.getline(newNode->contact, 15);
        if (!isValidPhone(newNode->contact)) {
            cout << "Invalid phone number! Must be exactly 11 digits.\n";
        }
    } while (!isValidPhone(newNode->contact));

    // Initialize availability (default to all available)
    for (int day = 0; day < 7; day++) {
        for (int hour = 0; hour < 24; hour++) {
            newNode->availability[day][hour] = true;
        }
    }
    newNode->next = nullptr;

    // Add to linked list
    if (doctorList == nullptr) {
        doctorList = newNode;
    } else {
        DoctorNode* temp = doctorList;
        while (temp->next != nullptr) {
            temp = temp->next;
        }
        temp->next = newNode;
    }

    // Add to BST
    doctorBST = insertDoctorBST(doctorBST, newNode->name, newNode->id);

    // Add to hash table
    addToHashTable(doctorHashTable, newNode->id, newNode);
    doctorCount++;

    // Save to file
    saveDoctorsToFile();

    cout << "Doctor added successfully! ID: " << newNode->id << "\n";
}

void viewDoctor(int id) {
    DoctorNode* d = findDoctor(id);
    if (!d) {
        cout << "Doctor not found!\n";
        return;
    }

    cout << "\nDoctor Details:\n";
    cout << "ID: " << d->id << "\n";
    cout << "Name: " << d->name << "\n";
    cout << "Department: " << d->department << "\n";
    cout << "Specialty: " << d->specialty << "\n";
    cout << "Contact: " << d->contact << "\n";

    cout << "Availability:\n";
    const char* days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    for (int day = 0; day < 7; day++) {
        cout << days[day] << ": ";
        bool available = false;
        for (int hour = 0; hour < 24; hour++) {
            if (d->availability[day][hour]) {
                if (!available) {
                    cout << hour << ":00-";
                    available = true;
                }
            } else {
                if (available) {
                    cout << hour << ":00 ";
                    available = false;
                }
            }
        }
        if (available) cout << "24:00 ";
        cout << "\n";
    }
}

void updateDoctor(int id) {
    DoctorNode* d = findDoctor(id);
    if (!d) {
        cout << "Doctor not found!\n";
        return;
    }

    cout << "Update doctor details (leave blank to keep current value):\n";
    cout << "Name (" << d->name << "): ";
    cin.ignore();
    char name[50];
    cin.getline(name, 50);
    if (strlen(name) > 0) {
        if (!isValidName(name)) {
            cout << "Invalid name! Only letters and spaces allowed.\n";
            return;
        }
        strcpy(d->name, name);
    }

    cout << "Specialty (" << d->specialty << "): ";
    char spec[50];
    cin.getline(spec, 50);
    if (strlen(spec) > 0) {
        char newDept[50];
        getDoctorSpecialization(spec, newDept);
        strcpy(d->specialty, spec);
        strcpy(d->department, newDept);
    }

    cout << "Contact (" << d->contact << "): ";
    char contact[15];
    cin.getline(contact, 15);
    if (strlen(contact) > 0) {
        if (!isValidPhone(contact)) {
            cout << "Invalid phone number! Must be exactly 11 digits.\n";
            return;
        }
        strcpy(d->contact, contact);
    }

    // Save to file
    saveDoctorsToFile();

    cout << "Doctor updated successfully!\n";
}

void deleteDoctor(int id) {
    DoctorNode* d = findDoctor(id);
    if (!d) {
        cout << "Doctor not found!\n";
        return;
    }

    // Check if doctor has any patients
    PatientNode* currentPatient = patientList;
    while (currentPatient != nullptr) {
        if (currentPatient->doctorId == id) {
            cout << "Cannot delete doctor who has assigned patients!\n";
            return;
        }
        currentPatient = currentPatient->next;
    }

    // Check if doctor has any appointments
    AppointmentNode* currentAppt = appointmentList;
    while (currentAppt != nullptr) {
        if (currentAppt->doctorId == id && currentAppt->status == 0) {
            cout << "Cannot delete doctor who has scheduled appointments!\n";
            return;
        }
        currentAppt = currentAppt->next;
    }

    // Remove from hash table
    removeFromHashTable(doctorHashTable, id);

    // Remove from linked list
    if (doctorList == d) {
        doctorList = d->next;
    } else {
        DoctorNode* current = doctorList;
        while (current != nullptr && current->next != d) {
            current = current->next;
        }
        if (current != nullptr) {
            current->next = d->next;
        }
    }

    // Remove from BST would require rebuilding the BST
    // For simplicity, we'll leave this as is

    delete d;
    doctorCount--;

    // Save to file
    saveDoctorsToFile();

    cout << "Doctor deleted successfully!\n";
}

void viewDoctorsByDepartment(const char* department) {
    cout << "\nDoctors in " << department << " department:\n";
    bool found = false;
    DoctorNode* current = doctorList;
    while (current != nullptr) {
        if (strcmp(current->department, department) == 0) {
            cout << current->id << " - " << current->name << " (" << current->specialty << ")\n";
            found = true;
        }
        current = current->next;
    }

    if (!found) {
        cout << "No doctors found in this department!\n";
    }
}

// Appointment Management Functions
void addAppointment(int patientId, int doctorId) {
    if (appointmentCount >= MAX_APPOINTMENTS) {
        cout << "Maximum appointment capacity reached!\n";
        return;
    }

    AppointmentNode* newNode = new AppointmentNode;
    newNode->id = generateAppointmentId();
    newNode->patientId = patientId;
    newNode->doctorId = doctorId;

    // Get patient
    PatientNode* p = searchPatient();
    if (!p) return;
    newNode->patientId = p->id;

    // Get doctor
    DoctorNode* d = searchDoctorByNameWithDisambiguation();
    if (!d) return;
    newNode->doctorId = d->id;

    cout << "Enter appointment date (DD MM YYYY): ";
    cin >> newNode->date.day >> newNode->date.month >> newNode->date.year;
    if (!isDateValid(newNode->date)) {
        cout << "Invalid date!\n";
        delete newNode;
        return;
    }

    cout << "Enter appointment time (HH MM): ";
    cin >> newNode->time.hour >> newNode->time.minute;
    if (!isTimeValid(newNode->time)) {
        cout << "Invalid time!\n";
        delete newNode;
        return;
    }

    // Check doctor availability
    time_t now = time(0);
    tm* ltm = localtime(&now);
    int currentDay = ltm->tm_wday; // 0 = Sunday, ..., 6 = Saturday

    int apptDay = -1;
    // This is a simplified check - in real system would need proper date->day calculation
    if (newNode->date.year == 1900 + ltm->tm_year && newNode->date.month == 1 + ltm->tm_mon) {
        apptDay = (currentDay + (newNode->date.day - ltm->tm_mday)) % 7;
    }

    if (apptDay != -1 && !d->availability[apptDay][newNode->time.hour]) {
        cout << "Doctor not available at this time!\n";
        delete newNode;
        return;
    }

    // Check for conflicting appointments
    AppointmentNode* current = appointmentList;
    while (current != nullptr) {
        if (current->doctorId == newNode->doctorId &&
            current->date.day == newNode->date.day &&
            current->date.month == newNode->date.month &&
            current->date.year == newNode->date.year &&
            current->time.hour == newNode->time.hour &&
            current->status == 0) {
            cout << "Doctor already has an appointment at this time!\n";
            delete newNode;
            return;
        }
        current = current->next;
    }

    cout << "Enter purpose: ";
    cin.ignore();
    cin.getline(newNode->purpose, 100);
    newNode->status = 0; // Scheduled
    newNode->next = nullptr;

    // Add to appointment list
    if (appointmentList == nullptr) {
        appointmentList = newNode;
    } else {
        AppointmentNode* temp = appointmentList;
        while (temp->next != nullptr) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
    appointmentCount++;

    // Sort appointments
    AppointmentNode* tail = getTail(appointmentList);
    quickSortAppointments(appointmentList, tail);

    // Save to file
    saveAppointmentsToFile();

    cout << "Appointment scheduled successfully! ID: " << newNode->id << "\n";
}

void viewAppointment(int id) {
    AppointmentNode* current = appointmentList;
    while (current != nullptr) {
        if (current->id == id) {
            cout << "\nAppointment Details:\n";
            cout << "ID: " << current->id << "\n";

            PatientNode* p = findPatient(current->patientId);
            cout << "Patient: " << (p ? p->name : "Unknown") << " (ID: " << current->patientId << ")\n";

            DoctorNode* d = findDoctor(current->doctorId);
            cout << "Doctor: " << (d ? d->name : "Unknown") << " (ID: " << current->doctorId << ")\n";

            cout << "Date: " << current->date.day << "/" << current->date.month
                 << "/" << current->date.year << "\n";
            cout << "Time: " << current->time.hour << ":"
                 << (current->time.minute < 10 ? "0" : "") << current->time.minute << "\n";
            cout << "Purpose: " << current->purpose << "\n";

            const char* statuses[] = {"Scheduled", "Completed", "Cancelled", "No-show"};
            cout << "Status: " << statuses[current->status] << "\n";

            return;
        }
        current = current->next;
    }
    cout << "Appointment not found!\n";
}

void viewAppointmentsByPatientName() {
    PatientNode* p = searchPatient();
    if (!p) return;

    // Sort appointments before displaying
    AppointmentNode* tail = getTail(appointmentList);
    quickSortAppointments(appointmentList, tail);

    cout << "\nAppointments for " << p->name << ":\n";

    bool found = false;
    AppointmentNode* current = appointmentList;
    while (current != nullptr) {
        if (current->patientId == p->id) {
            found = true;
            cout << "ID: " << current->id << "\n";
            cout << "Date: " << current->date.day << "/"
                 << current->date.month << "/" << current->date.year << "\n";
            cout << "Time: " << current->time.hour << ":"
                 << (current->time.minute < 10 ? "0" : "") << current->time.minute << "\n";

            DoctorNode* doc = findDoctor(current->doctorId);
            cout << "Doctor: " << (doc ? doc->name : "Unknown") << "\n";
            cout << "Purpose: " << current->purpose << "\n";
            cout << "-------------------\n";
        }
        current = current->next;
    }

    if (!found) {
        cout << "No appointments found for this patient.\n";
    }
}

void viewAppointmentsByDoctorName() {
    DoctorNode* d = searchDoctorByNameWithDisambiguation();
    if (!d) return;

    // Sort appointments before displaying
    AppointmentNode* tail = getTail(appointmentList);
    quickSortAppointments(appointmentList, tail);

    cout << "\nAppointments for Dr. " << d->name << ":\n";

    bool found = false;
    AppointmentNode* current = appointmentList;
    while (current != nullptr) {
        if (current->doctorId == d->id) {
            found = true;
            cout << "ID: " << current->id << "\n";
            cout << "Date: " << current->date.day << "/"
                 << current->date.month << "/" << current->date.year << "\n";
            cout << "Time: " << current->time.hour << ":"
                 << (current->time.minute < 10 ? "0" : "") << current->time.minute << "\n";

            PatientNode* pat = findPatient(current->patientId);
            cout << "Patient: " << (pat ? pat->name : "Unknown") << "\n";
            cout << "Purpose: " << current->purpose << "\n";
            cout << "-------------------\n";
        }
        current = current->next;
    }

    if (!found) {
        cout << "No appointments found for this doctor.\n";
    }
}

void updateAppointmentStatus(int id, int status) {
    AppointmentNode* current = appointmentList;
    while (current != nullptr) {
        if (current->id == id) {
            if (status >= 0 && status <= 3) {
                current->status = status;
                // Save to file
                saveAppointmentsToFile();
                cout << "Appointment status updated successfully!\n";
            } else {
                cout << "Invalid status code!\n";
            }
            return;
        }
        current = current->next;
    }
    cout << "Appointment not found!\n";
}

void cancelAppointment(int id) {
    updateAppointmentStatus(id, 2); // 2 = Cancelled
}

void searchAppointmentsByPatient(int patientId) {
    cout << "\nAppointments for patient ID " << patientId << ":\n";
    bool found = false;
    AppointmentNode* current = appointmentList;
    while (current != nullptr) {
        if (current->patientId == patientId) {
            cout << "Appt ID: " << current->id << " - ";
            cout << "Date: " << current->date.day << "/" << current->date.month << "/" << current->date.year;
            cout << " Time: " << current->time.hour << ":" << current->time.minute;

            DoctorNode* d = findDoctor(current->doctorId);
            cout << " with Dr. " << (d ? d->name : "Unknown") << "\n";

            found = true;
        }
        current = current->next;
    }

    if (!found) {
        cout << "No appointments found for this patient!\n";
    }
}

void searchAppointmentsByDoctor(int doctorId) {
    cout << "\nAppointments for doctor ID " << doctorId << ":\n";
    bool found = false;
    AppointmentNode* current = appointmentList;
    while (current != nullptr) {
        if (current->doctorId == doctorId) {
            cout << "Appt ID: " << current->id << " - ";
            cout << "Date: " << current->date.day << "/" << current->date.month << "/" << current->date.year;
            cout << " Time: " << current->time.hour << ":" << current->time.minute;

            PatientNode* p = findPatient(current->patientId);
            cout << " with " << (p ? p->name : "Unknown") << "\n";

            found = true;
        }
        current = current->next;
    }

    if (!found) {
        cout << "No appointments found for this doctor!\n";
    }
}

// Room Management Functions
void initializeRooms() {
    loadRoomsFromFile();
    if (roomCount == 0) {
        for (int i = 0; i < MAX_ROOMS; i++) {
            rooms[i].number = 100 + i;
            rooms[i].isOccupied = false;
            rooms[i].patientId = -1;
        }
        roomCount = MAX_ROOMS;
        saveRoomsToFile();
    }
}

void viewRoomStatus() {
    cout << "\nRoom Status:\n";
    cout << "Room#\tStatus\t\tPatient ID\n";
    for (int i = 0; i < roomCount; i++) {
        cout << rooms[i].number << "\t";
        cout << (rooms[i].isOccupied ? "Occupied" : "Available") << "\t";
        if (rooms[i].isOccupied) {
            cout << rooms[i].patientId;
        }
        cout << "\n";
    }
}

// Medicine Management Functions
void addMedicine() {
    if (medicineCount >= MAX_MEDICINES) {
        cout << "Maximum medicine capacity reached!\n";
        return;
    }

    MedicineNode* newNode = new MedicineNode;
    newNode->id = generateMedicineId();
    cout << "Enter medicine details:\n";
    cout << "Name: ";
    cin.ignore();
    cin.getline(newNode->name, 50);
    cout << "Code: ";
    cin.getline(newNode->code, 20);
    cout << "Quantity: ";
    cin >> newNode->quantity;
    cout << "Expiry date (DD MM YYYY): ";
    cin >> newNode->expiryDate.day >> newNode->expiryDate.month >> newNode->expiryDate.year;
    if (!isDateValid(newNode->expiryDate)) {
        cout << "Invalid date!\n";
        delete newNode;
        return;
    }
    cout << "Price: ";
    cin >> newNode->price;
    newNode->next = nullptr;

    // Add to linked list
    if (medicineList == nullptr) {
        medicineList = newNode;
    } else {
        MedicineNode* temp = medicineList;
        while (temp->next != nullptr) {
            temp = temp->next;
        }
        temp->next = newNode;
    }

    // Add to BST
    medicineBST = insertMedicineBST(medicineBST, newNode->name, newNode->id);

    // Add to hash table
    addToHashTable(medicineHashTable, newNode->id, newNode);
    medicineCount++;

    // Save to file
    saveMedicinesToFile();

    cout << "Medicine added successfully! ID: " << newNode->id << "\n";
}

MedicineNode* findMedicine(int id) {
    return static_cast<MedicineNode*>(getFromHashTable(medicineHashTable, id));
}

void searchMedicineByName(const char* name) {
    cout << "\nSearch results for '" << name << "':\n";
    MedicineNode* results[MAX_MEDICINES];
    int count = 0;

    // Search using BST
    searchMedicineBST(medicineBST, name, results, count);

    if (count == 0) {
        cout << "No medicines found with that name!\n";
        return;
    }

    for (int i = 0; i < count; i++) {
        cout << "ID: " << results[i]->id << "\n";
        cout << "Name: " << results[i]->name << "\n";
        cout << "Code: " << results[i]->code << "\n";
        cout << "Quantity: " << results[i]->quantity << "\n";
        cout << "Expiry: " << results[i]->expiryDate.day << "/"
             << results[i]->expiryDate.month << "/" << results[i]->expiryDate.year << "\n";
        cout << "Price: $" << results[i]->price << "\n\n";
    }
}

void searchMedicineByCode(const char* code) {
    cout << "\nSearch results for code '" << code << "':\n";
    bool found = false;
    MedicineNode* current = medicineList;
    while (current != nullptr) {
        if (strcmp(current->code, code) == 0) {
            cout << "ID: " << current->id << "\n";
            cout << "Name: " << current->name << "\n";
            cout << "Code: " << current->code << "\n";
            cout << "Quantity: " << current->quantity << "\n";
            cout << "Expiry: " << current->expiryDate.day << "/"
                 << current->expiryDate.month << "/" << current->expiryDate.year << "\n";
            cout << "Price: $" << current->price << "\n\n";
            found = true;
            break; // Codes should be unique
        }
        current = current->next;
    }

    if (!found) {
        cout << "No medicines found with that code!\n";
    }
}

void removeExpiredMedicines() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    Date currentDate;
    currentDate.day = ltm->tm_mday;
    currentDate.month = 1 + ltm->tm_mon;
    currentDate.year = 1900 + ltm->tm_year;

    int removed = 0;
    MedicineNode* current = medicineList;
    MedicineNode* prev = nullptr;

    while (current != nullptr) {
        if (isDateBefore(current->expiryDate, currentDate)) {
            // Remove from hash table
            removeFromHashTable(medicineHashTable, current->id);

            // Remove from BST (would need to rebuild)

            // Remove from linked list
            if (prev == nullptr) {
                medicineList = current->next;
            } else {
                prev->next = current->next;
            }

            MedicineNode* toDelete = current;
            current = current->next;
            delete toDelete;
            medicineCount--;
            removed++;
        } else {
            prev = current;
            current = current->next;
        }
    }

    // Save to file
    saveMedicinesToFile();

    cout << "Removed " << removed << " expired medicines.\n";
}

void viewAllMedicines() {
    cout << "\nAll Medicines in Stock:\n";
    cout << "ID\tName\t\tCode\tQuantity\tExpiry\t\tPrice\n";
    cout << "------------------------------------------------------------\n";
    MedicineNode* current = medicineList;
    while (current != nullptr) {
        cout << current->id << "\t";
        cout << current->name << "\t";
        cout << current->code << "\t";
        cout << current->quantity << "\t\t";
        cout << current->expiryDate.day << "/"
             << current->expiryDate.month << "/"
             << current->expiryDate.year << "\t";
        cout << "$" << current->price << "\n";
        current = current->next;
    }
    if (medicineCount == 0) {
        cout << "No medicines in inventory!\n";
    }
}

// Emergency Handling Functions
void admitEmergencyPatient() {
    PatientNode* p = searchPatient();
    if (p) {
        cout << "This is being processed as an emergency case.\n";
        admitPatient(p->id);
    }
}

void viewEmergencyPatients() {
    cout << "\nEmergency Patients:\n";
    bool found = false;
    PatientNode* current = patientList;
    while (current != nullptr) {
        if (current->isEmergency) {
            printPatientSummary(current);
            found = true;
        }
        current = current->next;
    }
    if (!found) cout << "No emergency patients!\n";
}

void processNextEmergency() {
    if (!emergencyQueue) {
        cout << "No emergency cases in queue!\n";
        return;
    }

    EmergencyNode* temp = emergencyQueue;
    emergencyQueue = emergencyQueue->next;

    PatientNode* p = findPatient(temp->patientId);
    if (p) {
        cout << "Processing emergency case for patient: " << p->name << "\n";
        // Additional emergency processing
    } else {
        cout << "Warning: Patient in emergency queue not found in records!\n";
    }

    delete temp;
}

// Main Menu
void displayMainMenu() {
    cout << "\nHospital Management System\n";
    cout << "1. Patient Management\n";
    cout << "2. Doctor Management\n";
    cout << "3. Appointment Management\n";
    cout << "4. Room Management\n";
    cout << "5. Medicine Management\n";
    cout << "6. Emergency Handling\n";
    cout << "0. Exit\n";
    cout << "Enter your choice: ";
}

void patientManagementMenu() {
    int choice;
    do {
        cout << "\nPatient Management\n";
        cout << "1. Add New Patient\n";
        cout << "2. View Patient Details\n";
        cout << "3. Update Patient Details\n";
        cout << "4. Delete Patient\n";
        cout << "5. Assign Doctor to Patient\n";
        cout << "6. Admit Patient\n";
        cout << "7. Discharge Patient\n";
        cout << "0. Back to Main Menu\n";
        cout << "Enter your choice: ";
        cin >> choice;

        PatientNode* p;
        DoctorNode* d;
        switch (choice) {
            case 1:
                addPatient();
                break;
            case 2:
                p = searchPatient();
                if (p) viewPatient(p->id);
                break;
            case 3:
                p = searchPatient();
                if (p) updatePatient(p->id);
                break;
            case 4:
                p = searchPatient();
                if (p) deletePatient(p->id);
                break;
            case 5:
                p = searchPatient();
                if (!p) break;
                d = searchDoctorByNameWithDisambiguation();
                if (!d) break;
                assignDoctorToPatient(p->id, d->id);
                break;
            case 6:
                p = searchPatient();
                if (p) admitPatient(p->id);
                break;
            case 7:
                p = searchPatient();
                if (p) dischargePatient(p->id);
                break;
            case 0:
                break;
            default:
                cout << "Invalid choice!\n";
        }
    } while (choice != 0);
}

void doctorManagementMenu() {
    int choice;
    do {
        cout << "\nDoctor Management\n";
        cout << "1. Add New Doctor\n";
        cout << "2. View Doctor Details\n";
        cout << "3. Update Doctor Details\n";
        cout << "4. Delete Doctor\n";
        cout << "5. View Doctors by Department\n";
        cout << "0. Back to Main Menu\n";
        cout << "Enter your choice: ";
        cin >> choice;

        DoctorNode* d;
        char dept[50];
        switch (choice) {
            case 1:
                addDoctor();
                break;
            case 2:
                d = searchDoctorByNameWithDisambiguation();
                if (d) viewDoctor(d->id);
                break;
            case 3:
                d = searchDoctorByNameWithDisambiguation();
                if (d) updateDoctor(d->id);
                break;
            case 4:
                d = searchDoctorByNameWithDisambiguation();
                if (d) deleteDoctor(d->id);
                break;
            case 5:
                cout << "Enter department: ";
                cin.ignore();
                cin.getline(dept, 50);
                viewDoctorsByDepartment(dept);
                break;
            case 0:
                break;
            default:
                cout << "Invalid choice!\n";
        }
    } while (choice != 0);
}

void appointmentManagementMenu() {
    int choice;
    do {
        cout << "\nAppointment Management\n";
        cout << "1. Schedule Appointment\n";
        cout << "2. View Appointment Details\n";
        cout << "3. Cancel Appointment\n";
        cout << "4. View Appointments by Patient Name\n";
        cout << "5. View Appointments by Doctor Name\n";
        cout << "6. Update Appointment Status\n";
        cout << "0. Back to Main Menu\n";
        cout << "Enter your choice: ";
        cin >> choice;

        int id, status;
        switch (choice) {
            case 1:
                addAppointment(-1, -1); // IDs will be set in the function
                break;
            case 2:
                cout << "Enter appointment ID: ";
                cin >> id;
                viewAppointment(id);
                break;
            case 3:
                cout << "Enter appointment ID: ";
                cin >> id;
                cancelAppointment(id);
                break;
            case 4:
                viewAppointmentsByPatientName();
                break;
            case 5:
                viewAppointmentsByDoctorName();
                break;
            case 6:
                cout << "Enter appointment ID: ";
                cin >> id;
                cout << "Enter new status (0-Scheduled, 1-Completed, 2-Cancelled, 3-No-show): ";
                cin >> status;
                updateAppointmentStatus(id, status);
                break;
            case 0:
                break;
            default:
                cout << "Invalid choice!\n";
        }
    } while (choice != 0);
}

void roomManagementMenu() {
    int choice;
    do {
        cout << "\nRoom Management\n";
        cout << "1. View Room Status\n";
        cout << "0. Back to Main Menu\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                viewRoomStatus();
                break;
            case 0:
                break;
            default:
                cout << "Invalid choice!\n";
        }
    } while (choice != 0);
}

void medicineManagementMenu() {
    int choice;
    do {
        cout << "\nMedicine Management\n";
        cout << "1. Add New Medicine\n";
        cout << "2. Search Medicine by Name\n";
        cout << "3. Search Medicine by Code\n";
        cout << "4. View All Medicines in Stock\n";
        cout << "5. Remove Expired Medicines\n";
        cout << "0. Back to Main Menu\n";
        cout << "Enter your choice: ";
        cin >> choice;

        char searchTerm[50];
        switch (choice) {
            case 1:
                addMedicine();
                break;
            case 2:
                cout << "Enter medicine name: ";
                cin.ignore();
                cin.getline(searchTerm, 50);
                searchMedicineByName(searchTerm);
                break;
            case 3:
                cout << "Enter medicine code: ";
                cin.ignore();
                cin.getline(searchTerm, 20);
                searchMedicineByCode(searchTerm);
                break;
            case 4:
                viewAllMedicines();
                break;
            case 5:
                removeExpiredMedicines();
                break;
            case 0:
                break;
            default:
                cout << "Invalid choice!\n";
        }
    } while (choice != 0);
}

void emergencyMenu() {
    int choice;
    do {
        cout << "\nEmergency Handling\n";
        cout << "1. Admit Emergency Patient\n";
        cout << "2. View Emergency Patients\n";
        cout << "3. Process Next Emergency\n";
        cout << "0. Back to Main Menu\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                admitEmergencyPatient();
                break;
            case 2:
                viewEmergencyPatients();
                break;
            case 3:
                processNextEmergency();
                break;
            case 0:
                break;
            default:
                cout << "Invalid choice!\n";
        }
    } while (choice != 0);
}

// Cleanup functions
void deletePatientBST(PatientBSTNode* root) {
    if (root == nullptr) return;
    deletePatientBST(root->left);
    deletePatientBST(root->right);
    delete root;
}

void deleteDoctorBST(DoctorBSTNode* root) {
    if (root == nullptr) return;
    deleteDoctorBST(root->left);
    deleteDoctorBST(root->right);
    delete root;
}

void deleteMedicineBST(MedicineBSTNode* root) {
    if (root == nullptr) return;
    deleteMedicineBST(root->left);
    deleteMedicineBST(root->right);
    delete root;
}

void loadAllData() {
    loadPatientsFromFile();
    loadDoctorsFromFile();
    loadMedicinesFromFile();
    loadAppointmentsFromFile();
    loadRoomsFromFile();
    loadEmergencyQueueFromFile();
}

int main() {
    loadAllData();
    initializeDefaultDoctors();
    initializeRooms();

    int choice;
    do {
        displayMainMenu();
        cin >> choice;

        switch (choice) {
            case 1:
                patientManagementMenu();
                break;
            case 2:
                doctorManagementMenu();
                break;
            case 3:
                appointmentManagementMenu();
                break;
            case 4:
                roomManagementMenu();
                break;
            case 5:
                medicineManagementMenu();
                break;
            case 6:
                emergencyMenu();
                break;
            case 0:
                cout << "Exiting system...\n";
                break;
            default:
                cout << "Invalid choice!\n";
        }
    } while (choice != 0);

    // Clean up memory
    // Patients
    PatientNode* currentPatient = patientList;
    while (currentPatient != nullptr) {
        PatientNode* next = currentPatient->next;
        delete currentPatient;
        currentPatient = next;
    }
    deletePatientBST(patientBST);

    // Doctors
    DoctorNode* currentDoctor = doctorList;
    while (currentDoctor != nullptr) {
        DoctorNode* next = currentDoctor->next;
        delete currentDoctor;
        currentDoctor = next;
    }
    deleteDoctorBST(doctorBST);

    // Appointments
    AppointmentNode* currentAppointment = appointmentList;
    while (currentAppointment != nullptr) {
        AppointmentNode* next = currentAppointment->next;
        delete currentAppointment;
        currentAppointment = next;
    }

    // Medicines
    MedicineNode* currentMedicine = medicineList;
    while (currentMedicine != nullptr) {
        MedicineNode* next = currentMedicine->next;
        delete currentMedicine;
        currentMedicine = next;
    }
    deleteMedicineBST(medicineBST);

    // Emergency queue
    EmergencyNode* currentEmergency = emergencyQueue;
    while (currentEmergency != nullptr) {
        EmergencyNode* next = currentEmergency->next;
        delete currentEmergency;
        currentEmergency = next;
    }

    // Clean up hash tables
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        HashNode* current = patientHashTable[i];
        while (current != nullptr) {
            HashNode* next = current->next;
            delete current;
            current = next;
        }

        current = doctorHashTable[i];
        while (current != nullptr) {
            HashNode* next = current->next;
            delete current;
            current = next;
        }

        current = medicineHashTable[i];
        while (current != nullptr) {
            HashNode* next = current->next;
            delete current;
            current = next;
        }
    }

    return 0;
}
