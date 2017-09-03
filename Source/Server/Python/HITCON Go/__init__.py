#
#    HITCON Go
#
#    Copyright (C) 2017 Zhi-Wei Cai.
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

import verify as validator

# validator.verify(INPUT_KEY) -> bool
# validator.is_fixed(INPUT_KEY) -> bool
# validator.valid_keys() -> List

def print_result(_key):
    print('Verification for key "' + str(_key) + '" = ' + str(validator.verify(_key)) + ', is fixed key: ' + str(validator.is_fixed(_key)))

keys = validator.valid_keys()

print('Validate keys: ' + str(keys))

_input = None # a null key
print_result(_input)

_input = 'wrong' # an invalid key
print_result(_input)

_input = keys[0] # current time based key
print_result(_input)

_input = keys[1] # previous time based key
print_result(_input)

_input = keys[2] # fixed key
print_result(_input)
