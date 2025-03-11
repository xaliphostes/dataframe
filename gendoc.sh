#!/bin/bash

# Script to generate documentation for DataFrame library

set -e

# Create output directory if it doesn't exist
mkdir -p docs

# Check if Doxygen is installed
if ! command -v doxygen &> /dev/null; then
    echo "Error: Doxygen is not installed. Please install it first."
    exit 1
fi

# Check if dot (Graphviz) is installed (for diagrams)
if ! command -v dot &> /dev/null; then
    echo "Warning: Graphviz (dot) is not installed. Diagrams will not be generated."
    sed -i 's/HAVE_DOT               = YES/HAVE_DOT               = NO/g' Doxyfile
fi

# Copy the custom CSS file to the docs directory
cp custom_doxygen.css docs/

# Run Doxygen
echo "Generating documentation with Doxygen..."
doxygen Doxyfile

# Check if the documentation was generated successfully
if [ $? -eq 0 ]; then
    echo "Documentation generated successfully in the 'docs' directory."
    echo "Open 'docs/html/index.html' in your browser to view it."
else
    echo "Error: Documentation generation failed."
    exit 1
fi

# Generate PDF documentation if LaTeX is enabled and installed
if grep -q "GENERATE_LATEX *= *YES" Doxyfile && command -v pdflatex &> /dev/null; then
    echo "Generating PDF documentation..."
    cd docs/latex
    make
    
    if [ -f "refman.pdf" ]; then
        cp refman.pdf ../DataFrame_Reference_Manual.pdf
        echo "PDF documentation generated at 'docs/DataFrame_Reference_Manual.pdf'"
    else
        echo "Error: PDF generation failed."
    fi
fi

echo "Done!"
