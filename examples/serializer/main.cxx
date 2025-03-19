#include <cmath>
#include <dataframe/Serie.h>
#include <dataframe/io/binary_serialization.h>
#include <iostream>
#include <string>
#include <vector>

// Define a custom data type
struct SensorReading {
    int64_t timestamp;
    double temperature;
    double humidity;
    std::string sensorId;
    bool isValid;
};

// Function to print a Serie of SensorReadings
void printReadings(const df::Serie<SensorReading> &readings) {
    std::cout << "Sensor Readings (" << readings.size() << " entries):\n";
    std::cout << "---------------------------------------------------------\n";
    std::cout << "Timestamp   | Temperature | Humidity | Sensor ID | Valid |\n";
    std::cout << "---------------------------------------------------------\n";

    for (size_t i = 0; i < readings.size(); ++i) {
        const auto &r = readings[i];
        std::cout << r.timestamp << " | " << std::fixed << std::setprecision(2)
                  << r.temperature << "°C | " << std::fixed
                  << std::setprecision(2) << r.humidity << "% | " << r.sensorId
                  << " | " << (r.isValid ? "Yes" : "No") << " |\n";
    }
    std::cout << "---------------------------------------------------------\n";
}

int main() {
    try {
        // Register our custom SensorReading type with the serialization system
        df::io::registerCustomType<SensorReading>(
            "SensorReading",

            // Write function
            [](std::ofstream &ofs, const SensorReading &reading,
               bool swap_needed) {
                // Write each field in order
                df::io::detail::write_value(ofs, reading.timestamp,
                                            swap_needed);
                df::io::detail::write_value(ofs, reading.temperature,
                                            swap_needed);
                df::io::detail::write_value(ofs, reading.humidity, swap_needed);
                df::io::detail::serializer<std::string>::write(
                    ofs, reading.sensorId, swap_needed);
                df::io::detail::write_value(ofs, reading.isValid, swap_needed);
            },

            // Read function
            [](std::ifstream &ifs, bool swap_needed) {
                SensorReading reading;
                // Read each field in the same order
                reading.timestamp =
                    df::io::detail::read_value<int64_t>(ifs, swap_needed);
                reading.temperature =
                    df::io::detail::read_value<double>(ifs, swap_needed);
                reading.humidity =
                    df::io::detail::read_value<double>(ifs, swap_needed);
                reading.sensorId =
                    df::io::detail::serializer<std::string>::read(ifs,
                                                                  swap_needed);
                reading.isValid =
                    df::io::detail::read_value<bool>(ifs, swap_needed);
                return reading;
            });

        // Create some sample data
        df::Serie<SensorReading> readings = {
            {1647264000, 22.5, 45.2, "sensor-001", true},
            {1647264060, 22.7, 45.5, "sensor-001", true},
            {1647264120, 22.8, 45.7, "sensor-001", true},
            {1647264180, 23.1, 46.0, "sensor-001", true},
            {1647264240, 0.0, 0.0, "sensor-001", false}, // Invalid reading
            {1647264300, 23.2, 46.5, "sensor-001", true},
        };

        // Also create a Serie of a built-in type
        df::Serie<double> temperatures = {22.5, 22.7, 22.8, 23.1, 0.0, 23.2};

        std::cout << "Original data:\n";
        printReadings(readings);

        // Save data to binary files
        const std::string readingsFile = "sensor_readings.bin";
        const std::string temperaturesFile = "temperatures.bin";

        std::cout << "\nSaving data to files...\n";
        df::io::save(readings, readingsFile);
        df::io::save(temperatures, temperaturesFile);

        // Check file types
        std::cout << "\nFile type of " << readingsFile << ": "
                  << df::io::get_file_type(readingsFile) << std::endl;
        std::cout << "File type of " << temperaturesFile << ": "
                  << df::io::get_file_type(temperaturesFile) << std::endl;

        // Load with explicit type
        std::cout << "\nLoading data with explicit type...\n";
        auto loadedReadings = df::io::load<SensorReading>(readingsFile);
        auto loadedTemperatures = df::io::load<double>(temperaturesFile);

        std::cout << "\nLoaded readings (explicit type):\n";
        printReadings(loadedReadings);

        // Load with automatic type detection
        std::cout << "\nLoading data with automatic type detection...\n";
        auto autoReadings = df::io::load(readingsFile);
        auto autoTemperatures = df::io::load(temperaturesFile);

        // Cast to appropriate type
        auto typedReadings =
            std::dynamic_pointer_cast<df::Serie<SensorReading>>(autoReadings);
        auto typedTemperatures =
            std::dynamic_pointer_cast<df::Serie<double>>(autoTemperatures);

        if (typedReadings) {
            std::cout << "\nLoaded readings (auto-detection):\n";
            printReadings(*typedReadings);
        } else {
            std::cout << "Error: Could not cast to Serie<SensorReading>\n";
        }

        if (typedTemperatures) {
            std::cout << "\nLoaded temperatures (auto-detection): ";
            for (size_t i = 0; i < typedTemperatures->size(); ++i) {
                std::cout << (*typedTemperatures)[i]
                          << (i < typedTemperatures->size() - 1 ? ", " : "");
            }
            std::cout << std::endl;
        } else {
            std::cout << "Error: Could not cast to Serie<double>\n";
        }

        // Demonstrate error handling - attempt to load with wrong type
        try {
            std::cout
                << "\nAttempting to load readings as double (will fail)...\n";
            auto wrongType = df::io::load<double>(readingsFile);
            std::cout << "This should not be reached!\n";
        } catch (const std::exception &e) {
            std::cout << "Expected error: " << e.what() << std::endl;
        }

        // Clean up files
        std::remove(readingsFile.c_str());
        std::remove(temperaturesFile.c_str());

        std::cout << "\nExample completed successfully!\n";
        return 0;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}