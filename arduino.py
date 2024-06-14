import serial
import csv
import re
import time  # Import the time module

# Configura el puerto serie (asegúrate de que el nombre del puerto sea correcto)
ser = serial.Serial('COM23', 9600, timeout=1)  # Actualiza 'COM23' según corresponda

# Counter for file names
file_counter = 10

# Function to create a new file with a sequential name
def create_new_file(counter):
    return open(f'datalog{counter}.csv', mode='w', newline='')

# Open the first file
file = create_new_file(file_counter)
writer = csv.writer(file)
writer.writerow(["Timestamp", "Angle (degrees)", "Distance (cm)"])  # Escribe el encabezado

while True:
    line = ser.readline().decode('utf-8').strip()
    if line:
        print(line)
        # Extrae el ángulo y la distancia del texto recibido usando expresiones regulares
        match = re.search(r'Angle: (\d+), Distance: (\d+) cm', line)
        if match:
            angle = match.group(1)
            distance = match.group(2)
            timestamp = time.strftime("%Y-%m-%d %H:%M:%S")  # Captura el tiempo actual
            writer.writerow([timestamp, angle, distance])  # Escribe los datos en el archivo CSV
            file.flush()  # Asegura que los datos se escriban inmediatamente
        
        # Check if file size exceeds a limit (e.g., 1 MB) to create a new file
        if file.tell() > 1 * 1024 * 1024:  # 1 MB limit
            file_counter += 1
            file.close()
            file = create_new_file(file_counter)
            writer = csv.writer(file)
            writer.writerow(["Timestamp", "Angle (degrees)", "Distance (cm)"])  # Escribe el encabezado
