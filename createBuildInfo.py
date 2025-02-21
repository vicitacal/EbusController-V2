import re
from datetime import datetime

BUILD_INFO_FILE = 'src/BuildInfo.h'

try:
    with open(BUILD_INFO_FILE, 'r') as f:
        content = f.read()
    match = re.search(r'#define BUILD_NUMBER (\d+)', content)
    build_number = int(match.group(1)) + 1 if match else 0
except FileNotFoundError:
    build_number = 0

build_time = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
with open(BUILD_INFO_FILE, 'w') as f:
    f.write(f'#ifndef BUILD_INFO_H\n')
    f.write(f'#define BUILD_INFO_H\n\n')
    f.write(f'#define BUILD_NUMBER {build_number} // auto generated value\n\n')
    f.write(f'#define BUILD_DATE "{build_time}"\n\n')
    f.write(f'#endif\n')