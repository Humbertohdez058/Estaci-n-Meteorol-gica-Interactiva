#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <LiquidCrystal_I2C.h>

// Configuración del LCD
#define LCD_ADDR 0x27
#define LCD_COLUMNS 20
#define LCD_ROWS 4
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLUMNS, LCD_ROWS);

// Crear objeto para el sensor BME280
Adafruit_BME280 bme;

// Definición de pines
#define LED_PIN 13
#define FAN_PIN 9  // Pin donde se controla el ventilador
#define LDR_PIN A0 // Pin analógico donde está conectado el LDR

// Umbrales de temperatura y humedad
const float TEMP_LOW_THRESHOLD = 20.0;  // Umbral bajo de temperatura
const float TEMP_HIGH_THRESHOLD = 30.0; // Umbral alto de temperatura
const float HUM_LOW_THRESHOLD = 30.0;   // Umbral bajo de humedad
const float HUM_HIGH_THRESHOLD = 70.0;  // Umbral alto de humedad

// Definición de la función mostrarLCD
void mostrarLCD(const char* mensaje1, const char* mensaje2 = "");

// Configuración inicial
void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Configurar pines
  pinMode(LED_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);  // Apagar el LED al inicio
  analogWrite(FAN_PIN, 0);      // Apagar el ventilador al inicio

  // Iniciar LCD
  lcd.begin(LCD_COLUMNS, LCD_ROWS);
  lcd.backlight();
  mostrarLCD("Iniciando sistema", "");

  // Iniciar el sensor BME280
  if (!bme.begin(0x76)) {
    mostrarLCD("Error BME280", "");
    digitalWrite(LED_PIN, LOW);  // Apagar el LED en caso de error
    while (1); // Detener el programa si no se encuentra el sensor
  } else {
    digitalWrite(LED_PIN, HIGH);  // Encender el LED si el sensor se inicia correctamente
  }

  mostrarLCD("Sistema listo", "");
}

void loop() {
  // Leer los valores del sensor
  float temperatura = bme.readTemperature();
  float humedad = bme.readHumidity();
  int ldrValue = analogRead(LDR_PIN);  // Leer el valor de la fotoresistencia

  // Controlar la velocidad del ventilador según los umbrales
  controlarVentilador(temperatura, humedad);

  // Mostrar los valores de los sensores en el LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperatura);
  lcd.print(" C");

  lcd.setCursor(0, 1);
  lcd.print("Humedad: ");
  lcd.print(humedad);
  lcd.print(" %");

  lcd.setCursor(0, 2);
  lcd.print("Luz: ");
  lcd.print(ldrValue);  // Mostrar el valor del LDR

  delay(2000);  // Esperar 2 segundos antes de la próxima lectura

  // Leer y procesar datos de la aplicación (simulando lectura desde el puerto serie)
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');  // Leer comando hasta nueva línea
    mostrarLCD("Comando: ", command.c_str());  // Mostrar el comando en el LCD
    delay(2000);  // Esperar 2 segundos para mostrar el mensaje en el LCD

    // Limpiar el LCD después de mostrar el comando
    lcd.clear();

    // Procesar comandos
    if (command == "ENCENDER_VENTILADOR") {
      analogWrite(FAN_PIN, 255); // Encender ventilador a máxima velocidad
      mostrarLCD("Ventilador", "Encendido");
    } else if (command == "APAGAR_VENTILADOR") {
      analogWrite(FAN_PIN, 0); // Apagar ventilador
      mostrarLCD("Ventilador", "Apagado");
    }

    delay(2000); // Esperar 2 segundos para mostrar el estado en el LCD
  }
}

void controlarVentilador(float temperatura, float humedad) {
  if (temperatura < TEMP_LOW_THRESHOLD || humedad < HUM_LOW_THRESHOLD) {
    // Temperatura y humedad baja
    analogWrite(FAN_PIN, 0); // Ventilador apagado
  } else if ((temperatura >= TEMP_LOW_THRESHOLD && temperatura < TEMP_HIGH_THRESHOLD) ||
             (humedad >= HUM_LOW_THRESHOLD && humedad < HUM_HIGH_THRESHOLD)) {
    // Temperatura y humedad media
    analogWrite(FAN_PIN, 128); // Ventilador a 50% (PWM = 128)
  } else {
    // Temperatura y humedad alta
    analogWrite(FAN_PIN, 255); // Ventilador a velocidad máxima (PWM = 255)
  }
}

// Definición de la función para mostrar mensajes en el LCD
void mostrarLCD(const char* mensaje1, const char* mensaje2 = "") {
  lcd.clear(); // Borrar el LCD antes de mostrar un nuevo mensaje
  lcd.setCursor(0, 0);
  lcd.print(mensaje1);
  lcd.setCursor(0, 1);
  lcd.print(mensaje2);
}