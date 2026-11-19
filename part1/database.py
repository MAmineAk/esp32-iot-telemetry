import sqlite3
from models import Measurement
from pathlib import Path

PATH_DATABASE = Path(__file__).resolve().parent/ "measurements.db"

def database_init():
    connection = sqlite3.connect(PATH_DATABASE)

    try:

        connection.execute("CREATE TABLE IF NOT EXISTS measurements(id INTEGER PRIMARY KEY, temperature REAL, humidity REAL, timestamp TEXT)")

        connection.commit() 

    finally:

        connection.close()

def save_measurements(measurement : Measurement):

    connection = sqlite3.connect(PATH_DATABASE)

    try:
        connection.execute("INSERT INTO measurements (temperature,humidity,timestamp) VALUES (?,?,?)" , (measurement.temperature,measurement.humidity,measurement.timestamp.isoformat()))

        connection.commit() 

    finally:

        connection.close()

def get_measurements():

    connection = sqlite3.connect(PATH_DATABASE)
    connection.row_factory = sqlite3.Row

    try :
        cursor = connection.execute("SELECT * FROM measurements")
        rows = cursor.fetchall()
        measurements_list = []
        for row in rows:
            measurements_list.append(dict(row))

        return measurements_list

    finally:    
        connection.close()