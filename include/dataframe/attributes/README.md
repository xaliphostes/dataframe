# Attributes Manager Architecture

## Overview

The Attributes Manager system provides a flexible way to handle decomposition and access of complex data types within the DataFrame library. It's built around three main components:

1. The Manager class
2. The Decomposer interface
3. Serie<T> templates

## Core Concepts

### Serie<T>
The Serie<T> class is the fundamental building block of the system:
- A typed container that holds a sequence of values
- Supports various data types including primitives, vectors, and matrices
- Provides operations like map, forEach, and reduce
- Maintains type safety through templates

Example:
```cpp
Serie<Vector3> positions = {{1,0,0}, {0,1,0}, {0,0,1}};
Serie<double> temperatures = {98.6, 97.5, 99.1};
```

### Attribute Manager
The Manager class orchestrates attribute decomposition and access:
- Maintains a collection of Decomposers
- Provides attribute lookup and access
- Handles type conversion and validation
- Manages the lifecycle of decomposed attributes

Key responsibilities:
- Attribute name management
- Type-safe attribute access
- Decomposer coordination
- Error handling and validation

### Decomposers
Decomposers are specialized classes that know how to break down complex types:

#### Base Decomposer Interface
```cpp
class Decomposer {
    virtual std::vector<std::string> names(...) const = 0;
    virtual Serie<double> serie(...) const = 0;
    virtual std::unique_ptr<Decomposer> clone() const = 0;
};
```

#### Example of specialized Decomposers

1. Coordinates Decomposer
   - Handles spatial coordinates (x, y, z)
   - Works with Vector2 and Vector3 types
   - Creates named components (e.g., "position_x")

2. Components Decomposer
   - Handles general array-like types
   - Supports matrices and tensors
   - Creates indexed components (e.g., "stress_0")

## Relationships and Data Flow

### Manager → Serie
- Manager maintains references to Series through the Dataframe
- Provides type-safe access to Serie data
- Handles Serie lookup and validation

### Manager → Decomposers
- Owns collection of Decomposer instances
- Delegates decomposition requests
- Manages Decomposer lifecycle

### Decomposers → Serie
- Read from source Series
- Create new Series for components
- Handle type conversion

## Usage Patterns

### 1. Basic Attribute Access
```cpp
// Create and populate a dataframe
Dataframe df;
df.add("positions", positions);
df.add("temperatures", temperatures);

// Create manager with decomposers
Manager manager(df);
manager.addDecomposer(std::make_unique<Coordinates>());

// Access attributes
auto posX = manager.getSerie<double>("positions_x");
```

### 2. Component Extraction
```cpp
// Add stress tensor data
df.add("stress", stressTensor);
manager.addDecomposer(std::make_unique<Components>());

// Get stress components
auto stressXX = manager.getSerie<double>("stress_0");
auto stressXY = manager.getSerie<double>("stress_1");
```

### 3. Attribute Discovery
```cpp
// Get all available attributes
auto names = manager.getNames(sizeof(double));

// Check for specific attribute
if (manager.hasAttribute(sizeof(Vector3), "positions")) {
    // Handle vector attribute
}
```

## Type System

### Supported Types
1. Primitive Types
   - double
   - int
   - bool

2. Vector Types
   - Vector2
   - Vector3
   - Vector4

3. Matrix Types
   - Matrix2D
   - Matrix3D
   - Stress2D
   - Stress3D

### Type Safety
The system ensures type safety through:
- Template specialization
- Compile-time type checking
- Runtime type validation
- Exception handling for type mismatches

## Error Handling

The system provides comprehensive error handling:

1. Validation Errors
   - Invalid attribute names
   - Missing attributes
   - Type mismatches

2. Runtime Errors
   - Memory allocation failures
   - Invalid decomposition requests
   - Index out of bounds

3. Type Errors
   - Incompatible type conversions
   - Unsupported type decompositions

## Extension Points

The system can be extended through:

1. Custom Decomposers
   - Implement new decomposition strategies
   - Support additional data types
   - Add specialized naming conventions

2. New Data Types
   - Add support for new Serie types
   - Implement type-specific decomposition rules
   - Extend existing decomposers

## Best Practices

1. Type Management
   - Use appropriate types for data
   - Maintain consistent type usage
   - Document type requirements

2. Error Handling
   - Check return values
   - Handle exceptions appropriately
   - Validate input data

3. Performance
   - Use appropriate container sizes
   - Minimize type conversions
   - Optimize decomposition strategies

## Limitations and Considerations

1. Performance Implications
   - Decomposition overhead
   - Memory usage for component storage
   - Type conversion costs

2. Memory Management
   - Smart pointer usage
   - Resource cleanup
   - Memory allocation strategies

3. Thread Safety
   - Concurrent access considerations
   - Thread-safe decomposition
   - Resource sharing guidelines

# Last example
```c++
// Create some sample data
df::Serie<df::Vector3> positions = {{1,0,0}, {0,1,0}, {0,0,1}};
df::Serie<df::Stress3D> stresses = {{1,2,3,4,5,6}, {7,8,9,10,11,12}};

// Create a dataframe
df::Dataframe dataframe;
dataframe.add("positions", positions);
dataframe.add("stresses", stresses);

// Create manager with decomposers
df::attributes::Manager manager(dataframe);
manager.addDecomposer(std::make_unique<df::attributes::Coordinates>());
manager.addDecomposer(std::make_unique<df::attributes::Components>());

// Access decomposed attributes
auto posX = manager.getSerie<double>("positions_x");
auto stressXX = manager.getSerie<double>("stresses_0");

// Get all available attribute names
auto names = manager.getNames(sizeof(double));
```