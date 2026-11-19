
# ESP32-S3 IoT Telemetry System

## About the Project

I built this project to learn how a microcontroller communicates with a server over Wi-Fi. An ESP32-S3 sends measurements to a Python API, which stores them in an SQLite database. A web dashboard shows the latest values and a graph of recent measurements.

The ESP32-S3 measures its internal chip temperature, not the temperature of the room. Humidity is currently simulated because no external humidity sensor is connected.

## How It Works

Every 10 seconds, the ESP32-S3 reads its internal temperature and sends an HTTP POST request containing JSON data. Each measurement includes the temperature, simulated humidity, and a timestamp synchronized using NTP.

FastAPI uses Pydantic to validate the incoming data and saves it in SQLite. The dashboard retrieves stored measurements through an HTTP GET request and refreshes every 10 seconds.

```text
ESP32-S3 -> Wi-Fi -> FastAPI -> SQLite -> Web Dashboard
```

## Technologies Used

- **ESP32-S3 and C++:** Read the internal temperature and send measurements over Wi-Fi.
- **Arduino and PlatformIO:** Develop and upload the microcontroller program.
- **Python, FastAPI, and Pydantic:** Receive and validate measurements through an API.
- **SQLite:** Store measurements locally.
- **HTML, CSS, JavaScript, and Chart.js:** Display data on a web dashboard.

## Project Structure

```text
IOT/
├── Projet IOT/
│   ├── include/
│   │   └── secrets.example.h
│   ├── src/
│   │   └── main.cpp
│   └── platformio.ini
├── part1/
│   ├── dashboard/
│   │   └── index.html
│   ├── main.py
│   ├── models.py
│   ├── database.py
│   └── requirements.txt
├── .gitignore
└── README.md
```

## How to Run the Project

### 1. Start the FastAPI Server

Open PowerShell in the main `IOT` folder. Create a Python virtual environment and install the dependencies:

```powershell
python -m venv .environnement_iot
.\.environnement_iot\Scripts\python.exe -m pip install -r part1\requirements.txt
```

Start the server with:

```powershell
.\.environnement_iot\Scripts\python.exe -m uvicorn main:app --app-dir part1 --host 0.0.0.0 --port 8000 --reload
```

Keep the terminal open while the ESP32-S3 is sending measurements.

### 2. Configure the ESP32-S3

Open the `Projet IOT` folder in VS Code with PlatformIO. Copy `include/secrets.example.h` to `include/secrets.h` and enter your Wi-Fi name, password, and computer's local IP address.

The server URL should follow this format:

```cpp
#define SERVER_URL "http://YOUR_COMPUTER_IP:8000/measurements"
```

Connect the ESP32-S3 and the computer to the same trusted local network. Upload the program using PlatformIO and open the Serial Monitor. A successful transmission displays `HTTP POST: 200 OK`.

### 3. Open the Dashboard

With FastAPI running, open these addresses in your browser:

- **Dashboard:** http://127.0.0.1:8000/dashboard
- **Stored measurements:** http://127.0.0.1:8000/measurements
- **API documentation:** http://127.0.0.1:8000/docs

The dashboard uses Chart.js from an online CDN, so an internet connection is required to load the graph library.

## API Endpoints

| Method | Endpoint | Description |
|---|---|---|
| POST | `/measurements` | Receives and stores a measurement. |
| GET | `/measurements` | Returns stored measurements as JSON. |
| GET | `/dashboard` | Displays the web dashboard. |

## Current Limitations

The internal ESP32-S3 sensor measures chip temperature rather than room temperature. Humidity is simulated at `20.2%`, and older database records may contain simulated temperature values.

The server uses HTTP without authentication and is intended for testing on a trusted local network. The firmware does not yet store failed measurements for later transmission.

The `secrets.h` file contains private Wi-Fi settings and must not be uploaded to GitHub. It is excluded by `.gitignore`.

## Future Improvements

I plan to add an external temperature and humidity sensor, improve handling of failed connections, and add API authentication before using the system outside a trusted local network.
