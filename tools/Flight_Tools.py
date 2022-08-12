import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from mpl_toolkits import mplot3d
import imageio
import os

from Vincenty import Vincenty_Inverse

class Flight:

    def __init__(self, time, lat, long, alt):
        lateral = [0]
        vertical = [alt[0]]
        distance = [0]
        speed = [0]
        for i in range(1, time.size):
            lateral.append(Vincenty_Inverse(lat[i-1], long[i-1], lat[i], long[i]).meters)
            vertical.append(alt[i] - alt[i-1])
            distance.append(np.sqrt(lateral[-1]**2 + vertical[-1]**2))
            speed.append(distance[-1] / (pd.to_datetime(time[i], format='%H:%M:%S') - pd.to_datetime(time[i-1], format='%H:%M:%S')).total_seconds())        
            
        self.data = pd.DataFrame({'time': time, 'lat': lat, 'long': long, 
                                  'alt': alt, 'lateral': lateral, 'vertical': vertical, 
                                  'distance': distance, 'speed': speed})
        
        self.flights = []
        
    def split_flights(self):
        max_flights = 10        # Limit number of flights to process
        flight_threshold = 1    # Speed in m/s to be considered in flight
        length_threshold = 30   # Number of records to be considered a flight
        window = 20             # Number of records to average when detecting flight
        margin = 10             # Number of records to pad around active flight
        
        temp = self.data[:]
        
        for i in range(max_flights):
            active = temp['speed'].rolling(window).mean() > flight_threshold
            
            if not active.any():
                break
            
            flight_start = active.idxmax()
            flight_end = active[flight_start:].idxmin()
            
            flight = temp[flight_start-margin:flight_end+margin]
            
            if len(flight) > length_threshold:
                self.flights.append(flight)
            
            temp = temp[flight_end+margin:]
            temp.reset_index(inplace=True, drop=True)
        
    def plot(self, flight=0, entire_flight=False):
        degree_margin = 0.00005
        meter_margin = 2
        
        if entire_flight:
            data = self.data
        else:
            data = self.flights[flight]
            
        x_min = data['lat'].min() - degree_margin
        x_max = data['lat'].max() + degree_margin
        y_min = data['long'].min() - degree_margin
        y_max = data['long'].max() + degree_margin
        z_min = data['alt'].min() - meter_margin
        z_max = data['alt'].max() + meter_margin
        
        fig = plt.figure()
        ax = plt.axes(projection='3d')
        ax.set_xlim3d(x_min, x_max)
        ax.set_ylim3d(y_min, y_max)
        ax.set_zlim3d(z_min, z_max)
        
        ax.plot3D(data['lat'], data['long'], data['alt'])
        plt.show()
    
    def make_gif(self, flight=0, entire_flight=False):  
        degree_margin = 0.00005
        meter_margin = 2
        lag = 2
        
        if entire_flight:
            data = self.data
            flight='all'
        else:
            data = self.flights[flight]
            
        x_min = data['lat'].min() - degree_margin
        x_max = data['lat'].max() + degree_margin
        y_min = data['long'].min() - degree_margin
        y_max = data['long'].max() + degree_margin
        z_min = data['alt'].min() - meter_margin
        z_max = data['alt'].max() + meter_margin
        
        images = []
        for i in range(data['time'].size):
            fig = plt.figure()
            ax = plt.axes(projection='3d')
            ax.set_xlim3d(x_min, x_max)
            ax.set_ylim3d(y_min, y_max)
            ax.set_zlim3d(z_min, z_max)
            ax.plot3D(data['lat'][:i+1], data['long'][:i+1], data['alt'][:i+1])
        
            image_name = f'{i}.png'
            images.append(image_name)
            plt.savefig(image_name)
            plt.close()
        
        with imageio.get_writer(f'flight_{flight}.gif', mode='I') as writer:
            for image in images:
                for i in range(lag):
                    writer.append_data(imageio.imread(image))
        
        for image in set(images):
            os.remove(image)

