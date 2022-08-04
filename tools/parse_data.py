import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

file_name = "./archive/080322_trim.csv"

header_names = ["type","time","lat","NS","long","EW","quality","numSV","HDOP","alt","M_alt","sep","M_sep","diffAge","diffStation"]

df = pd.read_csv(file_name, names=header_names)

print(df)
print(df.dtypes)

lat_deg = np.floor(df["lat"]/100)
lat_min = df["lat"] - lat_deg*100
lat_dir = [1 if x == "N" else -1 for x in df["NS"]]
lat_dd = lat_dir*(lat_deg + lat_min/60.)

lon_deg = np.floor(df["long"]/100)
lon_min = df["long"] - lon_deg*100
lon_dir = [1 if x == "E" else -1 for x in df["EW"]]
lon_dd = lon_dir*(lon_deg + lon_min/60.)

alt = df["alt"]
time = df["time"]

lat_lon_df = pd.DataFrame({"lat": lat_dd, "lon": lon_dd, "alt": alt, "time": time})

lat_lon_df.to_csv("{}_lat_lon.csv".format(file_name), index=False)

print("Altitude Range [{} {}]".format(lat_lon_df["alt"].min(), lat_lon_df["alt"].max()))
