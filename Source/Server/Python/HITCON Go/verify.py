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

import sys
from datetime import datetime
from datetime import timedelta
import hashlib

# Const
TOKEN_DELAY = 5
UTC_DATE 	= datetime.utcnow()
FIXED_KEY	= 'c239e2e1'

# Fuctions

def __calculate_datetime():
	_delta_msec = UTC_DATE.microsecond * -1
	_delta_sec  = UTC_DATE.second * -1
	_delta_min  = UTC_DATE.minute / TOKEN_DELAY * TOKEN_DELAY - UTC_DATE.minute

	_timedelta_current = timedelta(minutes=_delta_min, seconds=_delta_sec, microseconds=_delta_msec)
	_timedelta_previous = timedelta(minutes=TOKEN_DELAY * -1)
	_hash_date_current = UTC_DATE + _timedelta_current
	_hash_date_previous = _hash_date_current + _timedelta_previous
	return [_hash_date_current, _hash_date_previous]

def __print_datetime(_datetime):
	return _datetime.strftime('%Y-%m-%d %H:%M:%S')

def __hash_checksum(_hash):
	count = 0
	checksum = 0
	for x in _hash:
		checksum += int(x, 16) * (count + 13)
		count += 1
	checksum %= 16
	return format(checksum, 'x')

def __hash_final(_hash):
	count = 0
	result = ''
	for x in _hash:
		value = (int(x, 16) * (count + 1337)) % 16
		result += format(value, 'x')
		count += 1
	return result

def __hash_fixed(_hash):
	count = 0
	result = ''
	for x in _hash:
		value = (int(x, 16) * (count + 1338)) % 16
		result += format(value, 'x')
		count += 1
	return result

def is_fixed(_hash):
	return str(_hash) == __hash_fixed(FIXED_KEY)

def __hash_datetime(_datetime):
	_hash = hashlib.md5('_MY_AWESOME_HITCON_TOKEN_'+__print_datetime(_datetime)).hexdigest()
	_hash = _hash[5:-20]
	return _hash + __hash_checksum(_hash)

def __datetime_hashes():
	_datetimes = __calculate_datetime()
	return [__hash_final(__hash_datetime(_datetimes[0])), __hash_final(__hash_datetime(_datetimes[1])), __hash_fixed(FIXED_KEY)]

def valid_keys():
	return __datetime_hashes()

def verify(_hash):
	_hashes = __datetime_hashes()
	if _hash in _hashes:
		return True
	return False

def main(argv):
	if len(sys.argv) > 1:
		_input = str(sys.argv[1])
		print('date:\t' + __print_datetime(UTC_DATE))
		print('key:\t' + _input)
		if verify(_input):
			print('result:\tvalid')
			return True
	else:
		print('usage:\t' + sys.argv[0] + ' KEY')
	print('result:\tinvalid')
	return False

if __name__ == "__main__":
    main(sys.argv)
