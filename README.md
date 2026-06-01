# 🏥 Hospital Management System

> A comprehensive, console-based Hospital Management System built in C++ that demonstrates core **Data Structures & Algorithms** concepts in a real-world application.

---

## 📋 Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Data Structures & Algorithms Used](#data-structures--algorithms-used)
- [Project Structure](#project-structure)
- [Getting Started](#getting-started)
- [System Modules](#system-modules)
- [Data Persistence](#data-persistence)
- [Constraints & Limits](#constraints--limits)
- [Screenshots](#screenshots)
- [Author](#author)

---

## Overview

This project is a fully functional Hospital Management System developed as part of a Data Structures & Algorithms course. It manages patients, doctors, appointments, medicines, hospital rooms, and emergency cases — all using hand-implemented data structures (no STL containers for core logic).

The system runs entirely in the terminal and persists all data to flat text files so records survive between sessions.

---

## Features

### 👤 Patient Management
- Register new patients with full profile (name, age, gender, contact, address, medical history)
- Assign patients to doctors and rooms
- Search patients by **ID** (O(1) via hash table) or **name** (O(log n) via BST)
- View all admitted and non-admitted patients
- Update patient records and discharge patients

### 🩺 Doctor Management
- Add doctors with department, specialization, and contact details
- Choose from 10 built-in medical specializations (Cardiology, Neurology, Orthopedics, etc.)
- Manage weekly availability schedules (7 days × 24 hours)
- Search doctors by ID or name
- View all doctors by department

### 📅 Appointment Scheduling
- Schedule appointments with date, time, and purpose
- Appointments are **sorted chronologically** using Quicksort
- Track appointment status: Scheduled / Completed / Cancelled / No-show
- View upcoming and past appointments per patient or doctor

### 💊 Medicine / Pharmacy Management
- Add medicines with name, code, quantity, price, and expiry date
- Search medicines by name (BST) or ID (hash table)
- Track stock levels and expiry dates
- Update quantities and pricing

### 🛏️ Room Management
- Manage up to 200 hospital rooms
- Assign and vacate rooms for admitted patients
- View real-time room availability

### 🚨 Emergency Queue
- Priority-based emergency queue (lower number = higher priority)
- Enqueue critical patients with a priority level
- Dequeue and process the highest-priority case first
- Emergency records persist to file between sessions

---

## Data Structures & Algorithms Used

| Data Structure / Algorithm | Where It's Applied |
|---|---|
| **Singly Linked Lists** | Patient list, Doctor list, Appointment list, Medicine list, Emergency queue nodes |
| **Binary Search Trees (BST)** | Name-based search for Patients, Doctors, and Medicines |
| **Hash Tables** (chaining) | O(1) ID-based lookup for Patients, Doctors, and Medicines |
| **Priority Queue** (sorted linked list) | Emergency case management — sorted by priority level |
| **Quicksort** | Sorting appointments chronologically by date and time |
| **File I/O (CSV format)** | Persistent storage across sessions for all entities |
| **Struct-based OOP** | Modular data modelling using C++ structs |

---

## Project Structure

```
Hospital-Management-System--DataStructures-Algorithms/
│
├── ManagementSystem.cpp      # Full source code (~2,750 lines)
│
├── Screenshots/              # UI screenshots of the running system
│
├── patients.txt              # Auto-generated patient records (CSV)
├── doctors.txt               # Auto-generated doctor records (CSV)
├── appointments.txt          # Auto-generated appointment records (CSV)
├── medicines.txt             # Auto-generated medicine records (CSV)
├── rooms.txt                 # Auto-generated room records (CSV)
└── emergency.txt             # Auto-generated emergency queue (CSV)
```

> **Note:** The `.txt` data files are created automatically at runtime. You do not need to create them manually.

---

## Getting Started

### Prerequisites

- A C++ compiler supporting **C++11 or later** (e.g., `g++`, `clang++`, MSVC)
- Terminal / Command Prompt

### Compile

```bash
g++ -std=c++11 -o HospitalSystem ManagementSystem.cpp
```

### Run

```bash
# Linux / macOS
./HospitalSystem

# Windows
HospitalSystem.exe
```

### Notes
- All data files (`patients.txt`, `doctors.txt`, etc.) will be created in the **same directory** as the executable on first run.
- Subsequent runs will automatically load existing data from these files.

---

## System Modules

### Main Menu Options

```
1. Patient Management
2. Doctor Management
3. Appointment Management
4. Medicine Management
5. Room Management
6. Emergency Management
0. Exit
```

### Input Validation
The system enforces validation throughout:
- **Names** — alphabetic characters and spaces only
- **Age** — must be between 1 and 119
- **Gender** — M / F / J
- **Phone numbers** — exactly 11 digits
- **Dates** — validated including leap year logic
- **Times** — valid 24-hour format (00:00 – 23:59)

---

## Data Persistence

All records are stored in **comma-separated (CSV) text files**:

| File | Contents |
|---|---|
| `patients.txt` | ID, name, age, gender, contact, address, medical history, assigned doctor, room, admission status |
| `doctors.txt` | ID, name, department, specialty, contact, 7×24 availability matrix |
| `appointments.txt` | ID, patient ID, doctor ID, date, time, purpose, status |
| `medicines.txt` | ID, name, code, quantity, expiry date, price |
| `rooms.txt` | Room number, occupancy status, patient ID |
| `emergency.txt` | Patient ID, priority level |

Data is loaded into memory on startup and written back on every modification.

---

## Constraints & Limits

| Entity | Maximum |
|---|---|
| Patients | 1,000 |
| Doctors | 100 |
| Appointments | 1,000 |
| Medicines | 500 |
| Rooms | 200 |
| Hash Table Size | 128 buckets |

---

## Screenshots

Screenshots of the running system are available in the Screenshots folder of this repository.

---

## Author

**Zarwa Ayaz**
- GitHub: [@ZarwaAyaz](https://github.com/ZarwaAyaz)

---

> *This project was built for educational purposes as a demonstration of Data Structures & Algorithms applied to a real-world domain.*
