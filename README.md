# Budget Planner

`Budget Planner` is a layered C++ desktop application for managing personal finances with a native macOS window. It combines expense tracking, salary planning, recurring bills, savings automation, long-term goals, monthly charts, and persistent local storage in a single application.

The project started as a console-based expense tracker and was extended into a full desktop budgeting tool with a cleaner architecture, richer business logic, file persistence, test coverage, and a resizable user interface.

## What The Application Does

The app helps a user manage monthly money flow in a practical way:

- add, update, and delete expenses
- review expenses for the currently selected month
- open past months from the same interface
- set and update the monthly salary
- set an automatic monthly savings amount
- transfer extra money into savings manually
- configure recurring monthly bills such as rent, electricity, gas, internet, or phone
- calculate how much money is still available in the current month
- estimate how many months are needed to afford a target amount
- create savings goals that remain saved across runs
- notify the user when a savings goal has been reached
- display monthly spending in a chart
- show small saving suggestions based on spending patterns
- keep all saved data between runs of the application

## Main Features

### 1. Expense Management

Users can add a new expense with:

- amount
- category
- date in `YYYY-MM-DD`
- note / description

Existing expenses from the selected month can also be:

- updated
- deleted

Selecting a row in the expenses table loads that expense back into the form so the user can edit it directly.

### 2. Monthly Budget Tracking

The app stores the monthly financial state and updates it automatically:

- salary
- available money
- total savings
- monthly automatic savings amount
- recurring monthly bills

When the month changes:

- the tracked month resets
- the configured monthly savings contribution is carried into total savings
- the monthly available amount is recalculated

### 3. Recurring Bills

Recurring bills are stored separately from normal expenses and are applied in the monthly availability calculation. This is useful for fixed monthly costs such as:

- rent
- electricity
- gas
- internet
- phone

Users can add new recurring bills or remove existing ones from the desktop interface.

### 4. Savings Goals

The app supports named savings goals, for example:

- emergency fund
- laptop
- vacation

For each goal, the app stores:

- goal name
- target amount
- whether the user has already been notified that the goal was reached

When the savings balance becomes greater than or equal to a goal target, the app shows a notification and marks that goal as already announced so the message does not repeat on every launch.

### 5. Monthly Analytics

The application includes several useful budgeting insights:

- total spent in the selected month
- total spent in the year
- average monthly spending
- safe amount to spend per day
- estimated months needed for a goal
- spending distribution by month through a chart

### 6. Persistent Local Storage

The app stores its data in the local `data/` folder:

- expenses are saved in `data/expense.csv`
- budget state, recurring bills, savings values, and goals are saved in `data/budget_state.txt`

That means data remains available every time the program is launched again.

## User Interface

The current UI is a native macOS desktop window built with AppKit and Objective-C++.

It contains:

- a top area with summary cards
- an expense editor
- salary and savings controls
- recurring bill controls
- savings goal controls
- a monthly chart
- a saving ideas panel
- an expenses table for the selected month

The window is:

- resizable
- scrollable when the height becomes too small
- able to show older months from the same window

## Architecture

The application follows a layered architecture:

`Domain -> Repository -> Service -> UI`

### Domain Layer

The domain layer contains the core models:

- `Expense`
- `BudgetState`
- `SavingsGoal`

Responsibilities:

- represent business data
- keep financial state structures clear and reusable

### Repository Layer

Repositories handle storage and retrieval:

- `InMemoryExpenseRepository`
- `FileExpenseRepository`
- `FileBudgetStateRepository`
- repository interfaces for abstraction

Responsibilities:

- store expenses
- load and save the budget state
- abstract persistence away from the business logic

### Service Layer

The service layer contains the main budgeting logic:

- expense CRUD
- budget recalculation
- salary updates
- savings transfers
- fixed bill management
- goal management
- monthly and yearly totals
- affordability estimation

This is the most important layer for application behavior.

### UI Layer

There are currently two UI paths in the repository:

- the old console UI
- the new desktop AppKit UI

The desktop app is the main intended interface now.

### Validation Layer

Validation ensures data quality for:

- ids
- amounts
- dates
- descriptions

## Project Structure

```text
include/
  app/
  domain/
  repo/
  service/
  ui/
  validation/

src/
  app/
  domain/
  repo/
  service/
  ui/
  validation/

tests/
data/
```

## Important Files

### App Entry And Desktop UI

- `src/app/main.mm`
- `src/app/AppSupport.cpp`
- `include/app/AppSupport.h`

These files define the native macOS application window, layout, chart support, demo seed data, and month-based helpers.

### Domain

- `include/domain/Expense.h`
- `src/domain/Expense.cpp`
- `include/domain/BudgetState.h`
- `src/domain/BudgetState.cpp`

### Repositories

- `include/repo/IExpenseRepository.h`
- `include/repo/IBudgetStateRepository.h`
- `include/repo/InMemoryExpenseRepository.h`
- `include/repo/FileExpenseRepository.h`
- `include/repo/FileBudgetStateRepository.h`

### Services

- `include/service/IExpenseService.h`
- `include/service/ExpenseService.h`
- `src/service/ExpenseService.cpp`

### Validation

- `include/validation/Validators.h`
- `src/validation/Validators.cpp`

## Data Flow

Typical flow when adding an expense:

1. the user enters amount, category, date, and note in the desktop form
2. the UI sends the request to `ExpenseService`
3. validation checks the fields
4. the expense repository stores the new expense
5. the budget state is recalculated if the expense belongs to the tracked month
6. the UI refreshes summary cards, the chart, and the monthly expenses table

Typical flow when saving a salary or recurring bill:

1. the user changes the value in the UI
2. the service updates `BudgetState`
3. the new state is saved through `FileBudgetStateRepository`
4. the app recalculates availability and refreshes the dashboard

## Demo Data

On first run, if no saved data exists yet, the app seeds demo content automatically:

- a sample salary
- a sample monthly savings amount
- sample recurring bills
- sample savings goals
- expenses across multiple recent months

This makes the interface useful immediately, even before the user adds real data.

## Build Requirements

- macOS
- CMake 3.20 or newer
- AppleClang / Xcode command line tools

The project uses:

- `C++20`
- `Objective-C++`
- AppKit / Foundation frameworks

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
open build/ExpenseTracker_BudgetApp.app
```

If you build in the provided workspace output directory, the app is also available here:

```bash
open cmake-build/ExpenseTracker_BudgetApp.app
```

## Run Tests

```bash
ctest --test-dir build --output-on-failure
```

or for the current workspace build directory:

```bash
ctest --test-dir cmake-build --output-on-failure
```

## Testing Coverage

The project includes tests for:

- in-memory repository behavior
- service-layer budgeting logic
- monthly savings behavior
- recurring bill behavior
- delete / update expense effects on available money
- month reset behavior

## Current Technical Notes

- the desktop app is the primary interface
- the old console code is still present in the repository, but the bundle app is the main entry point
- the current version is designed for macOS because the window uses AppKit
- the layout is compact and scrollable for smaller window sizes

## Possible Future Improvements

- inline editing for recurring bills and goals
- better chart interactions
- category-based pie or bar analysis
- export/import helpers
- notifications through native macOS notification center
- more adaptive layouts for very narrow windows

## Author

Teo Argint  
Mathematics and Computer Science Student
