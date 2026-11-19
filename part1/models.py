from pydantic import BaseModel, Field
from datetime import datetime

class Measurement(BaseModel):
    temperature: float
    humidity: float = Field(ge=0,le=100)
    timestamp: datetime 