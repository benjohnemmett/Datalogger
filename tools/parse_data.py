from pathlib import Path

from GPS_Helper import BN_220
from Flight_Tools import Flight

# If running as a project, it will use Datalogger at working directory - if running as a stand alone script, it will use tools directory
log_file = Path(r'./archive/57210423.C00')
log_file = log_file if log_file.exists() else r'.\tools' / log_file

data = BN_220(log_file).data

flight = Flight(data['time'], data['lat'], data['long'], data['alt'])

flight.split_flights()

for i in range(len(flight.flights)):
    flight.plot(flight=i)
    flight.make_gif(flight=i)

