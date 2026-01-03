# Bank Account Management & Warehouse Inventory System

**Object-Oriented Programming & Data Structures Assignment**  
Course: CCP6124 | Trimester 2520 | Multimedia University

## Overview

This project implements two comprehensive C++ systems demonstrating advanced OOP concepts, custom data structures, and professional software engineering practices.

### 1. Bank Account Management System
A complete banking solution using inheritance hierarchy, smart pointers, and singly linked lists.

**Key Features:**
- Three account types: Basic, Savings (with interest), Checking (with overdraft)
- Polymorphic account operations with runtime dispatch
- Three-tier error handling with immediate retry capability
- File persistence with automatic save/load across sessions
- Smart pointer memory management (unique_ptr)

### 2. Warehouse Inventory & Shipping System
Custom template-based Stack and Queue implementations modeling real warehouse workflow.

**Key Features:**
- Custom Stack (LIFO) for inventory management
- Custom Queue (FIFO) for shipping operations
- Template-based design for type safety and reusability
- File persistence maintaining structure ordering
- Real-world workflow: Incoming → Processing → Shipping

## Technologies Used

- **Language:** C++ (C++11/14)
- **Concepts:** OOP, Inheritance, Polymorphism, Templates, Smart Pointers
- **Data Structures:** Singly Linked List, Stack, Queue
- **Features:** File I/O, Exception Handling, Memory Management

## How to Compile & Run

### Bank Account System (Q1)
```bash
g++ Q1.cpp -o bank_system -std=c++11
./bank_system
```

### Warehouse System (Q2)
```bash
g++ Q2.cpp -o warehouse_system -std=c++11
./warehouse_system
```

## Project Structure
```
├── Q1.cpp                      # Bank Account Management System
├── Q2.cpp                      # Warehouse Inventory System
├── bank_accounts.txt           # Persistent account data
├── warehouse_inventory.txt     # Persistent inventory data
├── warehouse_shipping.txt      # Persistent shipping queue data
└── README.md
```

## Design Highlights

### Bank System Architecture
- **Inheritance Hierarchy:** Base Account → Savings/Checking/Basic
- **Hybrid Memory Management:** Smart pointers for objects + raw pointers for list navigation
- **Error Recovery:** User-friendly retry mechanism without menu disruption
- **Polymorphic Operations:** Runtime dispatch for account-specific behaviors

### Warehouse System Architecture
- **Template Design:** Type-safe generic Stack and Queue classes
- **LIFO/FIFO:** Perfect demonstration of stack and queue behaviors
- **Order Preservation:** File persistence maintains exact structure ordering
- **Workflow Modeling:** Real inventory management process simulation

## Key Accomplishments

- Complete OOP implementation with inheritance and polymorphism
- Custom data structures built from scratch (no STL stack/queue)
- Professional error handling with context-aware messages
- Robust file persistence with automatic session management
- Clean, maintainable, and extensible code architecture
- O(1) time complexity for all stack and queue operations

## Team Members

- **Eba Mohamed Abbas Ahmed** (Leader) - 242UC243BE
- Lama M. R. Siam - 242UC243B4
- Arina Aghaee - 1221303277
- Merey Abilkhan - 241UT24016

## Course Information

**Course:** CCP6124 - Object-Oriented Programming and Data Structures  
**Institution:** Multimedia University (MMU)  
**Tutorial Section:** TT1L  
**Group:** 4  
**Trimester:** 2520

## Usage Notes

**Important:** Always exit programs properly (Option 8 for Q1, Option 8 for Q2) to ensure data is saved to files.

## Documentation

For detailed technical documentation, implementation details, and screenshots, refer to the assignment report PDF included in this repository.
