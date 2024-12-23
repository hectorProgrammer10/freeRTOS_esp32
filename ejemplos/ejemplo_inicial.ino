#include <Arduino_FreeRTOS.h>

// Declaración de tareas
void Task1(void *pvParameters);
void Task2(void *pvParameters);

void setup()
{
  // Crear tareas
  xTaskCreate(Task1, "Task 1", 128, NULL, 1, NULL);
  xTaskCreate(Task2, "Task 2", 128, NULL, 1, NULL);
}

void loop()
{
  // No se usa el loop en FreeRTOS; todo se maneja con tareas
}

void Task1(void *pvParameters)
{
  while (true)
  {
    Serial.println("Task 1 ejecutándose");
    vTaskDelay(1000 / portTICK_PERIOD_MS); // Retraso de 1 segundo
  }
}

void Task2(void *pvParameters)
{
  while (true)
  {
    Serial.println("Task 2 ejecutándose");
    vTaskDelay(2000 / portTICK_PERIOD_MS); // Retraso de 2 segundos
  }
}
