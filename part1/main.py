from fastapi import FastAPI
from models import Measurement
from database import save_measurements,database_init, get_measurements
from contextlib import asynccontextmanager
from pathlib import Path
from fastapi.responses import FileResponse

@asynccontextmanager
async def lifespan(app: FastAPI):
    database_init()
    yield
app = FastAPI(lifespan = lifespan)


@app.post("/measurements")   # Tells the FastAPI if any POST request came in the path "/measurements", call this function
def receiving_measurements(measurement : Measurement):
    save_measurements(measurement)
    return {"message":"Message received", "temperature": measurement.temperature, "humidity": measurement.humidity, "timestamp": measurement.timestamp}
    
@app.get("/measurements")
def read_measurements():
    return get_measurements()

@app.get("/dashboard")
def show_dashboard():
    html_file = Path(__file__).resolve().parent / "dashboard" / "index.html"
    return FileResponse(html_file)
