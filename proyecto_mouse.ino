#include <Mouse.h>

#include <TimerOne.h>

#define X_AXIS_PIN A0
#define Y_AXIS_PIN A1
#define BUTTON_PIN 8
#define SENSOR_PIN 3

#define TIMER_OVERRUN_USEC 100000  // Timer overrun in usecs (defines the sensitivity of the sensor)
#define AXIS_SENSITIVITY  100      // Higher sensitivity value = slower mouse, should be <= about 500
//-----------------------------------------------------------encoder------------------------------------------------------------
int A = 2;       //variable A a pin digital 2 (DT en modulo)
int B = 4;        //variable B a pin digital 4 (CLK en modulo)

int ANTERIOR = 50;    // almacena valor anterior de la variable POSICION

volatile int POSICION = 50; // variable POSICION con valor inicial de 50 y definida
        // como global al ser usada en loop e ISR (encoder)*/
//-----------------------------------------------------------encoder------------------------------------------------------------
int vertZero, horzZero;            // Stores the initial value of each axis, usually around 512
int vertValue, horzValue;          // Stores current analog output of each axis



volatile int clickLeft = 0;



// On timer overrun
void timerIrq()
{
  clickLeft = 0; // Let's handle this in the main loop
  Timer1.stop();
}

// On rising edge of the sensor pin
void sensorIrq()
{
  TCNT1 = 1; // Timer restart without triggering the interrupt  
  if (!clickLeft)
  {
    Timer1.start(); // Start the timer if not already started
  }
  clickLeft = 1; // Let's handle this in the main loop
}

void setup()
{
  //-----------------------------------------------------------encoder------------------------------------------------------------

  pinMode(A, INPUT);    // A como entrada
  pinMode(B, INPUT);    // B como entrada

  Serial.begin(9600);   // incializacion de comunicacion serie a 9600 bps

  attachInterrupt(digitalPinToInterrupt(A), encoder , LOW);// interrupcion sobre pin A con
                // funcion ISR encoder y modo LOW
  Serial.println("Listo");  // imprime en monitor serial List
  //-----------------------------------------------------------encoder------------------------------------------------------------

  // Pin config
  pinMode(X_AXIS_PIN, INPUT);      // Set both analog pins as inputs
  pinMode(Y_AXIS_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT);      // Pin for the connected button
  pinMode(SENSOR_PIN, INPUT);      // Pin for the sensor
  digitalWrite(BUTTON_PIN, HIGH);  // Enable pull-up on button pin (active low)
  
  // Timer initialization
  Timer1.initialize(TIMER_OVERRUN_USEC);
  Timer1.attachInterrupt(timerIrq);
  attachInterrupt(0, sensorIrq, RISING);
  
  // Axis calibration
  delay(1000);                        // short delay to let outputs settle
  vertZero = analogRead(Y_AXIS_PIN);  // get the initial values
  horzZero = analogRead(X_AXIS_PIN);  // Joystick should be in neutral position when reading these

}


void loop(){
    //------------------------------------------------------encoder----------------------------------------------

  if (POSICION != ANTERIOR) { // si el valor de POSICION es distinto de ANTERIOR
    Serial.println(POSICION); // imprime valor de POSICION
    ANTERIOR = POSICION ; // asigna a ANTERIOR el valor actualizado de POSICION
  }
}

void encoder()  {
  static unsigned long ultimaInterrupcion = 0;  // variable static con ultimo valor de
            // tiempo de interrupcion
  unsigned long tiempoInterrupcion = millis();  // variable almacena valor de func. millis

  if (tiempoInterrupcion - ultimaInterrupcion > 5) {  // rutina antirebote desestima
              // pulsos menores a 5 mseg.
    if (digitalRead(B) == HIGH)     // si B es HIGH, sentido horario
    {
      char xdir=0;
  char ydir=0;
  char wheel=1;
  
      Mouse.move(xdir, ydir,wheel);       // incrementa POSICION en 1
    }
    else {
      char xdir=0;
      char ydir=0;
      char wheel=-1;
      Mouse.move(xdir, ydir, wheel) ;        // decrementa POSICION en 1
    }

  
    ultimaInterrupcion = tiempoInterrupcion;  // guarda valor actualizado del tiempo
  }           // de la interrupcion en variable static
  //------------------------------------------------------encoder----------------------------------------------
  static int mouseClickFlagRight = 0;
  static int mouseClickFlagLeft = 0;
  
  // Get ADC vals
  vertValue = (analogRead(Y_AXIS_PIN) - vertZero);  // read vertical offset
  horzValue = (analogRead(X_AXIS_PIN) - horzZero);  // read horizontal offset
  
  // Axis movement
  if (vertValue != 0)
  {
    Mouse.move(0, vertValue/AXIS_SENSITIVITY, 0);  // move mouse on y axis
  }
  
  if (horzValue != 0)
  {
    Mouse.move(horzValue/AXIS_SENSITIVITY, 0, 0);  // move mouse on x axis
  } 
  
  // Right mouse button
  if ((digitalRead(BUTTON_PIN) == 0) && (!mouseClickFlagRight))  // if the joystick button is pressed
  {
    mouseClickFlagRight = 1;
    Mouse.press(MOUSE_RIGHT);  // click the left button down
  }
  else if ((digitalRead(BUTTON_PIN))&&(mouseClickFlagRight)) // if the joystick button is not pressed
  {
    mouseClickFlagRight = 0;
    Mouse.release(MOUSE_RIGHT);  // release the left button
  }
  
  // Left mouse button
  if ((clickLeft) && (!mouseClickFlagLeft))  // if the joystick button is pressed
  {
    mouseClickFlagLeft = 1;
    Mouse.press(MOUSE_RIGHT);  // click the left button down
  }
  else if ((!clickLeft)&&(mouseClickFlagLeft)) // if the joystick button is not pressed
  {
    mouseClickFlagLeft = 0;
    Mouse.release(MOUSE_LEFT);  // release the left button
  }
  }
