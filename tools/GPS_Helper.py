import pandas as pd
import numpy as np

class BN_220:
    header_names = ["type","time","lat","NS","long","EW","quality","numSV","HDOP","alt","M_alt","sep","M_sep","diffAge","diffStation"]
 
    def __init__(self, log_file):
        df = pd.read_csv(log_file, names=BN_220.header_names)

        ns_direction = [1 if x == "N" else -1 for x in df["NS"]]
        ew_direction = [1 if x == "E" else -1 for x in df["EW"]]

        time = pd.to_datetime(df["time"], format='%H%M%S').dt.time
        lat = ns_direction * self.degrees_decimal(df['lat'])
        long = ew_direction * self.degrees_decimal(df['long'])
        alt = df['alt']

        self.data = pd.DataFrame({'time': time, 'lat': lat, 'long': long, 'alt': alt})

    def degrees_decimal(self, data):
        degrees = np.floor(data/100)
        minutes = data - degrees*100
                
        return (degrees + minutes/60.0)
