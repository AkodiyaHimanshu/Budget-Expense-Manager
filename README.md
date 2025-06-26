# 💰 Budget & Expense Manager

A command-line application for managing your personal finances through transactions, budgets, reports, and user profiles. Designed with modular UI and service layers to keep your data organized and secure.

---

## 📦 Features

### ✅ Transaction Management
- View all transactions
- Filter by category, type, date range, amount, or month
- Add new transactions
- Monthly transaction summary

### 💼 Budget Management
- Set and manage budgets per category
- Filter budgets by month or category
- View budget usage reports
- Update or remove existing budgets

### 📊 Financial Reports
- Monthly spending summaries
- Budget utilization analytics

### 👤 User Profile Management
- Create, select, and manage multiple user profiles
- Update profile display name
- Delete or list profiles

---

## 🚀 Installation

### Prerequisites
- C++17 compatible compiler (e.g., `g++`, `clang++`)
- CMake 3.10+ (if using build system)
- Linux, macOS, or Windows

### Steps

```bash
# Clone the repository
git clone https://github.com/your-username/budget-manager.git
cd budget-manager

# Build the project (simple g++ approach)
g++ -std=c++17 -o budget src/main.cpp -Iinclude

# Or use CMake (if CMakeLists.txt is provided)
mkdir build && cd build
cmake ..
make
```

---

## 🧪 Running the Application

### Default Interactive Mode
```bash
./budget
```

### Help Mode
```bash
./budget --help
# or
./budget -h
```

---

## ⚙️ Command Line Options

| Option     | Description                        |
|------------|------------------------------------|
| `--help`   | Display help message and exit      |
| `-h`       | Alias for `--help`                 |

---

## 🧭 Menu Navigation Guide

```
===== Budget & Expense Manager =====
1. Transaction Management
2. Budget Management
3. Financial Reports
4. User Profile Management
0. Exit
```

Each section leads to sub-options like viewing, adding, updating data.

---

## 🔧 Build Configuration Notes

- All model/service/UI components are modular
- Shared pointers manage memory safely
- Flags handled with `unordered_map` for O(1) dispatch

---

## 📣 Contributions

PRs are welcome! Please follow the coding standards.