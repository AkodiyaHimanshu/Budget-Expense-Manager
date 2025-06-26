# 🧾 CLI Command Reference – Budget & Expense Manager

This document provides a complete reference for all CLI options and features available in the application.

---

## 🧭 Global Flags

| Flag         | Description                            |
|--------------|----------------------------------------|
| `--help`     | Display detailed usage information     |
| `-h`         | Alias for `--help`                     |
| `--version`  | Print application version              |
| `-v`         | Alias for `--version`                  |

---

## 🧩 Main Menu Commands

### 1. Transaction Management
```text
1. View All Transactions
2. View Transactions By Category
3. View Transactions By Type (Income/Expense)
4. View Transactions By Date Range
5. View Transactions By Amount Range
6. View Transactions By Month
7. View Monthly Summary
8. Add New Transaction
0. Back to Main Menu
```

---

### 2. Budget Management
```text
1. View All Budgets
2. View Budgets By Category
3. View Budgets By Month
4. Set New Budget
5. Update Budget
6. Remove Budget
7. View Budget Usage Report
0. Back to Main Menu
```

---

### 3. Financial Reports
```text
1. Monthly Summary
2. Budget Utilization Report
0. Back to Main Menu
```

---

### 4. User Profile Management
```text
1. Create Profile
2. Select Profile
3. View Profile Info
4. Update Profile Display Name
5. Delete Profile
6. List All Profiles
0. Back to Main Menu
```

---

## 🛠️ Examples

### Run in interactive mode:
```bash
./budget
```

### Show help menu:
```bash
./budget --help
```

### Show version:
```bash
./budget --version
```

---

## 🧪 Notes

- All operations are available through an interactive menu.
- You must create or select a profile before accessing transactions or budgets.
- Flags are only evaluated at program start. If no flags are passed, the interactive menu is shown.
