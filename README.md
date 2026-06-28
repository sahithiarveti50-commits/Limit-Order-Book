# Limit Order Book Simulator

A console-based C++ project that simulates a simple limit order book. The program stores buy and sell orders, matches compatible orders, records completed trades, shows market summary values, and exports data to CSV files.

The project is implemented in a single source file: `main.cpp`.

## Features

- Add buy limit orders.
- Add sell limit orders.
- Match orders when the highest buy price is greater than or equal to the lowest sell price.
- Support partial order fills.
- Cancel active orders using order ID.
- Display active buy and sell orders.
- Display completed trades.
- Show best bid, best ask, spread, and mid-price.
- Export active orders to `orderbook.csv`.
- Export completed trades to `trades.csv`.

## How It Works

The order book keeps two lists:

- `buyOrders`: active buy orders.
- `sellOrders`: active sell orders.

Buy orders are sorted by highest price first. If two buy orders have the same price, the older order gets priority.

Sell orders are sorted by lowest price first. If two sell orders have the same price, the older order gets priority.

A trade is executed when:

```text
highest buy price >= lowest sell price
```

The trade quantity is the smaller quantity between the buy order and sell order. If one order still has quantity left after the trade, it remains in the order book.

## Technologies Used

- C++17
- `vector`
- `string`
- `algorithm`
- `fstream`
- `iomanip`
- classes and structs

## Project Structure

```text
limit-order-book-simulator/
  main.cpp
  README.md
  .gitignore
```

## Build And Run

Compile the program:

```bash
g++ -std=c++17 main.cpp -o lob
```

Run on macOS/Linux:

```bash
./lob
```

Run on Windows PowerShell:

```powershell
.\lob.exe
```

## Menu Options

```text
1. Add buy order
2. Add sell order
3. Cancel order
4. Show order book
5. Show trades
6. Show market summary
7. Export order book to CSV
8. Export trades to CSV
0. Exit
```

## Example Input Flow

```text
Select a menu option by entering a number from 0 to 8: 1
You selected: Add buy order
Enter order price, for example 101.50: 101.00
Enter order quantity, for example 10: 5
```

## CSV Output

The program can generate two CSV files:

- `orderbook.csv`: contains all active buy and sell orders.
- `trades.csv`: contains all completed trades.

These files are generated only when the export options are selected from the menu.

## Sample CSV Format

`orderbook.csv`

```csv
OrderID,Side,Price,Quantity,Time
2,BUY,100.00,15,2
3,SELL,101.00,7,3
```

`trades.csv`

```csv
TradeID,BuyOrderID,SellOrderID,Price,Quantity
1,5,3,101.00,5
```
