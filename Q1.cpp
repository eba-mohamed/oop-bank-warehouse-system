#include <iostream>
#include <string>
#include <limits>
#include <memory>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

class Account {
protected:
    string accountNumber, customerName;
    double balance;

public:
    Account(const string& accNum = "", const string& custName = "", double initialBalance = 0.0)
        : accountNumber(accNum), customerName(custName), balance(initialBalance) {}

    virtual ~Account() = default;

    // Getters
    string getAccountNumber() const { return accountNumber; }
    string getCustomerName() const { return customerName; }
    double getBalance() const { return balance; }

    // Setters
    void setBalance(double newBalance) { balance = newBalance; }

    virtual void displayDetails() const {
        cout << "Account Type: " << getAccountType() << "\nAccount Number: " << accountNumber
             << "\nCustomer Name: " << customerName << "\nBalance: $" << balance << endl;
    }

    virtual string getAccountType() const { return "Basic Account"; }
    virtual bool canWithdraw(double amount) const { return amount <= balance; }
    virtual double getAvailableBalance() const { return balance; }
    virtual void showSpecialFeatures() const { cout << "No special features for basic account." << endl; }
};

class SavingsAccount : public Account {
private:
    double interestRate;

public:
    SavingsAccount(const string& accNum = "", const string& custName = "", double initialBalance = 0.0, double rate = 2.5)
        : Account(accNum, custName, initialBalance), interestRate(rate) {}

    double getInterestRate() const { return interestRate; }

    void displayDetails() const override {
        Account::displayDetails();
        cout << "Interest Rate: " << interestRate << "%" << endl;
    }

    string getAccountType() const override { return "Savings Account"; }

    void showSpecialFeatures() const override {
        cout << "Annual interest on current balance: $" << balance * (interestRate / 100)
             << "\nInterest Rate: " << interestRate << "%" << endl;
    }
};

class CheckingAccount : public Account {
private:
    double overdraftLimit;

public:
    CheckingAccount(const string& accNum = "", const string& custName = "", double initialBalance = 0.0, double overdraft = 500.0)
        : Account(accNum, custName, initialBalance), overdraftLimit(overdraft) {}

    double getOverdraftLimit() const { return overdraftLimit; }

    void displayDetails() const override {
        Account::displayDetails();
        cout << "Overdraft Limit: $" << overdraftLimit << endl;
    }

    string getAccountType() const override { return "Checking Account"; }
    bool canWithdraw(double amount) const override { return amount <= (balance + overdraftLimit); }
    double getAvailableBalance() const override { return balance + overdraftLimit; }

    void showSpecialFeatures() const override {
        cout << "Overdraft Limit: $" << overdraftLimit
             << "\nAvailable Balance (including overdraft): $" << getAvailableBalance() << endl;
    }
};

struct Node {
    unique_ptr<Account> account;
    Node* next;
    Node(unique_ptr<Account> acc) : account(std::move(acc)), next(nullptr) {}
};

class BankSystem {
private:
    Node* head = nullptr;
    const string DATA_FILE = "./bank_accounts.txt";

    bool isValid(const string& str, bool isName) const {
        if (str.empty() || str.length() < (isName ? 2 : 3)) {
            cout << "Error: " << (isName ? "Name" : "Account number")
                 << " must be at least " << (isName ? 2 : 3) << " characters long." << endl;
            return false;
        }

        bool valid;
        if (isName) {
            valid = all_of(str.begin(), str.end(), [](char c) { return isalpha(c) || c == ' '; });
        } else {
            valid = all_of(str.begin(), str.end(), [](char c) { return isdigit(c); });
        }

        if (!valid) {
            cout << "Error: " << (isName ? "Name can only contain letters and spaces"
                                        : "Account number can only contain numbers") << endl;
            return false;
        }

        if (isName && !any_of(str.begin(), str.end(), [](char c) { return isalpha(c); })) {
            cout << "Error: Name must contain at least one letter." << endl;
            return false;
        }

        return true;
    }

public:
    BankSystem() { loadAccountsFromFile(); }
    BankSystem(const BankSystem&) = delete;
    BankSystem& operator=(const BankSystem&) = delete;

    ~BankSystem() {
        saveAccountsToFile();
        while (head) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
    }

    void saveAccountsToFile() {
        ofstream outFile(DATA_FILE);
        if (!outFile) {
            cout << "Error: Could not save to file." << endl;
            return;
        }

        int count = 0;
        for (Node* current = head; current; current = current->next) {
            Account* acc = current->account.get();
            outFile << "TYPE:" << acc->getAccountType() << "\nNUMBER:" << acc->getAccountNumber()
                    << "\nNAME:" << acc->getCustomerName() << "\nBALANCE:" << acc->getBalance() << "\n";

            if (auto* savings = dynamic_cast<SavingsAccount*>(acc))
                outFile << "INTEREST_RATE:" << savings->getInterestRate() << "\n";
            else if (auto* checking = dynamic_cast<CheckingAccount*>(acc))
                outFile << "OVERDRAFT_LIMIT:" << checking->getOverdraftLimit() << "\n";

            outFile << "----------\n";
            count++;
        }
        outFile.close();
        cout << count << " accounts saved." << endl;
    }

    void loadAccountsFromFile() {
        ifstream inFile(DATA_FILE);
        if (!inFile) {
            cout << "No existing data found. Starting fresh." << endl;
            return;
        }

        string line, type, number, name;
        double balance, rate, limit;
        int count = 0;

        while (getline(inFile, line) && line.substr(0, 5) == "TYPE:") {
            type = line.substr(5);

            if (getline(inFile, line) && line.substr(0, 7) == "NUMBER:")
                number = line.substr(7);
            if (getline(inFile, line) && line.substr(0, 5) == "NAME:")
                name = line.substr(5);
            if (getline(inFile, line) && line.substr(0, 8) == "BALANCE:")
                balance = stod(line.substr(8));

            unique_ptr<Account> acc;
            if (type == "Basic Account") {
                acc = make_unique<Account>(number, name, balance);
            } else if (type == "Savings Account") {
                if (getline(inFile, line) && line.substr(0, 14) == "INTEREST_RATE:")
                    rate = stod(line.substr(14));
                acc = make_unique<SavingsAccount>(number, name, balance, rate);
            } else if (type == "Checking Account") {
                if (getline(inFile, line) && line.substr(0, 16) == "OVERDRAFT_LIMIT:")
                    limit = stod(line.substr(16));
                acc = make_unique<CheckingAccount>(number, name, balance, limit);
            }

            if (acc) {
                addAccount(std::move(acc));
                count++;
            }
            getline(inFile, line); // Skip separator
        }
        cout << count << " accounts loaded." << endl;
    }

    bool addAccount(unique_ptr<Account> newAccount) {
        if (searchByAccountNumber(newAccount->getAccountNumber())) {
            cout << "Error: Account already exists." << endl;
            return false;
        }

        Node* newNode = new Node(std::move(newAccount));
        if (!head) {
            head = newNode;
        } else {
            Node* current = head;
            while (current->next) current = current->next;
            current->next = newNode;
        }
        return true;
    }

    Account* searchByAccountNumber(const string& accNum) {
        for (Node* current = head; current; current = current->next) {
            if (current->account->getAccountNumber() == accNum)
                return current->account.get();
        }
        return nullptr;
    }

    void displayAllAccounts() const {
        if (!head) {
            cout << "No accounts in the system." << endl;
            return;
        }
        int count = 1;
        for (Node* current = head; current; current = current->next) {
            cout << "\n--- Account " << count++ << " ---" << endl;
            current->account->displayDetails();
        }
    }

    bool deleteAccount(const string& accNum) {
        if (!head) return false;

        if (head->account->getAccountNumber() == accNum) {
            Node* temp = head;
            head = head->next;
            delete temp;
            cout << "Account deleted successfully." << endl;
            return true;
        }

        for (Node* current = head; current->next; current = current->next) {
            if (current->next->account->getAccountNumber() == accNum) {
                Node* temp = current->next;
                current->next = temp->next;
                delete temp;
                cout << "Account deleted successfully." << endl;
                return true;
            }
        }
        cout << "Account not found." << endl;
        return false;
    }

    bool deposit(const string& accNum, double amount) {
        if (amount <= 0) { cout << "Error: Amount must be positive." << endl; return false; }

        Account* acc = searchByAccountNumber(accNum);
        if (!acc) { cout << "Error: Account not found." << endl; return false; }

        double oldBalance = acc->getBalance();
        acc->setBalance(oldBalance + amount);
        cout << "Deposit successful! Previous: $" << oldBalance
             << ", Deposited: $" << amount << ", New: $" << acc->getBalance() << endl;
        return true;
    }

    bool withdraw(const string& accNum, double amount) {
        if (amount <= 0) { cout << "Error: Amount must be positive." << endl; return false; }

        Account* acc = searchByAccountNumber(accNum);
        if (!acc) { cout << "Error: Account not found." << endl; return false; }

        if (!acc->canWithdraw(amount)) {
            cout << "Error: Insufficient funds! Available: $" << acc->getAvailableBalance() << endl;
            return false;
        }

        double oldBalance = acc->getBalance();
        acc->setBalance(oldBalance - amount);
        cout << "Withdrawal successful! Previous: $" << oldBalance
             << ", Withdrawn: $" << amount << ", New: $" << acc->getBalance() << endl;
        return true;
    }

    void showAccountInfo(const string& accNum) {
        Account* acc = searchByAccountNumber(accNum);
        if (!acc) { cout << "Error: Account not found." << endl; return; }

        cout << "\n=== Account Information ===" << endl;
        acc->displayDetails();
        cout << "\n=== Special Features ===" << endl;
        acc->showSpecialFeatures();
    }

    unique_ptr<Account> createAccount(int type, const string& accNum, const string& custName, double balance) {
        if (!isValid(accNum, false) || !isValid(custName, true) || balance < 0) return nullptr;

        switch (type) {
        case 1: return make_unique<Account>(accNum, custName, balance);
        case 2: {
            double rate;
            cout << "Enter interest rate (default 2.5%): ";
            cin >> rate;
            return make_unique<SavingsAccount>(accNum, custName, balance, rate > 0 ? rate : 2.5);
        }
        case 3: {
            double overdraft;
            cout << "Enter overdraft limit (default $500): $";
            cin >> overdraft;
            return make_unique<CheckingAccount>(accNum, custName, balance, overdraft >= 0 ? overdraft : 500);
        }
        }
        return nullptr;
    }
};

// Function to clear input buffer
void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Function to ask for retry with specific error messages
bool askForRetry(const string& errorType) {
    string choice;
    cout << "Error in " << errorType << ". Do you want to retry? (y/n): ";
    getline(cin, choice);
    return (choice == "y" || choice == "Y");
}

// menu selection with error handling
int getMenuChoice() {
    int choice;
    while (true) {
        cout << "\n===== Bank Account Management System =====\n"
             << "1. Add account\n2. Display all accounts\n3. Search by account number\n"
             << "4. Deposit\n5. Withdraw\n6. Delete account\n7. Show account info\n8. Exit\n";
        cout << "Enter choice: ";

        if (!(cin >> choice) || choice < 1 || choice > 8) {
            cout << "Error: Invalid input. Please enter a number between 1 and 8." << endl;
            clearInputBuffer();
            if (!askForRetry("menu selection")) {
                return -1; // User chose not to retry
            }
            continue;
        }
        cin.ignore(); // Clear the newline after numeric input
        return choice;
    }
}

// account number input with validation
string getAccountNumber(BankSystem& bankSystem, bool shouldExist = true) {
    string accNum;
    while (true) {
         cout << "Enter account number (numbers only, min 3 digits): ";
        getline(cin, accNum);

        // Basic validation
        if (accNum.empty() || accNum.length() < 3) {
            cout << "Error: Account number must be at least 3 characters long." << endl;
            if (!askForRetry("account number input")) {
                return "";
            }
            continue;
        }

        // Check if contains only digits
        bool validFormat = all_of(accNum.begin(), accNum.end(), [](char c) { return isdigit(c); });
        if (!validFormat) {
            cout << "Error: Account number can only contain numbers." << endl;
            if (!askForRetry("account number input")) {
                return "";
            }
            continue;
        }

        // Check existence
        Account* existingAcc = bankSystem.searchByAccountNumber(accNum);
        if (shouldExist && !existingAcc) {
            cout << "Error: Account number " << accNum << " not found." << endl;
            if (!askForRetry("account number input")) {
                return "";
            }
            continue;
        }

        if (!shouldExist && existingAcc) {
            cout << "Error: Account number " << accNum << " already exists." << endl;
            if (!askForRetry("account number input")) {
                return "";
            }
            continue;
        }

        return accNum;
    }
}

// amount input with validation
double getAmount() {
    double amount;
    while (true) {
        cout << "Enter amount: $";
        if (!(cin >> amount)) {
            cout << "Error: Invalid input. Please enter a valid number." << endl;
            clearInputBuffer();
            if (!askForRetry("amount input")) {
                return -1;
            }
            continue;
        }

        if (amount <= 0) {
            cout << "Error: Amount must be positive." << endl;
            clearInputBuffer(); // Clear the input buffer after detecting negative amount
            if (!askForRetry("amount input")) {
                return -1;
            }
            continue;
        }

        clearInputBuffer(); // Clear the input buffer before returning valid amount
        return amount;
    }
}

// customer name input
string getCustomerName() {
    string custName;
    while (true) {
        cout << "Enter customer name: ";
        getline(cin, custName);

        if (custName.empty() || custName.length() < 2) {
            cout << "Error: Name must be at least 2 characters long." << endl;
            if (!askForRetry("account input process")) {
                return "";
            }
            continue;
        }

        bool validName = all_of(custName.begin(), custName.end(), [](char c) { return isalpha(c) || c == ' '; });
        bool hasLetter = any_of(custName.begin(), custName.end(), [](char c) { return isalpha(c); });

        if (!validName) {
            cout << "Error: Name can only contain letters and spaces." << endl;
            if (!askForRetry("account input process")) {
                return "";
            }
            continue;
        }

        if (!hasLetter) {
            cout << "Error: Name must contain at least one letter." << endl;
            if (!askForRetry("account input process")) {
                return "";
            }
            continue;
        }

        return custName;
    }
}

// initial balance input
double getInitialBalance() {
    double balance;
    while (true) {
        cout << "Enter initial balance: $";
        if (!(cin >> balance)) {
            cout << "Error: Invalid input. Please enter a valid number." << endl;
            clearInputBuffer();
            if (!askForRetry("account input process")) {
                return -1;
            }
            continue;
        }

        if (balance < 0) {
            cout << "Error: Balance cannot be negative." << endl;
            clearInputBuffer(); // Clear the input buffer after detecting negative balance
            if (!askForRetry("account input process")) {
                return -1;
            }
            continue;
        }

        clearInputBuffer(); // Clear the input buffer before returning valid balance
        return balance;
    }
}

//  account type selection
int getAccountType() {
    int type;
    while (true) {
        cout << "\n1. Basic Account\n2. Savings Account\n3. Checking Account\n";
        cout << "Enter account type: ";

        if (!(cin >> type) || type < 1 || type > 3) {
            cout << "Error: Invalid choice. Please enter 1, 2, or 3." << endl;
            clearInputBuffer();
            if (!askForRetry("account input process")) {
                return -1;
            }
            continue;
        }
        cin.ignore(); // Clear the newline after numeric input
        return type;
    }
}

// account creation process
bool createNewAccount(BankSystem& bankSystem) {
    while (true) {
        cout << "\n=== Add New Account ===" << endl;

        // Get account type
        int type = getAccountType();
        if (type == -1) return false; // User chose not to retry

        // Get account number
        string accNum = getAccountNumber(bankSystem, false); // false = should not exist
        if (accNum.empty()) return false; // User chose not to retry

        // Get customer name
        string custName = getCustomerName();
        if (custName.empty()) return false; // User chose not to retry

        // Get initial balance
        double balance = getInitialBalance();
        if (balance == -1) return false; // User chose not to retry

        // Create and add account
        auto acc = bankSystem.createAccount(type, accNum, custName, balance);
        if (acc && bankSystem.addAccount(std::move(acc))) {
            cout << "Account added successfully!" << endl;
            return true;
        } else {
            cout << "Error: Failed to create account." << endl;
            if (!askForRetry("account input process")) {
                return false;
            }
            // Continue the loop to retry the entire process
        }
    }
}

// account search
bool searchAccount(BankSystem& bankSystem) {
    while (true) {
        string accNum = getAccountNumber(bankSystem, true); // true = should exist
        if (accNum.empty()) return false; // User chose not to retry

        Account* acc = bankSystem.searchByAccountNumber(accNum);
        if (acc) {
            cout << "\nAccount found:" << endl;
            acc->displayDetails();
            return true;
        } else {
            cout << "Error: Account not found." << endl;
            if (!askForRetry("account search")) {
                return false;
            }
            // Continue the loop to retry
        }
    }
}

// deposit process
bool performDeposit(BankSystem& bankSystem) {
    while (true) {
        string accNum = getAccountNumber(bankSystem, true);
        if (accNum.empty()) return false;

        double amount = getAmount();
        if (amount == -1) return false;

        if (bankSystem.deposit(accNum, amount)) {
            return true;
        } else {
            cout << "Error: Deposit failed." << endl;
            if (!askForRetry("amount input")) {
                return false;
            }
            // Continue the loop to retry
        }
    }
}

// withdrawal process
bool performWithdrawal(BankSystem& bankSystem) {
    while (true) {
        string accNum = getAccountNumber(bankSystem, true);
        if (accNum.empty()) return false;

        double amount = getAmount();
        if (amount == -1) return false;

        if (bankSystem.withdraw(accNum, amount)) {
            return true;
        } else {
            if (!askForRetry("withdrawal process")) {
                return false;
            }
            // Continue the loop to retry
        }
    }
}

// account deletion
bool deleteAccount(BankSystem& bankSystem) {
    while (true) {
        string accNum = getAccountNumber(bankSystem, true);
        if (accNum.empty()) return false;

        cout << "Delete account " << accNum << "? (y/n): ";
        string confirm;
        getline(cin, confirm);
        if (confirm == "y" || confirm == "Y") {
            bankSystem.deleteAccount(accNum);
            return true;
        } else {
            cout << "Account deletion cancelled." << endl;
            return true;
        }
    }
}

// Enhanced show account info
bool showAccountInfo(BankSystem& bankSystem) {
    while (true) {
        string accNum = getAccountNumber(bankSystem, true);
        if (accNum.empty()) return false;

        bankSystem.showAccountInfo(accNum);
        return true;
    }
}

// Main system function
int runBankSystem() {
    BankSystem bankSystem;
    int choice;

    do {
        choice = getMenuChoice();
        if (choice == -1) {
            cout << "Operation cancelled. Returning to main menu." << endl;
            continue;
        }

        switch (choice) {
        case 1:
            createNewAccount(bankSystem);
            break;
        case 2:
            bankSystem.displayAllAccounts();
            break;
        case 3:
            searchAccount(bankSystem);
            break;
        case 4:
            performDeposit(bankSystem);
            break;
        case 5:
            performWithdrawal(bankSystem);
            break;
        case 6:
            deleteAccount(bankSystem);
            break;
        case 7:
            showAccountInfo(bankSystem);
            break;
        case 8:
            cout << "Thank you for using Bank Account Management System!" << endl;
            break;
        }
    } while (choice != 8);

    return 1;
}

int main() {
    cout << "Welcome to Bank Account Management System!" << endl;
    runBankSystem();
    return 0;
}
