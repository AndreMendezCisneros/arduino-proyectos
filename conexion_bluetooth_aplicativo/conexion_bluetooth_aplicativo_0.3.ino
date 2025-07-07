#include <SoftwareSerial.h>
// #include <DHT.h> // COMENTADO/ELIMINADO: Ya no se usa el sensor DHT
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// --- Definiciones de SoftwareSerial para Bluetooth ---
const int BT_RX_PIN = 2;
const int BT_TX_PIN = 3;
SoftwareSerial BT(BT_RX_PIN, BT_TX_PIN);

// --- Definición del Pin del Relé (Bomba de Agua) ---
const int RELE_PIN_BOMBA = 9;

// --- Definiciones del Sensor KY-013 (Termistor) ---
#define THERMISTORPIN A0
#define SERIESRESISTOR 10000
#define THERMISTORNOMINAL 10000
#define TEMPERATURENOMINAL 25
#define BCOEFFICIENT 3950

// --- Definiciones de la Pantalla LCD I2C ---
LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- Variables Globales Generales ---
char comandoBT_char;
bool estadoBomba = false;
unsigned long tiempoAnteriorLecturaTemp = 0;
const long intervaloLecturaTemp = 2500;
unsigned long tiempoAnteriorLCD = 0;
const long intervaloActualizacionLCD = 1000;
float ultimaTemp = -999.0;
bool ultimoEstadoBombaLCD = !estadoBomba;

// --- Variables para el encendido temporizado por comando 'S<min>' ---
unsigned long tiempoEncendidoBombaProgramado_S_Cmd;
bool bombaProgramada_S_Cmd_Activa = false;
unsigned long duracionActivadaProgramadaMs_S_Cmd = 10 * 60 * 1000UL;

// --- Variables y Constantes para el Horario Diario (8AM - 6PM simulado) ---
bool dailyScheduleActive = false;
unsigned long dailyScheduleStartTime;
unsigned long cyclePhaseStartTime_Daily;

enum DailyPhase {
  D_IDLE,
  D_PUMP_ON,
  D_PUMP_OFF_WAIT
};
DailyPhase currentDailyPhase = D_IDLE;

const unsigned long DAILY_PUMP_ON_DUR_MS = 10 * 60 * 1000UL;
const unsigned long DAILY_PUMP_OFF_WAIT_DUR_MS = 1 * 60 * 60 * 1000UL;
const unsigned long TOTAL_DAILY_ACTIVE_DURATION_MS = 10UL * 60 * 60 * 1000;

// --- Declaración de Funciones ---
void inicializarSeriales();
void inicializarPines();
void inicializarLCD();
void controlarBomba(bool encender, bool esComandoManual);
void procesarComandosBluetooth();
void enviarEstadoBombaBT();
double readTemperatureKY013();
void enviarDatosSensorBT(float temperatura);
void actualizarPantallaLCD(float temperatura);
void limpiarLineaLCD(int linea);
void manageDailySchedule();

void setup() {
  inicializarSeriales();
  inicializarPines();
  inicializarLCD();
  controlarBomba(false, false);

  Serial.println(F("Sistema v3.5 - KY-013: Horario Diario (8AM-6PM simulado)."));
  BT.println(F("BT v3.5 - KY-013: Horario 8AM-6PM (10h desde inicio)."));
  BT.println(F("IMPORTANTE: REINICIAR ARDUINO A LAS 8AM DIARIAMENTE."));
  BT.println(F("Cmds manuales ('1','0','S<min>') anulan horario."));
  BT.println(F("Otros: '?':Estado, 'D':Datos"));

  dailyScheduleActive = true;
  dailyScheduleStartTime = millis();
  currentDailyPhase = D_PUMP_ON;
  cyclePhaseStartTime_Daily = millis();
  controlarBomba(true, false);
  Serial.println(F("Horario diario iniciado (simula 8AM): Bomba ON por 10 min."));
  BT.println(F("Horario (8AM): Bomba ON (10 min)."));
  bombaProgramada_S_Cmd_Activa = false;
  ultimaTemp = readTemperatureKY013();
  actualizarPantallaLCD(ultimaTemp);
}

void loop() {
  procesarComandosBluetooth();
  manageDailySchedule();
  unsigned long tiempoActual = millis();

  if (bombaProgramada_S_Cmd_Activa && !dailyScheduleActive && estadoBomba) {
    if (tiempoActual - tiempoEncendidoBombaProgramado_S_Cmd >= duracionActivadaProgramadaMs_S_Cmd) {
      Serial.println(F("Fin ciclo S<min>: Bomba APAGADA."));
      BT.println(F("Bomba APAGADA (fin S<min>)."));
      controlarBomba(false, false);
      bombaProgramada_S_Cmd_Activa = false;
    }
  }

  if (tiempoActual - tiempoAnteriorLecturaTemp >= intervaloLecturaTemp) {
    tiempoAnteriorLecturaTemp = tiempoActual;
    ultimaTemp = readTemperatureKY013();
    enviarDatosSensorBT(ultimaTemp);
  }

  if (tiempoActual - tiempoAnteriorLCD >= intervaloActualizacionLCD) {
    tiempoAnteriorLCD = tiempoActual;
    actualizarPantallaLCD(ultimaTemp);
  }
}

void procesarComandosBluetooth() {
  if (BT.available()) {
    comandoBT_char = BT.read();

    if (comandoBT_char == '1') {
      dailyScheduleActive = false;
      controlarBomba(true, true);
    } else if (comandoBT_char == '0') {
      dailyScheduleActive = false;
      controlarBomba(false, true);
    } else if (comandoBT_char == '?') {
      enviarEstadoBombaBT();
    } else if (comandoBT_char == 'D') {
      enviarDatosSensorBT(ultimaTemp);
    } else if (comandoBT_char == 'S') {
      String strTiempo = "";
      while (BT.available()) {
        char c = BT.read();
        if (isDigit(c)) {
          strTiempo += c;
        } else {
          break;
        }
      }
      if (strTiempo.length() > 0) {
        int minutos = strTiempo.toInt();
        if (minutos > 0 && minutos <= 120) {
          duracionActivadaProgramadaMs_S_Cmd = minutos * 60UL * 1000UL;
          tiempoEncendidoBombaProgramado_S_Cmd = millis();
          dailyScheduleActive = false;
          bombaProgramada_S_Cmd_Activa = true;
          controlarBomba(true, true);
          BT.print(F("Bomba ENCENDIDA por ")); BT.print(minutos); BT.println(F(" minutos (S<min>)"));
          Serial.print(F("Cmd S<min>: Encendido por ")); Serial.print(minutos); Serial.println(F(" minutos"));
        } else {
          BT.println(F("Error: S<min> debe ser entre 1 y 120"));
        }
      }
    }
  }
}

void enviarEstadoBombaBT() {
  BT.print(F("Estado Bomba: "));
  BT.println(estadoBomba ? F("ENCENDIDA") : F("APAGADA"));
}

double readTemperatureKY013() {
  int analogValue = analogRead(THERMISTORPIN);
  double resistance = SERIESRESISTOR / ((1023.0 / analogValue) - 1.0);
  double steinhart;
  steinhart = resistance / THERMISTORNOMINAL;
  steinhart = log(steinhart);
  steinhart /= BCOEFFICIENT;
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15);
  steinhart = 1.0 / steinhart;
  steinhart -= 273.15;
  return steinhart;
}

void enviarDatosSensorBT(float temperatura) {
  BT.print(F("Temp: ")); BT.print(temperatura, 1); BT.println(F(" °C"));
}

void actualizarPantallaLCD(float temperatura) {
  lcd.setCursor(0, 0);
  lcd.print(F("Temp: "));
  lcd.print(temperatura, 1);
  lcd.print(F(" C     "));
  lcd.setCursor(0, 1);
  lcd.print(F("Bomba: "));
  lcd.print(estadoBomba ? F("ENCENDIDA ") : F("APAGADA   "));
}

void limpiarLineaLCD(int linea) {
  lcd.setCursor(0, linea);
  for (int i = 0; i < 16; i++) lcd.print(" ");
}
