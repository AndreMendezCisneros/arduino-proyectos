# ⚙️ Proyectos Arduino – Villa Automation E.I.R.L.

Este repositorio reúne una colección de proyectos escritos en C++ para el entorno Arduino, desarrollados por **Andre Mendez** como parte de las prácticas preprofesionales en **Villa Automation E.I.R.L.**.

Los proyectos están organizados por tipo de aplicación y versión. La mayoría están enfocados en el control de motores: **DC, brushless, y paso a paso**, así como sensores y comunicación inalámbrica.

---

## 📂 Estructura del repositorio

Cada carpeta representa un proyecto. Las subversiones (`v0.1`, `v0.2`, etc.) indican avances, pruebas o mejoras progresivas:


---

## 🔧 Tecnologías y componentes utilizados

- **Lenguaje**: C++ con Arduino IDE
- **Plataformas**: Arduino UNO, Nano, Mega
- **Motores**:
  - DC
  - Brushless (mediante controladores ESC)
  - Paso a paso (con librería Stepper o A4988)
- **Sensores**:
  - DHT11/DHT22 (temperatura y humedad)
  - DS18B20 (temperatura digital)
  - Sensor de proximidad
- **Módulos**:
  - Bluetooth HC-05 / HC-06
  - Pantalla LCD
  - Relés

---

## 🚀 Cómo usar estos proyectos

1. Abre el **Arduino IDE**.
2. Ve a **Archivo > Abrir...** y selecciona el archivo `.ino` correspondiente.
3. Asegúrate de tener instalada la librería necesaria si el código lo requiere.
4. Conecta tu placa Arduino y carga el programa.

> ⚠️ **Revisa la versión del proyecto antes de cargarlo**, ya que pueden haber cambios en la lógica o el hardware utilizado.

---

## 📌 Ejemplos destacados

| Proyecto                         | Tipo de motor | Descripción breve                                      |
|----------------------------------|---------------|---------------------------------------------------------|
| `motorbase`                      | DC            | Control básico de encendido y giro de motor            |
| `motor_brazo_sensor_0.1`         | DC + sensor   | Motor con sensor de proximidad                        |
| `robot_motor_brushless_0.1`      | Brushless     | Control de robot con motores brushless                |
| `motor_ruedas_0.1`               | DC            | Sistema de ruedas con motor DC                        |
| `prueba_conexion_blueetoh_0.1`   | -             | Comunicación entre Arduino y app móvil por Bluetooth  |
| `pantalla_temperatura_0.1`       | -             | Visualización de temperatura en pantalla LCD          |

---

## 🧑‍💻 Autor

**Andre Mendez**  
Estudiante de Ingeniería de Sistemas  
Desarrollador de proyectos electrónicos en prácticas preprofesionales.  
[GitHub: @AndreMendezCisneros](https://github.com/AndreMendezCisneros)

---

## 📄 Licencia

Este repositorio se distribuye bajo la licencia **MIT**. Puedes usar y modificar los proyectos con fines educativos o profesionales, dando el debido crédito.

---

## ✅ Pendientes y mejoras futuras

- [ ] Documentación técnica de cada proyecto (`.pdf` o `.md`)
- [ ] Diagramas de conexión (fritzing o esquemas)
- [ ] Capturas de pruebas reales o videos
- [ ] Integración con sensores avanzados y control PID

