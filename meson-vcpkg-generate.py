import os
import re
import sys
from os.path import exists

# Define the template file and output file names
template_file = 'meson-vcpkg-template.txt'
output_file = 'meson-vcpkg.txt'

if exists(output_file):
    print(output_file + " already exists, remove it or make clean to regenerate.")
    exit(0)

# Get vcpkg root from environment variable
vcpkg_root = os.getenv('VCPKG_ROOT')
if not vcpkg_root:
    raise ValueError("VCPKG_ROOT environment variable is not set.")
vcpkg_root += '/'

script_dir = os.path.dirname(os.path.abspath(__file__))
vcpkg_base_install_dir = script_dir

# Determine platform based on OS
platform = {
    'linux': 'linux',
    'darwin': 'macos',
    'win32': 'windows'
}.get(sys.platform, 'linux')  # Default to linux if platform is unknown

# Read template file
with open(template_file, 'r') as f:
    template_content = f.read()

# Perform replacements using regular expressions
replacements = {
    r'BASE_PATH': f"{vcpkg_root}",
    r'x64-PLATFORM/': f'x64-{platform}/',
    r'PLATFORM': platform,
    r'BASE_INSTALL_DIR': vcpkg_base_install_dir,
}

for pattern, replacement in replacements.items():
    template_content = re.sub(pattern, replacement, template_content)

# Write to output file
with open(output_file, 'w') as f:
    f.write(template_content)

print(f'Generated {output_file} successfully.')