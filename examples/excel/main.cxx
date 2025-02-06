#include <dataframe/Dataframe.h>
#include <dataframe/Serie.h>
#include <dataframe/utils/zip.h>
#include <dataframe/utils/compose.h>

int main() {
    // Create a Dataframe similar to this Excel sheet:
    //
    // | Product  | Price | Quantity | Date       |
    // |----------|-------|----------|------------|
    // | Apple    | 0.50  | 100      | 2024-01-01|
    // | Orange   | 0.75  | 80       | 2024-01-01|
    // | Banana   | 0.30  | 150      | 2024-01-01|
    // | Apple    | 0.55  | 120      | 2024-01-02|
    // | Orange   | 0.80  | 90       | 2024-01-02|
    // | Banana   | 0.35  | 140      | 2024-01-02|

    df::Dataframe sales;

    // Adding columns (like creating columns in Excel)
    sales.add("Product", df::Serie<std::string>{
        "Apple", "Orange", "Banana", 
        "Apple", "Orange", "Banana"
    });

    sales.add("Price", df::Serie<double>{
        0.50, 0.75, 0.30,
        0.55, 0.80, 0.35
    });

    sales.add("Quantity", df::Serie<int>{
        100, 80, 150,
        120, 90, 140
    });

    sales.add("Date", df::Serie<std::string>{
        "2024-01-01", "2024-01-01", "2024-01-01",
        "2024-01-02", "2024-01-02", "2024-01-02"
    });

    // Excel-like operations:

    // 1. Creating a calculated column (like =Price*Quantity in Excel)
    auto prices = sales.get<double>("Price");
    auto quantities = sales.get<int>("Quantity");
    auto revenues = df::zip(prices, quantities).map([](const auto& tuple, size_t) {
        auto [price, qty] = tuple;
        return price * qty;
    });
    sales.add("Revenue", revenues);

    // 2. Filtering (like using Excel filters)
    // Get all Apple sales
    auto products = sales.get<std::string>("Product");
    auto apple_mask = products.map([](const std::string& p, size_t) {
        return p == "Apple";
    });

    // 3. Working with multiple columns (like VLOOKUP or INDEX/MATCH in Excel)
    // Get average price per product
    auto price_by_product = df::zip(products, prices).map([](const auto& tuple, size_t) {
        auto [product, price] = tuple;
        return std::make_tuple(product, price);
    });

    // 4. Date-based analysis (like Excel pivot tables)
    auto dates = sales.get<std::string>("Date");
    auto daily_sales = df::zip(dates, revenues).map([](const auto& tuple, size_t) {
        auto [date, rev] = tuple;
        return std::make_tuple(date, rev);
    });

    // Print structure (like Excel's table view)
    std::cout << "Dataframe Structure:\n";
    for (const auto& name : sales.names()) {
        std::cout << "Column: " << name << "\n";
    }

    return 0;
}
