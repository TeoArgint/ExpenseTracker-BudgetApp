# Expense Tracker – Layered C++ Application

A C++20 console application implementing a clean layered architecture for managing personal expenses.

The project demonstrates object-oriented design principles, separation of concerns, repository pattern implementation, file persistence and unit testing.

---

## 🔎 Overview

This application allows users to manage expenses using a structured architecture:

- Add new expenses
- View all stored expenses
- Persist data using CSV file storage
- Validate user input
- Execute unit tests for core logic

The project emphasizes clean architecture and modular design.

---

## 🏗 Architecture

The application follows a layered architecture pattern:

Domain  
→ Repository  
→ Service  
→ UI  

### 📁 Project Structure

include/
  domain/
  repo/
  service/
  ui/
  validation/

src/
  domain/
  repo/
  service/
  ui/
  validation/

tests/

---

### 🧱 Layers Explained

**Domain Layer**
- Defines the `Expense` entity
- Encapsulates core data structure

**Repository Layer**
- `InMemoryExpenseRepository` (runtime storage)
- `FileExpenseRepository` (CSV persistence)
- Implements Repository Pattern

**Service Layer**
- Contains business logic
- Acts as mediator between UI and repository

**UI Layer**
- Console-based user interface
- Handles user interaction

**Validation Layer**
- Input validation logic
- Prevents invalid data insertion

---

## ⚙️ Technical Highlights

- C++20
- STL usage
- Layered Architecture
- Repository Pattern
- CSV File I/O
- CMake build system
- Modular project organization
- Basic unit testing structure

---

## 🛠 Build Instructions

cmake -S . -B build
cmake --build build

---

## ▶ Run Application

./build/ExpenseTracker_BudgetApp

---

## 🧪 Run Tests

If tests are integrated into the main target:

./build/ExpenseTracker_BudgetApp

If a separate test executable is configured:

./build/tests

---

## 🎯 Learning Objectives

This project was developed to strengthen understanding of:

- Object-Oriented Programming in C++
- Clean software architecture
- Design patterns (Repository Pattern)
- File persistence mechanisms
- Modular project organization
- Unit testing concepts

---

## 👤 Author

Teo Argint  
Mathematics and Computer Science Student
