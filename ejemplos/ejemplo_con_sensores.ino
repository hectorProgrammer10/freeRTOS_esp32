#include <WiFi.h>
#include <HTTPClient.h>

// Definir los pines a utilizar
#define trigPin 12
#define echoPin 14

int contador = 0;
unsigned long tiempoAnterior = 0;
const unsigned long tiempoEspera = 5000;

// Configura la conexión WiFi
const char *ssid = "sobres";
const char *password = "1234567890";
const char *serverName = "https://api-node-scoore.vercel.app/api/data";

void setup()
{
  Serial.begin(9600);

  // Configurar los pines de Trig y Echo
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Crear tareas para los hilos
  xTaskCreatePinnedToCore(
      connectToWiFi, // Función de la tarea
      "WiFiTask",    // Nombre de la tarea
      10000,         // Tamaño de la pila
      NULL,          // Parámetro de entrada
      1,             // Prioridad de la |tarea
      NULL,          // Handle de la tarea
      0);            // Núcleo donde se ejecutará la tarea

  xTaskCreatePinnedToCore(
      measureDistance, // Función de la tarea
      "MeasureTask",   // Nombre de la tarea
      10000,           // Tamaño de la pila
      NULL,            // Parámetro de entrada
      1,               // Prioridad de la tarea
      NULL,            // Handle de la tarea
      1);              // Núcleo donde se ejecutará la tarea
}

void loop()
{
  // El loop principal no se utiliza cuando usamos freeRTOS
}

void connectToWiFi(void *parameter)
{
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi!");
  vTaskDelete(NULL); // Eliminar la tarea una vez que se haya conectado
}

void measureDistance(void *parameter)
{
  while (true)
  {
    // Generar un pulso de 10 microsegundos en el pin Trig para iniciar la medición
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Leer el tiempo que tarda el pulso en volver (en microsegundos)
    unsigned long tiempoVuelta = pulseIn(echoPin, HIGH, 40000); // 40 ms de tiempo máximo de espera (400 cm a 343 m/s)

    // Convertir el tiempo en distancia (en centímetros)
    float distancia_cm = tiempoVuelta * 0.01715; // Dividir por 2 * velocidad del sonido (343 m/s)

    Serial.print("Tiempo Vuelta (μs): ");
    Serial.print(tiempoVuelta);
    Serial.print(", Distancia (cm): ");
    Serial.println(distancia_cm);

    if (distancia_cm < 10)
    {
      contador++;
      tiempoAnterior = millis();

      // Enviar el valor del contador a la API
      if (WiFi.status() == WL_CONNECTED)
      {
        HTTPClient http;
        http.begin(serverName);
        http.addHeader("Content-Type", "application/json");

        // Datos a enviar en formato JSON
        String jsonData = "{\"sensor\":\"ultrasonico\",\"valor\":" + String(contador) + "}";

        int httpResponseCode = http.POST(jsonData);

        if (httpResponseCode > 0)
        {
          String response = http.getString();
          Serial.println(httpResponseCode);
          Serial.println(response);
        }
        else
        {
          Serial.print("Error on sending POST: ");
          Serial.println(httpResponseCode);
        }

        http.end();
      }

      delay(tiempoEspera);
    }

    // Imprimir el valor del contador
    Serial.print("Contador: ");
    Serial.println(contador);

    delay(50);
  }
}
