#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3);
//SoftwareSerial mySerial(8, 9);  // RX, TX

#include <Adafruit_Fingerprint.h>
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
uint8_t id;

#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
 // LCD connected to A0, A1, A2, A3, A4, A5
 // RX, TX for finger (pins 11 and 12)


#define enroll 11
#define del 12
#define up 4
#define down 5
#define match 6
#define sw1 7
#define sw2 8
#define sw3 9
#define resultsw 10
#define buzzer 13
int records = 25;
int var[25];  // Create an array of 99 integers
int FP_value;
int vote1, vote2, vote3, vote;

void setup() {
   lcd.begin();           // Initialize the LCD
  lcd.backlight();  
  delay(500);
  lcd.setCursor(1, 0);
  lcd.print("Voting Machine Mby");
  lcd.setCursor(3, 1);
  lcd.print("Finger Print");
  delay(2000);

  lcd.clear();
  lcd.print("Finding Module");
  lcd.setCursor(0, 1);
  delay(1000);

  Serial.begin(9600);
  finger.begin(57600);
  mySerial.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Found Module ");
    delay(1000);
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("module not Found");
    lcd.setCursor(0, 2);
    lcd.print("Check Connections");
    while (1) { delay(1); }
  }

  pinMode(enroll, INPUT_PULLUP);
  pinMode(up, INPUT_PULLUP);
  pinMode(down, INPUT_PULLUP);
  pinMode(del, INPUT_PULLUP);
  pinMode(match, INPUT_PULLUP);
  pinMode(sw1, INPUT_PULLUP);
  pinMode(sw2, INPUT_PULLUP);
  pinMode(sw3, INPUT_PULLUP);
  pinMode(resultsw, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);

  digitalWrite(buzzer, HIGH);
  delay(1000);
  digitalWrite(buzzer, LOW);

  if (digitalRead(resultsw) == 0) {
    lcd.clear();
    lcd.print("System Reseting....");
    delay(1000);

    for (int i = 0; i < records; i++) {
      EEPROM.write(i + 10, 0xff);
    }
    uint8_t p = -1;
    for (uint8_t id = 0; id < records; id++) {
      deleteFingerprint(id);
      for (int i = 0; i < records; i++) {
        if (EEPROM.read(i + 10) == id) {
          EEPROM.write(i + 10, 0xff);
          lcd.clear();
          lcd.print("deleted: ");
          lcd.print(i);
          delay(100);
          break;
        }
      }
    }

    // Print initial values of all variables
    for (int i = 0; i < records; i++) {
      var[i] = 0;
      EEPROM.put(i * sizeof(int), var[i]);  // Store initial value in EEPROM
      Serial.print("var[");
      Serial.print(i);
      Serial.print("] = ");
      Serial.println(var[i]);
    }
    EEPROM.write(100, 0);
    EEPROM.write(101, 0);
    EEPROM.write(102, 0);
    lcd.clear();
    lcd.clear();
    lcd.print("System Reset");
    delay(1000);
  }

  if (digitalRead(del) == 0) {
    // Print initial values of all variables
    for (int i = 0; i < records; i++) {
      var[i] = 0;
      EEPROM.put(i * sizeof(int), var[i]);  // Store initial value in EEPROM
      Serial.print("var[");
      Serial.print(i);
      Serial.print("] = ");
      Serial.println(var[i]);
    }
    EEPROM.write(100, 0);
    EEPROM.write(101, 0);
    EEPROM.write(102, 0);
    lcd.clear();
  }

  if (EEPROM.read(100) == 0xff)
    EEPROM.write(100, 0);
  if (EEPROM.read(101) == 0xff)
    EEPROM.write(101, 0);
  if (EEPROM.read(1) == 0xff)
    EEPROM.write(102, 0);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Cn1");
  lcd.setCursor(6, 0);
  lcd.print("Cn2");
  lcd.setCursor(12, 0);
  lcd.print("Cn3");
  lcd.setCursor(0, 1);
  vote1 = EEPROM.read(100);
  lcd.print(vote1);
  lcd.setCursor(6, 1);
  vote2 = EEPROM.read(101);
  lcd.print(vote2);
  lcd.setCursor(12, 1);
  vote3 = EEPROM.read(102);
  lcd.print(vote3);
  delay(2000);
  lcd.clear();
}

void loop() {

  if ((digitalRead(resultsw) == 0) && (digitalRead(match) == 0)) {
    vote = vote1 + vote2 + vote3;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("total votes :");
    lcd.setCursor(0, 1);
    lcd.print(vote);
    delay(2000);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Press Match Key ");
  lcd.setCursor(0, 1);
  lcd.print("to start system");

  if (digitalRead(match) == 0) {
    digitalWrite(buzzer, HIGH);
    delay(200);
    digitalWrite(buzzer, LOW);
    // for (i = 0; i < 1; i++) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Place Finger");
    delay(2000);
    int result = getFingerprintIDez();
    Serial.println(result);
    // Serial.println(FP_value);
    readVariable(result);
    if ((result >= 0) && (FP_value == 0)) {
      Vote();
      updateVariable(result);
      return;
    }

    if ((FP_value == 1) && (result >= 0)) {
      Serial.println("Voter has already voted.");
      lcd.clear();
      lcd.print("Already Voted");
      digitalWrite(buzzer, HIGH);
      delay(5000);
      digitalWrite(buzzer, LOW);
    }
    lcd.clear();
  }
  checkKeys();
  delay(1000);
}

// Function to update the value in EEPROM
void updateVariable(int index) {
  if (index >= 0 && index < records) {
    // Set the selected variable to 1
    var[index] = 1;

    // Store the updated value in EEPROM
    EEPROM.put(index * sizeof(int), var[index]);

    // Print confirmation
    Serial.print("var[");
    Serial.print(index);
    Serial.println("] is set to 1 and stored in EEPROM.");
  } else {
    Serial.println("Please enter a valid number between 0 and 24.");
  }
}

// Function to read the value from EEPROM
void readVariable(int index) {
  if (index >= 0 && index < records) {
    int value;
    EEPROM.get(index * sizeof(int), value);  // Read the value from EEPROM
    Serial.print("EEPROM value at var[");
    Serial.print(index);
    Serial.print("] = ");
    Serial.println(value);
    FP_value = value;
  } else {
    Serial.println("Please enter a valid number between 0 and 24.");
  }
}

void checkKeys() {
  if (digitalRead(enroll) == 0) {
    lcd.clear();
    lcd.print("Please Wait");
    delay(1000);
    while (digitalRead(enroll) == 0)
      ;
    Enroll();
  } else if (digitalRead(del) == 0) {
    lcd.clear();
    lcd.print("Please Wait");
    delay(1000);
    delet();
  }
}

void Enroll() {
  int count = 0;
  lcd.clear();
  lcd.print("Enter Finger ID:");
  while (1) {
    lcd.setCursor(0, 1);
    lcd.print(count);
    if (digitalRead(up) == 0) {
      count++;
      if (count > 25)
        count = 0;
      delay(500);
    }

    else if (digitalRead(down) == 0) {
      count--;
      if (count < 0)
        count = 25;
      delay(500);
    } else if (digitalRead(del) == 0) {
      id = count;
      getFingerprintEnroll();
      for (int i = 0; i < records; i++) {
        if (EEPROM.read(i + 10) == 0xff) {
          EEPROM.write(i + 10, id);
          break;
        }
      }
      return;
    }

    else if (digitalRead(enroll) == 0) {
      delay(2000);
      return;
    }
  }
}

void delet() {
  int count = 0;
  lcd.clear();
  lcd.print("Enter Finger ID");

  while (1) {
    lcd.setCursor(0, 1);
    lcd.print(count);
    if (digitalRead(up) == 0) {
      count++;
      if (count > 25)
        count = 0;
      delay(500);
    }

    else if (digitalRead(down) == 0) {
      count--;
      if (count < 0)
        count = 25;
      delay(500);
    } else if (digitalRead(del) == 0) {
      id = count;
      deleteFingerprint(id);
      for (int i = 0; i < records; i++) {
        if (EEPROM.read(i + 10) == id) {
          EEPROM.write(i + 10, 0xff);
          break;
        }
      }
      return;
    } else if (digitalRead(enroll) == 0) {
      return;
    }
  }
}

uint8_t getFingerprintEnroll() {
  int p = -1;
  lcd.clear();
  lcd.print("finger ID:");
  lcd.print(id);
  lcd.setCursor(0, 1);
  lcd.print("Place Finger");
  delay(2000);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        //Serial.println("Image taken");
        lcd.clear();
        lcd.print("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        //Serial.println("No Finger");
        lcd.clear();
        lcd.print("No Finger");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        //Serial.println("Communication error");
        lcd.clear();
        lcd.print("Comm Error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        //Serial.println("Imaging error");
        lcd.clear();
        lcd.print("Imaging Error");
        break;
      default:
        //Serial.println("Unknown error");
        lcd.clear();
        lcd.print("Unknown Error");
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      //Serial.println("Image converted");
      lcd.clear();
      lcd.print("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      //Serial.println("Image too messy");
      lcd.clear();
      lcd.print("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      //Serial.println("Communication error");
      lcd.clear();
      lcd.print("Comm Error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      //Serial.println("Could not find fingerprint features");
      lcd.clear();
      lcd.print("Feature Not Found");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      //Serial.println("Could not find fingerprint features");
      lcd.clear();
      lcd.print("Feature Not Found");
      return p;
    default:
      //Serial.println("Unknown error");
      lcd.clear();
      lcd.print("Unknown Error");
      return p;
  }

  //Serial.println("Remove finger");
  lcd.clear();
  lcd.print("Remove Finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  //Serial.print("ID "); //Serial.println(id);
  p = -1;
  //Serial.println("Place same finger again");
  lcd.clear();
  lcd.print("Place Finger");
  lcd.setCursor(0, 1);
  lcd.print("   Again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        //Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        //Serial.print(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        //Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        //Serial.println("Imaging error");
        break;
      default:
        //Serial.println("Unknown error");
        return;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      //Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      //Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      //Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      //Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      //Serial.println("Could not find fingerprint features");
      return p;
    default:
      //Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  //Serial.print("Creating model for #");  //Serial.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    //Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    //Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    //Serial.println("Fingerprints did not match");
    return p;
  } else {
    //Serial.println("Unknown error");
    return p;
  }

  //Serial.print("ID "); //Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    //Serial.println("Stored!");
    lcd.clear();
    lcd.print("Stored!");
    delay(2000);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    //Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    //Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    //Serial.println("Error writing to flash");
    return p;
  } else {
    //Serial.println("Unknown error");
    return p;
  }
}

int getFingerprintIDez() {
  uint8_t p = finger.getImage();

  if (p != FINGERPRINT_OK)
    return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)
    return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) {
    lcd.clear();
    lcd.print("Finger Not Found");
    lcd.setCursor(0, 1);
    lcd.print("Try Later");
    delay(2000);
    return -1;
  }
  // found a match!
  //Serial.print("Found ID #");
  //Serial.print(finger.fingerID);
  lcd.setCursor(0, 1);
  lcd.print("Finger ID : ");
  lcd.setCursor(12, 1);
  lcd.print(finger.fingerID);
  delay(2000);
  return finger.fingerID;
}

uint8_t deleteFingerprint(uint8_t id) {
  uint8_t p = -1;
  lcd.clear();
  lcd.print("Please wait");
  p = finger.deleteModel(id);
  if (p == FINGERPRINT_OK) {
    // Serial.println("Deleted!");
    lcd.clear();
    lcd.print("Finger ");
    lcd.print(id);
    lcd.print(" Deleted");
    lcd.setCursor(0, 1);
    lcd.print("Successfully");
    delay(1000);
  }

  else {
    //Serial.print("Something Wrong");
    lcd.clear();
    lcd.print("Something Wrong");
    lcd.setCursor(0, 1);
    lcd.print("Try Again Later");
    delay(2000);
    return p;
  }
}

void Vote() {
  lcd.clear();
  lcd.print("Please Place");
  lcd.setCursor(0, 1);
  lcd.print("Your Vote");
  digitalWrite(buzzer, HIGH);
  delay(500);
  digitalWrite(buzzer, LOW);
  delay(1000);
  while (1) {
    if (digitalRead(sw1) == 0) {
      vote1++;
      voteSubmit(1);
      EEPROM.write(100, vote1);
      while (digitalRead(sw1) == 0)
        ;
      return;
    }
    if (digitalRead(sw2) == 0) {
      vote2++;
      voteSubmit(2);
      EEPROM.write(101, vote2);
      while (digitalRead(sw2) == 0)
        ;
      return;
    }
    if (digitalRead(sw3) == 0) {
      vote3++;
      voteSubmit(3);
      EEPROM.write(102, vote3);
      while (digitalRead(sw3) == 0)
        ;
      return;
    }

    if (digitalRead(resultsw) == 0) {
      int w=0;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Can1");
      lcd.setCursor(6, 0);
      lcd.print("Can2");
      lcd.setCursor(12, 0);
      lcd.print("Can3");
      for (int i = 0; i < 3; i++) {
        lcd.setCursor(i * 6, 1);
        lcd.print(EEPROM.read(i));
      }
      delay(2000);
      vote = vote1 + vote2 + vote3;
      if (vote) {
        if ((vote1 > vote2 && vote1 > vote3)) {
          w=1;
          lcd.clear();
          lcd.print("Can1 Wins");
          diaply_votes();

        } else if (vote2 > vote1 && vote2 > vote3) {
          w=2;
          lcd.clear();
          lcd.print("Can2 Wins");
          diaply_votes();

        } else if ((vote3 > vote1 && vote3 > vote2)) {
          w=3;
          lcd.clear();
          lcd.print("Can3 Wins");
          diaply_votes();

        } else {
          w=-1;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("   Tie Up Or   ");
          diaply_votes();
        }
        String uno = String("$") + String(vote1) + String("b") + String(vote2) + String("c") + String(vote3) + String("d") + String(w)+ String("e");
  Serial.println(uno);
      } else {
        lcd.clear();
        lcd.print("No Voting....");
        delay(1000);
        lcd.clear();
      }
      vote1 = 0;
      vote2 = 0;
      vote3 = 0;
      vote = 0;
      lcd.clear();
      return;
    }
  }
}

void diaply_votes() {
  lcd.setCursor(0, 2);
  lcd.print("Cn1");
  lcd.setCursor(6, 2);
  lcd.print("Cn2");
  lcd.setCursor(12, 2);
  lcd.print("Cn3");
  lcd.setCursor(0, 3);
  vote1 = EEPROM.read(100);
  lcd.print(vote1);
  lcd.setCursor(6, 3);
  vote2 = EEPROM.read(101);
  lcd.print(vote2);
  lcd.setCursor(12, 3);
  vote3 = EEPROM.read(102);
  lcd.print(vote3);
  delay(2000);
  lcd.clear();
}

void voteSubmit(int cn) {
  lcd.clear();
  if (cn == 1) {
    lcd.print("Can1");
  } else if (cn == 2) {
    lcd.print("Can2");
  } else if (cn == 3) {
    lcd.print("Can3");
  }
  lcd.setCursor(0, 1);
  lcd.print("Vote Submitted");
  digitalWrite(buzzer, HIGH);
  delay(1000);
  digitalWrite(buzzer, LOW);
  return;
}
