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
// NOTA: Conecta el pin de señal del KY-013 al pin A0
#define THERMISTORPIN A0        // Pin analógico donde está conectado el KY-013
#define SERIESRESISTOR 10000    // Resistencia fija en ohmios (10k)
#define THERMISTORNOMINAL 10000 // Valor del termistor a 25°C
#define TEMPERATURENOMINAL 25   // Temperatura nominal (25°C)
#define BCOEFFICIENT 3950       // Coeficiente B del termistor

// --- Definiciones de la Pantalla LCD I2C ---
LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- Variables Globales Generales ---
char comandoBT_char;
bool estadoBomba = false;
unsigned long tiempoAnteriorLecturaTemp = 0; // Usado para el KY-013
const long intervaloLecturaTemp = 2500;      // Intervalo de lectura para el KY-013
unsigned long tiempoAnteriorLCD = 0;
const long intervaloActualizacionLCD = 1000;
float ultimaTemp = -999.0;
// float ultimaHum = -999.0; // Ya no se necesita para la lectura del sensor, solo para el envío BT
bool ultimoEstadoBombaLCD = !estadoBomba;

// --- Variables para el encendido temporizado por comando 'S<min>' ---
unsigned long tiempoEncendidoBombaProgramado_S_Cmd;
bool bombaProgramada_S_Cmd_Activa = false;
unsigned long duracionActivadaProgramadaMs_S_Cmd = 10 * 60 * 1000UL;

// --- Variables y Constantes para el Horario Diario (8AM - 6PM simulado) ---
bool dailyScheduleActive = false;
unsigned long dailyScheduleStartTime; // Momento (millis) en que inició el ciclo de 10 horas
unsigned long cyclePhaseStartTime_Daily; // Momento (millis) en que inició la fase ON o OFF actual

enum DailyPhase {
  D_IDLE,          // Horario inactivo o fuera de la ventana 8AM-6PM
  D_PUMP_ON,       // Bomba encendida por 10 minutos
  D_PUMP_OFF_WAIT  // Bomba apagada, esperando 1 hora
};
DailyPhase currentDailyPhase = D_IDLE;

const unsigned long DAILY_PUMP_ON_DUR_MS = 10 * 60 * 1000UL;       // 10 minutos
const unsigned long DAILY_PUMP_OFF_WAIT_DUR_MS = 1 * 60 * 60 * 1000UL; // 1 hora
const unsigned long TOTAL_DAILY_ACTIVE_DURATION_MS = 10UL * 60 * 60 * 1000; // 10 horas (8AM a 6PM)

// --- Declaración de Funciones ---
void inicializarSeriales();
void inicializarPines();
// void inicializarSensores(); // ELIMINADO: No se usa para KY-013
void inicializarLCD();
void controlarBomba(bool encender, bool esComandoManual);
void procesarComandosBluetooth();
void enviarEstadoBombaBT();
double readTemperatureKY013(); // Nueva función para leer temperatura del KY-013
// void leerDatosDHT(float &temperatura, float &humedad, bool &error); // ELIMINADO
void enviarDatosSensorBT(float temperatura); // Actualizada para el KY-013 y compatibilidad de formato
void actualizarPantallaLCD(float temperatura); // Actualizada para solo temperatura del KY-013
void limpiarLineaLCD(int linea);
void manageDailySchedule();

void setup() {
  inicializarSeriales();
  inicializarPines();
  // inicializarSensores(); // No se necesita, el KY-013 no requiere inicialización específica de librería
  inicializarLCD();

  controlarBomba(false, false);

  Serial.println(F("Sistema v3.5 - KY-013: Horario Diario (8AM-6PM simulado)."));
  BT.println(F("BT v3.5 - KY-013: Horario 8AM-6PM (10h desde inicio)."));
  BT.println(F("IMPORTANTE: REINICIAR ARDUINO A LAS 8AM DIARIAMENTE."));
  BT.println(F("Cmds manuales ('1','0','S<min>') anulan horario."));
  BT.println(F("Otros: '?':Estado, 'D':Datos"));

  // Iniciar el Horario Diario (asumiendo que setup() se ejecuta a las 8 AM)
  dailyScheduleActive = true;
  dailyScheduleStartTime = millis();
  currentDailyPhase = D_PUMP_ON; // Inicia con la bomba encendida
  cyclePhaseStartTime_Daily = millis();
  controlarBomba(true, false);
  Serial.println(F("Horario diario iniciado (simula 8AM): Bomba ON por 10 min."));
  BT.println(F("Horario (8AM): Bomba ON (10 min)."));

  // Asegurar que el temporizador S<cmd> no esté activo al inicio del horario
  bombaProgramada_S_Cmd_Activa = false;

  // Realiza la primera lectura de temperatura del KY-013 y actualiza LCD
  ultimaTemp = readTemperatureKY013();
  actualizarPantallaLCD(ultimaTemp);
}

void loop() {
  procesarComandosBluetooth();
  manageDailySchedule();

  unsigned long tiempoActual = millis();

  // Lógica para el temporizador S<min> (si está activo por comando manual)
  if (bombaProgramada_S_Cmd_Activa && !dailyScheduleActive && estadoBomba) {
    if (tiempoActual - tiempoEncendidoBombaProgramado_S_Cmd >= duracionActivadaProgramadaMs_S_Cmd) {
      Serial.println(F("Fin ciclo S<min>: Bomba APAGADA."));
      BT.println(F("Bomba APAGADA (fin S<min>)."));
      controlarBomba(false, false);
      bombaProgramada_S_Cmd_Activa = false;
    }
  }

  // Lectura de Temperatura del KY-013 (reemplaza la lectura DHT)
  if (tiempoActual - tiempoAnteriorLecturaTemp >= intervaloLecturaTemp) {
    tiempoAnteriorLecturaTemp = tiempoActual;
    ultimaTemp = readTemperatureKY013(); // Llama a la función del KY-013
    enviarDatosSensorBT(ultimaTemp);     // Llama a la función de envío compatible
  }

  // Actualización del LCD
  if (tiempoActual - tiempoAnteriorLCD >= intervaloActualizacionLCD) {
    tiempoAnteriorLCD = tiempoActual;
    actualizarPantallaLCD(ultimaTemp); // Solo necesita la temperatura
  }
}

void manageDailySchedule() {
    if (!dailyScheduleActive) {
        return; // Horario diario no activo
    }

    unsigned long currentTime = millis();

    // Verificar si han pasado las 10 horas del horario activo (simulando 8AM a 6PM)
    if (currentTime - dailyScheduleStartTime >= TOTAL_DAILY_ACTIVE_DURATION_MS) {
        Serial.println(F("Horario diario (10h) FINALIZADO (simula 6PM)."));
        BT.println(F("Horario (6PM): FINALIZADO. Reiniciar mañana 8AM."));
        if (estadoBomba) {
            controlarBomba(false, false); // Apagar bomba si estaba encendida
        }
        dailyScheduleActive = false;
        currentDailyPhase = D_IDLE;
        ultimoEstadoBombaLCD = !estadoBomba;
        return;
    }

    // Gestión de fases del ciclo diario
    unsigned long phaseElapsedTime = currentTime - cyclePhaseStartTime_Daily;

    switch (currentDailyPhase) {
        case D_PUMP_ON:
            if (phaseElapsedTime >= DAILY_PUMP_ON_DUR_MS) {
                Serial.println(F("Horario: Fin 10min ON. Esperando 1h."));
                BT.println(F("Horario: Bomba OFF. Esperando 1h."));
                controlarBomba(false, false);
                currentDailyPhase = D_PUMP_OFF_WAIT;
                cyclePhaseStartTime_Daily = currentTime;
                ultimoEstadoBombaLCD = !estadoBomba;
            }
            break;

        case D_PUMP_OFF_WAIT:
            if (phaseElapsedTime >= DAILY_PUMP_OFF_WAIT_DUR_MS) {
                Serial.println(F("Horario: Fin espera 1h. Bomba ON por 10 min."));
                BT.println(F("Horario: Bomba ON (10 min)."));
                controlarBomba(true, false);
                currentDailyPhase = D_PUMP_ON;
                cyclePhaseStartTime_Daily = currentTime;
                ultimoEstadoBombaLCD = !estadoBomba;
            }
            break;
        
        case D_IDLE:
            break;
    }
}

void inicializarSeriales() {
  Serial.begin(9600); BT.begin(9600);
  Serial.println(F("Monitor Serial Iniciado. BT Iniciado."));
}

void inicializarPines() {
  pinMode(RELE_PIN_BOMBA, OUTPUT);
  Serial.print(F("Pin Rele Bomba (")); Serial.print(RELE_PIN_BOMBA); Serial.println(F(") OUT."));
}

// ELIMINADA: La función inicializarSensores() que era para el DHT ya no es necesaria.
// void inicializarSensores() {
//   dht.begin(); Serial.println(F("Sensor DHT Iniciado."));
// }

void inicializarLCD() {
  Wire.begin(); lcd.init(); lcd.backlight(); lcd.clear();
  lcd.setCursor(0, 0); lcd.print(F("Sistema Bomba v3.5"));
  lcd.setCursor(0, 1); lcd.print(F("KY-013 8AM-6PM")); // Mensaje actualizado para el LCD
  Serial.println(F("LCD I2C Iniciado."));
  delay(1500);
}

void controlarBomba(bool encender, bool esComandoManual) {
  bool cambioDeEstado = (estadoBomba != encender);
  if (encender) {
    digitalWrite(RELE_PIN_BOMBA, HIGH); estadoBomba = true;
    if (cambioDeEstado) Serial.println(F("Bomba ENCENDIDA"));
    if (esComandoManual && cambioDeEstado) BT.println(F("Bomba ENCENDIDA (manual)"));
  } else {
    digitalWrite(RELE_PIN_BOMBA, LOW); estadoBomba = false;
    if (cambioDeEstado) Serial.println(F("Bomba APAGADA"));
    if (esComandoManual && cambioDeEstado) BT.println(F("Bomba APAGADA (manual)"));
  }
  if (cambioDeEstado) ultimoEstadoBombaLCD = !estadoBomba;
}

void procesarComandosBluetooth() {
  if (BT.available()) {
    comandoBT_char = (char)BT.read();
    bool comandoCancelaTodo = (comandoBT_char == '1' || comandoBT_char == '0' || comandoBT_char == 'S' || comandoBT_char == 's');

    if (comandoCancelaTodo) {
        if (dailyScheduleActive) {
            Serial.println(F("Comando manual: Horario diario CANCELADO."));
            BT.println(F("Horario diario CANCELADO por cmd manual."));
            dailyScheduleActive = false;
            currentDailyPhase = D_IDLE;
        }
        if (bombaProgramada_S_Cmd_Activa) {
            bombaProgramada_S_Cmd_Activa = false; // Cancela temporizador S<min> si estaba activo
        }
    }

    if (comandoBT_char == 'S' || comandoBT_char == 's') {
      BT.setTimeout(1000); long minutosProgramados = BT.parseInt();
      if (minutosProgramados > 0 && minutosProgramados <= 999) {
        duracionActivadaProgramadaMs_S_Cmd = minutosProgramados * 60 * 1000UL;
        controlarBomba(true, false); // No es '1' manual, es temporizado 'S'
        if (estadoBomba) {
            bombaProgramada_S_Cmd_Activa = true;
            tiempoEncendidoBombaProgramado_S_Cmd = millis();
            BT.print(F("Bomba ON (S<min>) por ")); BT.print(minutosProgramados); BT.println(F(" min."));
        } else { BT.println(F("Error: Bomba no pudo iniciar S<min>.")); }
      } else { BT.println(F("Error S<min>: Duracion invalida (1-999).")); }
      while(BT.available()) BT.read();
    } else if (comandoBT_char == '1') {
      controlarBomba(true, true);
    } else if (comandoBT_char == '0') {
      controlarBomba(false, true);
    } else if (comandoBT_char == '?') {
      enviarEstadoBombaBT();
      while(BT.available()) { char c = BT.read(); if (c == '\n') break;}
    } else if (comandoBT_char == 'D' || comandoBT_char == 'd') {
      // AQUÍ SE LLAMA A LAS FUNCIONES DEL KY-013 EN LUGAR DEL DHT
      ultimaTemp = readTemperatureKY013(); // Lee la temperatura del KY-013
      enviarDatosSensorBT(ultimaTemp);     // Envía la temperatura (con humedad simulada para la app)
      while(BT.available()) { char c = BT.read(); if (c == '\n') break;}
    } else {
      String cmdCompleto = String(comandoBT_char); BT.setTimeout(50);
      while(BT.available()){ char c = BT.read(); if(c == '\n' || c == '\r') break; cmdCompleto += c; }
      Serial.print(F("Cmd BT no reconocido: ")); Serial.println(cmdCompleto);
      BT.print(F("Cmd '")); BT.print(cmdCompleto); BT.println(F("' no reconocido."));
    }
  }
}

void enviarEstadoBombaBT() {
  if (estadoBomba) { BT.print(F("Estado: Bomba ENCENDIDA")); }
  else { BT.print(F("Estado: Bomba APAGADA")); }

  if (dailyScheduleActive) {
    BT.print(F(" (Horario Diario ACTIVO 8AM-6PM - "));
    switch(currentDailyPhase) {
        case D_PUMP_ON: BT.print(F("Regando")); break;
        case D_PUMP_OFF_WAIT: BT.print(F("Esperando")); break;
        default: BT.print(F("Fase Desconocida")); break;
    }
    // Info de tiempo restante de fase y horario total
    unsigned long phaseElapsedTimeMs = millis() - cyclePhaseStartTime_Daily;
    unsigned long currentPhaseTotalDurMs = (currentDailyPhase == D_PUMP_ON) ? DAILY_PUMP_ON_DUR_MS : DAILY_PUMP_OFF_WAIT_DUR_MS;
    if (currentPhaseTotalDurMs > phaseElapsedTimeMs) {
        unsigned long phaseRemainingS = (currentPhaseTotalDurMs - phaseElapsedTimeMs) / 1000;
        BT.print(F(", Fase Resta: ~")); BT.print(phaseRemainingS / 60); BT.print("m"); BT.print(phaseRemainingS % 60); BT.print("s");
    }
    unsigned long scheduleElapsedTimeMs = millis() - dailyScheduleStartTime;
    if (TOTAL_DAILY_ACTIVE_DURATION_MS > scheduleElapsedTimeMs) {
        unsigned long scheduleRemainingS = (TOTAL_DAILY_ACTIVE_DURATION_MS - scheduleElapsedTimeMs) / 1000;
        BT.print(F(" | Horario Fin en: ~")); BT.print(scheduleRemainingS / 3600); BT.print("h");
        BT.print((scheduleRemainingS % 3600) / 60);
        BT.print("m");
    }
    BT.println(F(")"));
  } else if (bombaProgramada_S_Cmd_Activa) {
    unsigned long transcurridoMs = millis() - tiempoEncendidoBombaProgramado_S_Cmd;
    if (duracionActivadaProgramadaMs_S_Cmd > transcurridoMs) {
        unsigned long restante_s = (duracionActivadaProgramadaMs_S_Cmd - transcurridoMs) / 1000;
        BT.print(F(" (Timer S<min>, restante: ~"));
        BT.print(restante_s / 60); 
        BT.print("m"); BT.print(restante_s % 60); 
        BT.print("s");
        BT.println(F(")"));
    } else { BT.println(F(" (Timer S<min> finalizando)")); }
  } else { // Ni horario diario, ni S<min> activo
    BT.println(F(" (Manual/Inactivo)"));
  }
}

// Nueva función para leer temperatura del KY-013 (correcta)
double readTemperatureKY013() {
  int adcValue = analogRead(THERMISTORPIN);

  double resistance = SERIESRESISTOR / ((1023.0 / adcValue) - 1.0);
  double steinhart;
  steinhart = resistance / THERMISTORNOMINAL;           // (R/Ro)
  steinhart = log(steinhart);                            // ln(R/Ro)
  steinhart /= BCOEFFICIENT;                             // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15);      // + (1/To)
  steinhart = 1.0 / steinhart;                           // Invertir
  steinhart -= 273.15;                                   // Kelvin a Celsius

  Serial.print(F("Temperatura KY-013: "));
  Serial.println(steinhart, 1); // Imprime con 1 decimal
  return steinhart;
}

// ELIMINADA: La función leerDatosDHT() es para el DHT, no para el KY-013.
// void leerDatosDHT(float &temperatura, float &humedad, bool &errorLectura) {
//   humedad = dht.readHumidity(); temperatura = dht.readTemperature();
//   errorLectura = isnan(humedad) || isnan(temperatura);
//   if (errorLectura) Serial.println(F("Error al leer del sensor DHT!"));
// }

// MODIFICADA: Esta función ahora recibe solo temperatura y envía el formato de DHT con humedad simulada
void enviarDatosSensorBT(float temperatura) {
  float humedadSimulada = 0.0; // O 50.0, etc.

  // Recrea el formato exacto que enviaba el DHT, incluyendo la humedad simulada
  String dataString = "T:" + String(temperatura, 1) + "C H:" + String(humedadSimulada, 1) + "%";
  
  Serial.print(F("USB: ")); Serial.println(dataString);
  // Mantén el prefijo "BT_DHT:" para compatibilidad con la aplicación
  BT.print(F("BT_DHT: ")); 
  BT.println(dataString);
}

void limpiarLineaLCD(int linea) {
  lcd.setCursor(0, linea); for (int i = 0; i < 16; i++) lcd.print(" ");
}

// MODIFICADA: Solo necesita la temperatura para el LCD
void actualizarPantallaLCD(float temperatura) {
  limpiarLineaLCD(0); lcd.setCursor(0, 0);
  if (temperatura > -999.0) { // Valor centinela para indicar error en lectura
    String lcdDataString = "T:";
    lcdDataString += String(temperatura, 1);
    lcdDataString += (char)223; // Carácter de grado
    lcdDataString += "C";
    lcd.print(lcdDataString);
  } else {
    lcd.print(F("Error KY-013!"));
  }

  // Lógica de la línea 1 (estado de la bomba) sigue igual
  if (ultimoEstadoBombaLCD != estadoBomba || dailyScheduleActive || bombaProgramada_S_Cmd_Activa) {
    limpiarLineaLCD(1); lcd.setCursor(0, 1); lcd.print(F("Bomba: "));
    if (estadoBomba) {
      lcd.print(F("ON "));
      if (dailyScheduleActive) { lcd.print(F("(Dia)")); } // Horario Diario
      else if (bombaProgramada_S_Cmd_Activa) { lcd.print(F("(Timer)")); }
    } else { // Bomba OFF
      lcd.print(F("OFF"));
      if (dailyScheduleActive && currentDailyPhase == D_PUMP_OFF_WAIT) { lcd.print(F("(Dia Esp)")); } // Espera del Horario Diario
      else if (dailyScheduleActive && currentDailyPhase == D_IDLE) { lcd.print(F("(Dia Fin)"));} // Horario Diario Terminado
      else if (!dailyScheduleActive && !bombaProgramada_S_Cmd_Activa) { /*lcd.print(F("(Manual)"));*/ } // Opcional
    }
    ultimoEstadoBombaLCD = estadoBomba;
  }
}
