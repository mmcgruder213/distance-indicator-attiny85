int ANIMDELAY = 1;  // animation delay, deafault value is 100
int INTENSITYMIN = 0; // minimum brightness, valid range [0,15]
int INTENSITYMAX = 8; // maximum brightness, valid range [0,15]
 
int DIN_PIN = 0;      // data in pin
int CS_PIN = 1;       // load (CS) pin
int CLK_PIN = 2;      // clock pin
int TRIG_PIN = 3;
int ECHO_PIN = 4;

long duration;
float distance;
 
// MAX7219 registers
byte MAXREG_DECODEMODE = 0x09;
byte MAXREG_INTENSITY  = 0x0a;
byte MAXREG_SCANLIMIT  = 0x0b;
byte MAXREG_SHUTDOWN   = 0x0c;
byte MAXREG_DISPTEST   = 0x0f;

const unsigned char ring0[] =
{
  B00000000,
  B00000000,
  B00000000,
  B00011000,
  B00011000,
  B00000000,
  B00000000,
  B00000000
};
const unsigned char ring1[] =
{
  B00000000,
  B00000000,
  B00011000,
  B00100100,
  B00100100,
  B00011000,
  B00000000,
  B00000000
};
const unsigned char ring2[] =
{
  B00000000,
  B00111100,
  B01000010,
  B01000010,
  B01000010,
  B01000010,
  B00111100,
  B00000000
};
const unsigned char ring3[] =
{
  B00111100,
  B01000010,
  B10000001,
  B10000001,
  B10000001,
  B10000001,
  B01000010,
  B00111100
};

const unsigned char ring4[] =
{
  B01111110,
  B10000001,
  B10000001,
  B10000001,
  B10000001,
  B10000001,
  B10000001,
  B01111110
};
 
 
 
void setup ()
{
  pinMode(DIN_PIN, OUTPUT);
  pinMode(CLK_PIN, OUTPUT);
  pinMode(CS_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
 
  // initialization of the MAX7219
  setRegistry(MAXREG_SCANLIMIT, 0x07);
  setRegistry(MAXREG_DECODEMODE, 0x00);  // using an led matrix (not digits)
  setRegistry(MAXREG_SHUTDOWN, 0x01);    // not in shutdown mode
  setRegistry(MAXREG_DISPTEST, 0x00);    // no display test
  setRegistry(MAXREG_INTENSITY, 0x0f & INTENSITYMIN);
 
  // draw hearth
  setRegistry(1, ring0[0]);
  setRegistry(2, ring0[1]);
  setRegistry(3, ring0[2]);
  setRegistry(4, ring0[3]);
  setRegistry(5, ring0[4]);
  setRegistry(6, ring0[5]);
  setRegistry(7, ring0[6]);
  setRegistry(8, ring0[7]);
}
 
 
void loop ()
{
  //Measure distance
   // Send a 10-microsecond pulse to the TRIG pin
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Read the time taken for the pulse to return
  duration = pulseIn(ECHO_PIN, HIGH);

  // Calculate the distance in inches
  distance = duration / 73.746 / 2;

  chooseRing(distance);
  delay(ANIMDELAY);
}
 
 
void setRegistry(byte reg, byte value)
{
  digitalWrite(CS_PIN, LOW);
 
  putByte(reg);   // specify register
  putByte(value); // send data
 
  digitalWrite(CS_PIN, LOW);
  digitalWrite(CS_PIN, HIGH);
}
 
void putByte(byte data)
{
  byte i = 8;
  byte mask;
  while (i > 0)
  {
    mask = 0x01 << (i - 1);        // get bitmask
    digitalWrite( CLK_PIN, LOW);   // tick
    if (data & mask)               // choose bit
      digitalWrite(DIN_PIN, HIGH); // send 1
    else
      digitalWrite(DIN_PIN, LOW);  // send 0
    digitalWrite(CLK_PIN, HIGH);   // tock
    --i;                           // move to lesser bit
  }
}

void chooseRing(float distance)
{
  //Combine rings based on distance
  int brightness = 0;
  int numRings = 0;
  unsigned char ring[] =
  {
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000
  };
  // ring 4
  if (distance < 6.33) {
    memcpy(ring, addRing(ring, ring4), sizeof(ring));
    numRings++;
  }
  
  // ring 3
  if (distance > 4.33 & distance < 10.33) {
    memcpy(ring, addRing(ring, ring3), sizeof(ring));
    numRings++;
  }

  // ring 2
  if (distance > 8.33 & distance < 14.33) {
    memcpy(ring, addRing(ring, ring2), sizeof(ring));
    numRings++;
  }

  // ring 1
  if (distance > 12.33 & distance < 18.33) {
    memcpy(ring, addRing(ring, ring1), sizeof(ring));
    numRings++;
  }

  // ring 0
  if (distance > 16.33) {
    memcpy(ring, addRing(ring, ring0), sizeof(ring));
    numRings++;
  }

  //Set the brightness of the LEDs
  if (distance < 6)
  {
    distance = 6;
  }
  else if (distance > 30)
  {
    distance = 30;
  }
  brightness = map((int) 100*distance, 3000, 600, 0, 8);
  if (brightness < 0)
  {
    brightness = 0;
  }

  drawRing(ring, brightness);
}

void drawRing(unsigned char ring[], int intensity)
{
    setRegistry(1, ring[0]);
    setRegistry(2, ring[1]);
    setRegistry(3, ring[2]);
    setRegistry(4, ring[3]);
    setRegistry(5, ring[4]);
    setRegistry(6, ring[5]);
    setRegistry(7, ring[6]);
    setRegistry(8, ring[7]);
    setRegistry(MAXREG_INTENSITY, 0x0f & intensity);
}

unsigned char* addRing(unsigned char init_ring[], unsigned char change_ring[])
{
  static unsigned char output[] =
  {
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000
  };

  for (int i = 0; i < 8; i++)
  {
    output[i] = init_ring[i] | change_ring[i];
  }

  return output;
}
