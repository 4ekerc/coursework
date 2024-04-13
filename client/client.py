# -*- coding: utf-8 -*-
import tkinter as tk
from tkinter import Label

import requests

def get_weather_data():
    try:
        response = requests.get("http://localhost:3000/raw")
        if response.status_code == 200:
            data = response.json()
            return data
        else:
            print("Error:", response.status_code)
    except Exception as e:
        print("An error occurred:", e)

def update_weather(event=None):
    weather_data = get_weather_data()
    if weather_data:
        weather_description = weather_data.get("weatherDescription", "Unknown")
        temperature = weather_data.get("maxTemperature", "Unknown")
        weather_label.config(text=weather_description)
        temperature_label.config(text=str(temperature) +  "\u00b0C")

app = tk.Tk()
app.title("Weather Client")
app.geometry("400x400")
app.configure(bg="white")

city_label = tk.Label(app, text="\u0421\u0438\u043c\u0444\u0435\u0440\u043e\u043f\u043e\u043b\u044c", font=("Arial", 14, "bold"), bg="orange", fg="black")
city_label.grid(row=0, column=0, columnspan=2, sticky="ew")

weather_label = tk.Label(app, text="", font=("Arial", 12), bg="orange", fg="black")
weather_label.grid(row=1, column=0, columnspan=2, sticky="ew")

temperature_label = tk.Label(app, text="", font=("Arial", 125, "bold"), bg="white", fg="black")
temperature_label.grid(row=2, column=0, columnspan=2, sticky="ew")

null_label: Label = tk.Label(app, text="", font=("Arial", 26 ), bg="orange", fg="black")
null_label.grid(row=3, column=0, columnspan=2, sticky="ew")

# Привязываем функцию обновления погоды и состояния погоды к событию нажатия левой кнопки мыши на окне
app.bind("<Button-1>", update_weather)

# Центрирование строк
app.grid_rowconfigure(2, weight=1)
app.grid_rowconfigure(1, minsize=0)  # Удалить расстояние между первой и второй меткой
app.grid_rowconfigure(2, minsize=150)  # Добавить расстояние между второй и третьей метками

app.grid_columnconfigure(0, weight=1)  # Установить вес для первого столбца
app.grid_columnconfigure(1, weight=1)  # Установить вес для второго столбца

update_weather()  # Обновляем погоду и состояние погоды при запуске приложения

app.mainloop()
