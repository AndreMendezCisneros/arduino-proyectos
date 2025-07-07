#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <math.h>

// --- Configuraciones ---
#define BT_RX_PIN 2
#define BT_TX_PIN 3
SoftwareSerial BT(BT_RX_PIN, BT_TX_PIN);

#define RELE_PIN_BOMBA 9

// --- Termistor ---
#define THERMISTORPIN A0
#define SERIESRESISTOR 10000
#define THERMISTORNOMINAL 10000
#define TEMPERATURENOMINAL 25
#define BCOEFFICIENT 3950

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Variables globales
bool estadoBomba = false;
unsigned long tiempoAnteriorSensor = 0;
const unsigned long intervaloLectura = 2500;
unsigned long tiempoAnteriorLCD = 0;
const unsigned long intervaloActualizacionLCD = 1000;

float temperatura = NAN;
float humedad = NAN;  // No se usa, solo decorativo

char comandoBT_char;

// Horario automático
unsigned long tiempoInicioHorario = 0;
bool horarioActivo = false;

enum EstadoHorario {
  APAGADO,
  BOMBA_ON,
  BOMBA_OFF
};

EstadoHorario estadoHorario = APAGADO;
unsigned long tiempoFase = 0;

const unsigned long DURACION_ON = 10UL * 60UL * 1000UL;
const unsigned long DURACION_OFF = 60UL * 60UL * 1000UL;
const unsigned long DURACION_TOTAL = 10UL * 60UL * 60UL * 1000UL;

// --- Función para obtener la temperatura desde el termistor ---
double readTemperature() {
  int adcValue = analogRead(THERMISTORPIN);

  double resistance = SERIESRESISTOR / ((1023.0 / adcValue) - 1.0);
  double steinhart;
  steinhart = resistance / THERMISTORNOMINAL;
  steinhart = log(steinhart);
  steinhart /= BCOEFFICIENT;
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15);
  steinhart = 1.0 / steinhart;
  steinhart -= 273.15;

  return steinhart;
}

void setup() {
  Serial.begin(9600);
  BT.begin(9600);

  pinMode(RELE_PIN_BOMBA, OUTPUT);
  digitalWrite(RELE_PIN_BOMBA, LOW);

  lcd.init();
  lcd.backlight();
  lcd.clear();

  Serial.println(F("Sistema v3.5 iniciado"));
  BT.println(F("Sistema v3.5 iniciado"));

  tiempoInicioHorario = millis();
  horarioActivo = true;
  estadoHorario = BOMBA_ON;
  tiempoFase = millis();
  controlarBomba(true);

  Serial.println(F("Horario iniciado: Bomba ON"));
  BT.println(F("Horario iniciado: Bomba ON"));
}

void loop() {
  manejarBluetooth();
  manejarHorarioDiario();
  manejarLecturaTermistor();
  manejarPantallaLCD();
}

void controlarBomba(bool encender) {
  estadoBomba = encender;
  digitalWrite(RELE_PIN_BOMBA, encender ? HIGH : LOW);

  Serial.print(F("Bomba "));
  Serial.println(encender ? F("ENCENDIDA") : F("APAGADA"));

  BT.print(F("Bomba "));
  BT.println(encender ? F("ENCENDIDA") : F("APAGADA"));
}

void manejarBluetooth() {
  if (BT.available()) {
    comandoBT_char = BT.read();

    switch (comandoBT_char) {
      case '1':
        controlarBomba(true);
        horarioActivo = false;
        Serial.println(F("Comando BT: Encender bomba manual"));
        break;

      case '0':
        controlarBomba(false);
        horarioActivo = false;
        Serial.println(F("Comando BT: Apagar bomba manual"));
        break;

      case 'D':
      case 'd':
        enviarDatosBT();
        break;

      case '?':
        BT.print(F("Bomba estado: "));
        BT.println(estadoBomba ? "ENCENDIDA" : "APAGADA");
        break;

      default:
        break;
    }
  }
}

void enviarDatosBT() {
  if (isnan(temperatura)) {
    BT.println(F("Error al leer temperatura"));
  } else {
    BT.print(F("Temp: "));
    BT.print(temperatura, 1);
    BT.println(F(" *C"));
  }
}

void manejarHorarioDiario() {
  if (!horarioActivo) return;

  unsigned long tiempoActual = millis();
  unsigned long tiempoDesdeInicio = tiempoActual - tiempoInicioHorario;

  if (tiempoDesdeInicio >= DURACION_TOTAL) {
    if (estadoBomba) {
      controlarBomba(false);
    }
    horarioActivo = false;
    estadoHorario = APAGADO;
    Serial.println(F("Horario diario finalizado"));
    BT.println(F("Horario diario finalizado"));
    return;
  }

  unsigned long tiempoEnFase = tiempoActual - tiempoFase;

  switch (estadoHorario) {
    case BOMBA_ON:
      if (tiempoEnFase >= DURACION_ON) {
        controlarBomba(false);
        estadoHorario = BOMBA_OFF;
        tiempoFase = tiempoActual;
        Serial.println(F("Horario: Bomba OFF - 1 hora"));
        BT.println(F("Horario: Bomba OFF - 1 hora"));
      }
      break;

    case BOMBA_OFF:
      if (tiempoEnFase >= DURACION_OFF) {
        controlarBomba(true);
        estadoHorario = BOMBA_ON;
        tiempoFase = tiempoActual;
        Serial.println(F("Horario: Bomba ON - 10 minutos"));
        BT.println(F("Horario: Bomba ON - 10 minutos"));
      }
      break;

    default:
      break;
  }
}

void manejarLecturaTermistor() {
  unsigned long tiempoActual = millis();
  if (tiempoActual - tiempoAnteriorSensor >= intervaloLectura) {
    tiempoAnteriorSensor = tiempoActual;

    float temp = readTemperature();

    if (!isnan(temp)) {
      temperatura = temp;

      Serial.print(F("Temp: "));
      Serial.print(temperatura, 1);
      Serial.println(F(" °C"));
    } else {
      Serial.println(F("Error al leer termistor!"));
    }
  }
}

void manejarPantallaLCD() {
  unsigned long tiempoActual = millis();
  if (tiempoActual - tiempoAnteriorLCD >= intervaloActualizacionLCD) {
    tiempoAnteriorLCD = tiempoActual;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Temp: "));
    if (isnan(temperatura))
      lcd.print(F("--"));
    else
      lcd.print(temperatura, 1);
    lcd.print((char)223);
    lcd.print(F("C"));

    lcd.setCursor(0, 1);
    lcd.print(F("Hum: --%")); // Humedad no disponible, solo decorativo

    lcd.setCursor(11, 0);
    lcd.print(estadoBomba ? F("B:ON ") : F("B:OFF"));
  }
}
