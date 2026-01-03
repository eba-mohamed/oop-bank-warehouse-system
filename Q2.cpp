#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

// File paths for persistent storage
const string INVENTORY_FILE = "./warehouse_inventory.txt";
const string SHIPPING_QUEUE_FILE = "./warehouse_shipping.txt";

// Item class to represent inventory/shipping items
class Item {
    string name; // item
public:
    Item(const string& n) : name(n) {}
    Item() : name("") {}
    string getName() const { return name; }
};

// Custom Stack implementation (LIFO)
template <typename T>
class Stack {
    struct Node {
        T data;
        Node* next;
        Node(const T& d, Node* n = nullptr) : data(d), next(n) {}
    };
    Node* topNode;
public:
    Stack() : topNode(nullptr) {}
    ~Stack() { while (!empty()) pop(); }
    void push(const T& val) { topNode = new Node(val, topNode); }
    void pop() {
        if (topNode) {
            Node* temp = topNode;
            topNode = topNode->next;
            delete temp;
        }
    }
    T& top() {
        if (!topNode) throw runtime_error("Stack is empty");
        return topNode->data;
    }
    const T& top() const {
        if (!topNode) throw runtime_error("Stack is empty");
        return topNode->data;
    }
    bool empty() const { return topNode == nullptr; }

    // For listing all items (top to bottom, non-destructive)
    void toVector(vector<T>& out) const {
        Node* curr = topNode;
        while (curr) {
            out.push_back(curr->data);
            curr = curr->next;
        }
    }

    // For persistence: extract all items (top to bottom) into a vector, destroys stack!
    void extractToVector(vector<T>& out) {
        vector<T> temp;
        while (!empty()) {
            temp.push_back(top());
            pop();
        }
        // Now reverse to get bottom-to-top order
        for (auto it = temp.rbegin(); it != temp.rend(); ++it)
            out.push_back(*it);
    }

    // For persistence: load from a vector (bottom to top)
    void loadFromVector(const vector<T>& in) {
        for (const T& item : in) {
            push(item);
        }
    }
};

// Custom Queue implementation (FIFO)
template <typename T>
class Queue {
    struct Node {
        T data;
        Node* next;
        Node(const T& d) : data(d), next(nullptr) {}
    };
    Node *frontNode, *backNode;
public:
    Queue() : frontNode(nullptr), backNode(nullptr) {}
    ~Queue() { while (!empty()) pop(); }
    void push(const T& val) {
        Node* n = new Node(val);
        if (backNode) backNode->next = n;
        backNode = n;
        if (!frontNode) frontNode = n;
    }
    void pop() {
        if (frontNode) {
            Node* temp = frontNode;
            frontNode = frontNode->next;
            if (!frontNode) backNode = nullptr;
            delete temp;
        }
    }
    T& front() {
        if (!frontNode) throw runtime_error("Queue is empty");
        return frontNode->data;
    }
    const T& front() const {
        if (!frontNode) throw runtime_error("Queue is empty");
        return frontNode->data;
    }
    bool empty() const { return frontNode == nullptr; }

    // For listing all items (front to back, non-destructive)
    void toVector(vector<T>& out) const {
        Node* curr = frontNode;
        while (curr) {
            out.push_back(curr->data);
            curr = curr->next;
        }
    }

    // For persistence: extract all items (front to back) into a vector, destroys queue!
    void extractToVector(vector<T>& out) {
        while (!empty()) {
            out.push_back(front());
            pop();
        }
    }

    // For persistence: load from a vector (front to back)
    void loadFromVector(const vector<T>& in) {
        for (const T& item : in) {
            push(item);
        }
    }
};

// Adds an item to the inventory stack
void addIncomingItem(Stack<Item>& inventory) {
    string item;
    cout << "Enter item name: ";
    getline(cin, item);
    if (item.empty()) {
        cout << "Item name cannot be empty.\n";
        return;
    }
    inventory.push(Item(item)); // Store Item object in stack
    cout << "Item \"" << item << "\" added to inventory.\n";
}

// Processes the last incoming item into the shipping queue
void processIncomingItem(Stack<Item>& inventory, Queue<Item>& shippingQueue) {
    if (inventory.empty()) {
        cout << "No items in inventory to process.\n";
        return;
    }
    Item item = inventory.top();
    inventory.pop();
    shippingQueue.push(item);
    cout << "Processed \"" << item.getName() << "\" and added to shipping queue.\n";
}

// Ships the next item from the shipping queue
void shipItem(Queue<Item>& shippingQueue) {
    if (shippingQueue.empty()) {
        cout << "No items to ship.\n";
        return;
    }
    Item item = shippingQueue.front();
    shippingQueue.pop();
    cout << "Shipping item: " << item.getName() << "\n";
}

// Views the last incoming item in the inventory stack
void viewLastIncomingItem(const Stack<Item>& inventory) {
    if (inventory.empty()) {
        cout << "No items in inventory.\n";
        return;
    }
    cout << "Last incoming item: " << inventory.top().getName() << "\n";
}

// Views the next item in the shipping queue
void viewNextShipment(const Queue<Item>& shippingQueue) {
    if (shippingQueue.empty()) {
        cout << "No items in shipping queue.\n";
        return;
    }
    cout << "Next item to ship: " << shippingQueue.front().getName() << "\n";
}

// View all items in the inventory stack (from top to bottom)
void viewAllIncomingItems(const Stack<Item>& inventory) {
    vector<Item> items;
    inventory.toVector(items);
    if (items.empty()) {
        cout << "No items in inventory.\n";
        return;
    }
    cout << "All items in inventory (top to bottom):\n";
    for (const auto& item : items)
        cout << "- " << item.getName() << endl;
}

// View all items in the shipping queue (from front to back)
void viewAllShippingItems(const Queue<Item>& shippingQueue) {
    vector<Item> items;
    shippingQueue.toVector(items);
    if (items.empty()) {
        cout << "No items in shipping queue.\n";
        return;
    }
    cout << "All items in shipping queue (front to back):\n";
    for (const auto& item : items)
        cout << "- " << item.getName() << endl;
}

// Save inventory stack to file (using custom Stack)
void saveInventory(Stack<Item> inventory) {
    ofstream outFile(INVENTORY_FILE);
    if (!outFile) {
        cerr << "Error: Could not open file for writing: " << INVENTORY_FILE << endl;
        return;
    }
    vector<Item> items;
    inventory.extractToVector(items); // this will destroy the stack copy

    outFile << "ITEM_COUNT:" << items.size() << endl;
    for (const auto& item : items) {
        outFile << "ITEM:" << item.getName() << endl;
    }
    outFile.close();
    cout << items.size() << " items saved to inventory file." << endl;
}

// Save shipping queue to file (using custom Queue)
void saveShippingQueue(Queue<Item> shippingQueue) {
    ofstream outFile(SHIPPING_QUEUE_FILE);
    if (!outFile) {
        cerr << "Error: Could not open file for writing: " << SHIPPING_QUEUE_FILE << endl;
        return;
    }
    vector<Item> items;
    shippingQueue.extractToVector(items); // this will destroy the queue copy

    outFile << "ITEM_COUNT:" << items.size() << endl;
    for (const auto& item : items) {
        outFile << "ITEM:" << item.getName() << endl;
    }
    outFile.close();
    cout << items.size() << " items saved to shipping queue file." << endl;
}

// Load inventory stack from file (using custom Stack)
void loadInventory(Stack<Item>& inventory) {
    ifstream inFile(INVENTORY_FILE);
    if (!inFile) {
        cout << "No existing inventory data found." << endl;
        return;
    }
    while (!inventory.empty()) {
        inventory.pop();
    }
    try {
        string line;
        int count = 0;
        getline(inFile, line);
        if (line.substr(0, 11) == "ITEM_COUNT:") {
            count = stoi(line.substr(11));
        } else {
            throw runtime_error("Invalid format: Expected ITEM_COUNT");
        }
        vector<Item> items;
        for (int i = 0; i < count; i++) {
            getline(inFile, line);
            if (line.substr(0, 5) == "ITEM:") {
                string itemName = line.substr(5);
                items.push_back(Item(itemName));
            } else {
                throw runtime_error("Invalid format: Expected ITEM");
            }
        }
        for (auto it = items.rbegin(); it != items.rend(); ++it) {
            inventory.push(*it);
        }
        inFile.close();
        cout << count << " items loaded into inventory." << endl;
    } catch (const exception& e) {
        cerr << "Error loading inventory data: " << e.what() << endl;
        inFile.close();
    }
}

// Load shipping queue from file (using custom Queue)
void loadShippingQueue(Queue<Item>& shippingQueue) {
    ifstream inFile(SHIPPING_QUEUE_FILE);
    if (!inFile) {
        cout << "No existing shipping queue data found." << endl;
        return;
    }
    while (!shippingQueue.empty()) {
        shippingQueue.pop();
    }
    try {
        string line;
        int count = 0;
        getline(inFile, line);
        if (line.substr(0, 11) == "ITEM_COUNT:") {
            count = stoi(line.substr(11));
        } else {
            throw runtime_error("Invalid format: Expected ITEM_COUNT");
        }
        vector<Item> items;
        for (int i = 0; i < count; i++) {
            getline(inFile, line);
            if (line.substr(0, 5) == "ITEM:") {
                string itemName = line.substr(5);
                items.push_back(Item(itemName));
            } else {
                throw runtime_error("Invalid format: Expected ITEM");
            }
        }
        for (const auto& item : items) {
            shippingQueue.push(item);
        }
        inFile.close();
        cout << count << " items loaded into shipping queue." << endl;
    } catch (const exception& e) {
        cerr << "Error loading shipping queue data: " << e.what() << endl;
        inFile.close();
    }
}

int main() {
    Stack<Item> inventory;
    Queue<Item> shippingQueue;

    // Load saved data
    loadInventory(inventory);
    loadShippingQueue(shippingQueue);

    int choice;

    do {
        cout << "\nWarehouse Inventory and Shipping System\n";
        cout << "1. Add Incoming Item\n";
        cout << "2. Process Incoming Item\n";
        cout << "3. Ship Item\n";
        cout << "4. View Last Incoming Item\n";
        cout << "5. View Next Shipment\n";
        cout << "6. View All Incoming Items\n";
        cout << "7. View All Shipping Items\n";
        cout << "8. Exit\n";
        cout << "Enter your choice: ";
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid choice. Please enter a number between 1 and 8.\n";
            continue;
        }
        cin.ignore();

        switch (choice) {
            case 1:
                addIncomingItem(inventory);
                break;
            case 2:
                processIncomingItem(inventory, shippingQueue);
                break;
            case 3:
                shipItem(shippingQueue);
                break;
            case 4:
                viewLastIncomingItem(inventory);
                break;
            case 5:
                viewNextShipment(shippingQueue);
                break;
            case 6:
                viewAllIncomingItems(inventory);
                break;
            case 7:
                viewAllShippingItems(shippingQueue);
                break;
            case 8:
                cout << "Saving data before exit...\n";
                saveInventory(inventory);
                saveShippingQueue(shippingQueue);
                cout << "Exiting...\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 8);

    return 0;
}
