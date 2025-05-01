#!/usr/bin/env python3

import argparse
import os
import sys

def generate_header(program_path, output_path):
    try:
        with open(program_path, 'r') as f:
            program_content = f.read()
    except FileNotFoundError:
        print(f"Error: Program file '{program_path}' not found")
        sys.exit(1)

    # Escape special characters and split into lines
    escaped_content = program_content.replace('\\', '\\\\').replace('"', '\\"')
    lines = escaped_content.split('\n')

    # Generate the header file content
    header_content = f"""#pragma once

#include <string_view>

namespace lox {{
    constexpr std::string_view program = R"({program_content})";
}}
"""

    # Create output directory if it doesn't exist
    os.makedirs(os.path.dirname(output_path), exist_ok=True)

    # Write the header file
    with open(output_path, 'w') as f:
        f.write(header_content)

def main():
    parser = argparse.ArgumentParser(description='Generate a C++ header file with a static program string')
    parser.add_argument('program_path', help='Path to the program file')
    parser.add_argument('output_path', help='Path to the output header file')
    args = parser.parse_args()

    generate_header(args.program_path, args.output_path)

if __name__ == '__main__':
    main() 