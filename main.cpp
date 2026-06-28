#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct Order {
    int id;
    string side;
    double price;
    int quantity;
    int time;
};

struct Trade {
    int tradeId;
    int buyOrderId;
    int sellOrderId;
    double price;
    int quantity;
};

class LimitOrderBook {
private:
    vector<Order> buyOrders;
    vector<Order> sellOrders;
    vector<Trade> trades;
    int nextOrderId;
    int nextTradeId;
    int currentTime;

public:
    LimitOrderBook() {
        nextOrderId = 1;
        nextTradeId = 1;
        currentTime = 1;
    }

    void addOrder(const string& side, double price, int quantity) {
        if (price <= 0 || quantity <= 0) {
            cout << "Price and quantity must be positive.\n";
            return;
        }

        Order order;
        order.id = nextOrderId++;
        order.side = side;
        order.price = price;
        order.quantity = quantity;
        order.time = currentTime++;

        if (side == "BUY") {
            buyOrders.push_back(order);
        } else {
            sellOrders.push_back(order);
        }

        cout << "Order added successfully. Order ID: " << order.id << "\n";
        matchOrders();
    }

    void cancelOrder(int orderId) {
        bool removed = removeOrderFromList(buyOrders, orderId);

        if (!removed) {
            removed = removeOrderFromList(sellOrders, orderId);
        }

        if (removed) {
            cout << "Order cancelled successfully.\n";
        } else {
            cout << "Order not found or already completed.\n";
        }
    }

    void showOrderBook() {
        sortBooks();

        cout << "\n========== ORDER BOOK ==========\n";
        cout << fixed << setprecision(2);

        cout << "\nBUY ORDERS\n";
        cout << left << setw(10) << "ID" << setw(12) << "Price" << setw(10) << "Qty" << "\n";
        for (const Order& order : buyOrders) {
            cout << left << setw(10) << order.id << setw(12) << order.price << setw(10) << order.quantity << "\n";
        }

        cout << "\nSELL ORDERS\n";
        cout << left << setw(10) << "ID" << setw(12) << "Price" << setw(10) << "Qty" << "\n";
        for (const Order& order : sellOrders) {
            cout << left << setw(10) << order.id << setw(12) << order.price << setw(10) << order.quantity << "\n";
        }

        cout << "================================\n";
    }

    void showTrades() {
        cout << "\n========== TRADES ==========\n";

        if (trades.empty()) {
            cout << "No trades completed yet.\n";
            return;
        }

        cout << fixed << setprecision(2);
        cout << left << setw(10) << "TradeID"
             << setw(12) << "BuyID"
             << setw(12) << "SellID"
             << setw(12) << "Price"
             << setw(10) << "Qty" << "\n";

        for (const Trade& trade : trades) {
            cout << left << setw(10) << trade.tradeId
                 << setw(12) << trade.buyOrderId
                 << setw(12) << trade.sellOrderId
                 << setw(12) << trade.price
                 << setw(10) << trade.quantity << "\n";
        }
    }

    void showMarketSummary() {
        sortBooks();

        cout << "\n========== MARKET SUMMARY ==========\n";
        cout << fixed << setprecision(2);

        if (buyOrders.empty()) {
            cout << "Best Bid: Not available\n";
        } else {
            cout << "Best Bid: " << buyOrders[0].price << "\n";
        }

        if (sellOrders.empty()) {
            cout << "Best Ask: Not available\n";
        } else {
            cout << "Best Ask: " << sellOrders[0].price << "\n";
        }

        if (!buyOrders.empty() && !sellOrders.empty()) {
            double spread = sellOrders[0].price - buyOrders[0].price;
            double midPrice = (sellOrders[0].price + buyOrders[0].price) / 2.0;

            cout << "Spread: " << spread << "\n";
            cout << "Mid Price: " << midPrice << "\n";
        } else {
            cout << "Spread: Not available\n";
            cout << "Mid Price: Not available\n";
        }

        cout << "Total Trades: " << trades.size() << "\n";
        cout << "====================================\n";
    }

    void exportOrderBookToCsv(const string& fileName) {
        sortBooks();

        ofstream file(fileName);

        if (!file) {
            cout << "Could not create " << fileName << "\n";
            return;
        }

        file << "OrderID,Side,Price,Quantity,Time\n";

        for (const Order& order : buyOrders) {
            file << order.id << ","
                 << order.side << ","
                 << fixed << setprecision(2) << order.price << ","
                 << order.quantity << ","
                 << order.time << "\n";
        }

        for (const Order& order : sellOrders) {
            file << order.id << ","
                 << order.side << ","
                 << fixed << setprecision(2) << order.price << ","
                 << order.quantity << ","
                 << order.time << "\n";
        }

        file.close();
        cout << "Order book exported to " << fileName << "\n";
    }

    void exportTradesToCsv(const string& fileName) {
        ofstream file(fileName);

        if (!file) {
            cout << "Could not create " << fileName << "\n";
            return;
        }

        file << "TradeID,BuyOrderID,SellOrderID,Price,Quantity\n";

        for (const Trade& trade : trades) {
            file << trade.tradeId << ","
                 << trade.buyOrderId << ","
                 << trade.sellOrderId << ","
                 << fixed << setprecision(2) << trade.price << ","
                 << trade.quantity << "\n";
        }

        file.close();
        cout << "Trades exported to " << fileName << "\n";
    }

private:
    void sortBooks() {
        sort(buyOrders.begin(), buyOrders.end(), [](const Order& a, const Order& b) {
            if (a.price == b.price) {
                return a.time < b.time;
            }
            return a.price > b.price;
        });

        sort(sellOrders.begin(), sellOrders.end(), [](const Order& a, const Order& b) {
            if (a.price == b.price) {
                return a.time < b.time;
            }
            return a.price < b.price;
        });
    }

    void matchOrders() {
        sortBooks();

        while (!buyOrders.empty() && !sellOrders.empty() && buyOrders[0].price >= sellOrders[0].price) {
            Order& buyOrder = buyOrders[0];
            Order& sellOrder = sellOrders[0];

            int tradedQuantity = min(buyOrder.quantity, sellOrder.quantity);
            double tradePrice = sellOrder.price;

            Trade trade;
            trade.tradeId = nextTradeId++;
            trade.buyOrderId = buyOrder.id;
            trade.sellOrderId = sellOrder.id;
            trade.price = tradePrice;
            trade.quantity = tradedQuantity;
            trades.push_back(trade);

            cout << "Trade executed: "
                 << tradedQuantity << " shares at price "
                 << fixed << setprecision(2) << tradePrice << "\n";

            buyOrder.quantity -= tradedQuantity;
            sellOrder.quantity -= tradedQuantity;

            if (buyOrder.quantity == 0) {
                buyOrders.erase(buyOrders.begin());
            }

            if (!sellOrders.empty() && sellOrders[0].quantity == 0) {
                sellOrders.erase(sellOrders.begin());
            }

            sortBooks();
        }
    }

    bool removeOrderFromList(vector<Order>& orders, int orderId) {
        auto position = find_if(orders.begin(), orders.end(), [orderId](const Order& order) {
            return order.id == orderId;
        });

        if (position == orders.end()) {
            return false;
        }

        orders.erase(position);
        return true;
    }
};

void seedDemoData(LimitOrderBook& book) {
    cout << "Loading sample orders into the book...\n";
    book.addOrder("BUY", 99.50, 10);
    book.addOrder("BUY", 100.00, 15);
    book.addOrder("SELL", 101.00, 12);
    book.addOrder("SELL", 102.00, 8);
    cout << "Sample data loaded. You can now add, cancel, view, or export orders.\n";
}

void printMenu() {
    cout << "\n========== LIMIT ORDER BOOK ==========\n";
    cout << "1. Add buy order\n";
    cout << "2. Add sell order\n";
    cout << "3. Cancel order\n";
    cout << "4. Show order book\n";
    cout << "5. Show trades\n";
    cout << "6. Show market summary\n";
    cout << "7. Export order book to CSV\n";
    cout << "8. Export trades to CSV\n";
    cout << "0. Exit\n";
    cout << "Select a menu option by entering a number from 0 to 8: ";
}

int main() {
    LimitOrderBook book;
    seedDemoData(book);

    int choice;

    do {
        printMenu();
        cin >> choice;

        if (choice == 1 || choice == 2) {
            double price;
            int quantity;

            if (choice == 1) {
                cout << "You selected: Add buy order\n";
            } else {
                cout << "You selected: Add sell order\n";
            }

            cout << "Enter order price: ";
            cin >> price;

            cout << "Enter order quantity: ";
            cin >> quantity;

            if (choice == 1) {
                book.addOrder("BUY", price, quantity);
            } else {
                book.addOrder("SELL", price, quantity);
            }
        }else if (choice == 3) {
            int orderId;
            cout << "You selected: Cancel order\n";
            cout << "Enter the order ID you want to cancel, for example 2: ";
            cin >> orderId;
            book.cancelOrder(orderId);
        } else if (choice == 4) {
            book.showOrderBook();
        } else if (choice == 5) {
            book.showTrades();
        } else if (choice == 6) {
            book.showMarketSummary();
        } else if (choice == 7) {
            book.exportOrderBookToCsv("orderbook.csv");
        } else if (choice == 8) {
            book.exportTradesToCsv("trades.csv");
        } else if (choice == 0) {
            cout << "Exiting project. Thank you!\n";
        } else {
            cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 0);

    return 0;
}
