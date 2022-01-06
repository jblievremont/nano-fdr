#
# This file is part of nano-fdr.
#
# nano-fdr is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# nano-fdr is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# nano-fdr. If not, see <https://www.gnu.org/licenses/>.
#

import sys
import serial

def main():
  if len(sys.argv) < 2:
    print(f'Usage: {sys.argv[0]} <serial port>')
    sys.exit(1)

  serial_port_name = sys.argv[1]
  with serial.Serial(serial_port_name, 115200) as serial_port:
    next_line = serial_port.readline().decode('utf-8')
    print(next_line, end="")
    while next_line is not None and len(next_line) > 0 and next_line[0] != ']':
      next_line = serial_port.readline().decode('utf-8')
      print(next_line, end="")

if __name__ == "__main__":
  main()
