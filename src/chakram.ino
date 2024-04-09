#include <Adafruit_LSM6DSO32.h>
#include <Wire.h>
#include <Adafruit_DotStar.h>
// Because conditional #includes don't work w/Arduino sketches...
#include <SPI.h>         // COMMENT OUT THIS LINE FOR GEMMA OR TRINKET


/* Config */

#define ENABLE_SERIAL 0
#define ENABLE_VOLTAGE 0


#define LED_COUNT 113 // Total number of Dotstars on the strip
//#define NUM_LIT_SEGMENTS 5  //The number of "sections" that should be lit (ie, if 4, there will be 4 evenly spaced lit segments around the circle
#define NUM_LIT_SEGMENTS 6  //The number of "sections" that should be lit (ie, if 4, there will be 4 evenly spaced lit segments around the circle
#define PIXELS_PER_SEGMENT 6
//#define SEGMENT_GAP_LENGTH 15 //Deprecated


//#define MAXHUE 54612 //Max value for Hue is HSV format (abs max is 65535)
#define MINHUE 35000 //BLue and Purple
#define MAXHUE 45000 //BLue and Purple

/*#define MINHUE 1
#define MAXHUE 65535 //Max value for Hue is HSV format (abs max is 65535)*/

#define COLOR_INCREMENT ((MAXHUE - MINHUE) / (NUM_LIT_SEGMENTS * PIXELS_PER_SEGMENT)) //When doing a rainbow effect, the amount to increase the color by each step

#define RADS_PER_PIXEL (6.28318 / LED_COUNT) //2pi radians in a circle (6.28). Each LED takes up 6.28 / LED_COUNT radians of space
#define PIXELS_PER_RADIAN (1 / RADS_PER_PIXEL) // Convert from space each LED takes up to the number of LEDS in one radian
#define DEGREES_PER_PIXEL (RADS_PER_PIXEL * 57.29577) // Convert from radians to degrees because it are make math easier later

//#define TAP_THRESHOLD 30 //The threshold for the hit detection, measured in m/s
#define TAP_THRESHOLD 40 //The threshold for the hit detection, measured in m/s

/* Commented out pending the #defines for macros works
 * // For full rainbow:
int maxHue = 65535;

//568 assumes 115 pixels (65535 / 115 = 568)
//int increment = 568;
int increment = maxHue / LED_COUNT;

// 2pi radians in a circle
float radsPerPixel = 6.28318 / LED_COUNT;
float pixelsPerRadian = 1 / radsPerPixel;

//rads * 180/pi = degrees
float degreesPerPixel = radsPerPixel * 57.29577;*/

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#ifdef ESP32
// Cannot use 6 as output for ESP. Pins 6-11 are connected to SPI flash. Use 16 instead.
#define LED_PIN    15
#else
#define LED_PIN    15
#endif

//#define BUTTON_PIN A0
#define BUTTON_PIN 26



// Declare our DotStar strip object:
Adafruit_DotStar strip(LED_COUNT, DOTSTAR_BRG);

#define SECONDARY_I2C_PORT &Wire1

// For SPI mode, we need a CS pin
#define LSM_CS 10
// For software-SPI mode we need SCK/MOSI/MISO pins
#define LSM_SCK 13
#define LSM_MISO 12
#define LSM_MOSI 11

// Defie accelerometer object
Adafruit_LSM6DSO32 dso32;

//Array to hold the "current" colo values in the pixels
int ledValues[LED_COUNT];

void setup(void) {
    if (ENABLE_SERIAL) {
        Serial.begin(115200);
    }
    /*while (!Serial) {
        delay(100); // will pause Zero, Leonardo, etc until serial console opens
    }*/

    if (ENABLE_SERIAL) {
        Serial.println("Adafruit LSM6DSO32 test!");
    }

    if (!dso32.begin_I2C(0x6A, SECONDARY_I2C_PORT)) {
        // if (!dso32.begin_SPI(LSM_CS)) {
        // if (!dso32.begin_SPI(LSM_CS, LSM_SCK, LSM_MISO, LSM_MOSI)) {
        // Serial.println("Failed to find LSM6DSO32 chip");
        while (1) {
            delay(10);
        }
    }

    if (ENABLE_SERIAL) {
        Serial.println("LSM6DSO32 Found!");
    }

    pinMode(BUTTON_PIN, INPUT);

    dso32.setAccelRange(LSM6DSO32_ACCEL_RANGE_8_G);
    /*Serial.print("Accelerometer range set to: ");*/
/*    switch (dso32.getAccelRange()) {
        case LSM6DSO32_ACCEL_RANGE_4_G:
            Serial.println("+-4G");
            break;
        case LSM6DSO32_ACCEL_RANGE_8_G:
            Serial.println("+-8G");
            break;
        case LSM6DSO32_ACCEL_RANGE_16_G:
            Serial.println("+-16G");
            break;
        case LSM6DSO32_ACCEL_RANGE_32_G:
            Serial.println("+-32G");
            break;
    }*/

    dso32.setGyroRange(LSM6DS_GYRO_RANGE_2000_DPS );
/*    Serial.print("Gyro range set to: ");
    switch (dso32.getGyroRange()) {
        case LSM6DS_GYRO_RANGE_125_DPS:
            Serial.println("125 degrees/s");
            break;
        case LSM6DS_GYRO_RANGE_250_DPS:
            Serial.println("250 degrees/s");
            break;
        case LSM6DS_GYRO_RANGE_500_DPS:
            Serial.println("500 degrees/s");
            break;
        case LSM6DS_GYRO_RANGE_1000_DPS:
            Serial.println("1000 degrees/s");
            break;
        case LSM6DS_GYRO_RANGE_2000_DPS:
            Serial.println("2000 degrees/s");
            break;
        case ISM330DHCX_GYRO_RANGE_4000_DPS:
            break; // unsupported range for the DSO32
    }*/

/*    // dso32.setAccelDataRate(LSM6DS_RATE_12_5_HZ);
    Serial.print("Accelerometer data rate set to: ");
    switch (dso32.getAccelDataRate()) {
        case LSM6DS_RATE_SHUTDOWN:
            Serial.println("0 Hz");
            break;
        case LSM6DS_RATE_12_5_HZ:
            Serial.println("12.5 Hz");
            break;
        case LSM6DS_RATE_26_HZ:
            Serial.println("26 Hz");
            break;
        case LSM6DS_RATE_52_HZ:
            Serial.println("52 Hz");
            break;
        case LSM6DS_RATE_104_HZ:
            Serial.println("104 Hz");
            break;
        case LSM6DS_RATE_208_HZ:
            Serial.println("208 Hz");
            break;
        case LSM6DS_RATE_416_HZ:
            Serial.println("416 Hz");
            break;
        case LSM6DS_RATE_833_HZ:
            Serial.println("833 Hz");
            break;
        case LSM6DS_RATE_1_66K_HZ:
            Serial.println("1.66 KHz");
            break;
        case LSM6DS_RATE_3_33K_HZ:
            Serial.println("3.33 KHz");
            break;
        case LSM6DS_RATE_6_66K_HZ:
            Serial.println("6.66 KHz");
            break;
    }*/

    //Set to 12.5Hz in Arudion IDE, changed to 833Hz in platformio
    //dso32.setGyroDataRate(LSM6DS_RATE_833_HZ);
    dso32.setGyroDataRate(LSM6DS_RATE_3_33K_HZ);
    //dso32.setGyroDataRate(LSM6DS_RATE_12_5_HZ);
    /*Serial.print("Gyro data rate set to: ");*/
/*    switch (dso32.getGyroDataRate()) {
        case LSM6DS_RATE_SHUTDOWN:
            Serial.println("0 Hz");
            break;
        case LSM6DS_RATE_12_5_HZ:
            Serial.println("12.5 Hz");
            break;
        case LSM6DS_RATE_26_HZ:
            Serial.println("26 Hz");
            break;
        case LSM6DS_RATE_52_HZ:
            Serial.println("52 Hz");
            break;
        case LSM6DS_RATE_104_HZ:
            Serial.println("104 Hz");
            break;
        case LSM6DS_RATE_208_HZ:
            Serial.println("208 Hz");
            break;
        case LSM6DS_RATE_416_HZ:
            Serial.println("416 Hz");
            break;
        case LSM6DS_RATE_833_HZ:
            Serial.println("833 Hz");
            break;
        case LSM6DS_RATE_1_66K_HZ:
            Serial.println("1.66 KHz");
            break;
        case LSM6DS_RATE_3_33K_HZ:
            Serial.println("3.33 KHz");
            break;
        case LSM6DS_RATE_6_66K_HZ:
            Serial.println("6.66 KHz");
            break;
    }*/

    strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
    strip.show();            // Turn OFF all pixels ASAP
    strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)

    for (int i = 0; i < LED_COUNT; i++) {
        //ledValues[i] = i * 569; // This was based on a hardcoded interval? JD
        ledValues[i] = i * COLOR_INCREMENT;
    }

    static uint8_t ucParameterToPass;
    xTaskCreate( vTaskPollGyro, "vTaskPollGyro", 10000, &ucParameterToPass, tskIDLE_PRIORITY, NULL );
}

int hue = 1;
float theta;
int ledIndex;
long lastSpinningMillis = 0;

float pixelsMoved;
long elapsedTime;
float spinningIndex = 0;

float degSec;

float brightness = 0;


long lastFadeUpdateTime = 0;



float lastStationaryIndex = 0;

int currHue = 0;
int modifier = 1;

long hitDisplayEndMillis = 0;

float voltageReading = 0;
long currMillis = 0;

sensors_event_t accel;
sensors_event_t gyro;
sensors_event_t temp;
void vTaskPollGyro (void * pvParameters) {
    while (true) {
        dso32.getEvent(&accel, &gyro, &temp);

        currMillis = millis();
        elapsedTime = currMillis - lastSpinningMillis; //Time since last loop

        //JMD: "-1 *" is if the accelerometer is upside down
        /*
         * Given the amount of time that as passed (elapsedTime) and the rate of rotation, determine by how many pixels we need to shift to appear stationary
         */
        pixelsMoved = (elapsedTime * gyro.gyro.z / 1000) * PIXELS_PER_RADIAN;

        spinningIndex = spinningIndex + pixelsMoved;
        spinningIndex = spinningIndex - ((int) (spinningIndex / LED_COUNT) * LED_COUNT); //Rollover in case we passed the end of the strip

        lastSpinningMillis = currMillis;
        //vTaskDelay(2);
    }
}


/*void example_deep_sleep_register_ext0_wakeup(void) {
    //const int ext_wakeup_pin_0 = 25;
    const int ext_wakeup_pin_0 = GPIO_NUM_26;

    //printf("Enabling EXT0 wakeup on pin GPIO%d\n", ext_wakeup_pin_0);
    //ESP_ERROR_CHECK(esp_sleep_enable_ext0_wakeup(ext_wakeup_pin_0, 1));
    ESP_ERROR_CHECK(esp_sleep_enable_ext0_wakeup((gpio_num_t ) ext_wakeup_pin_0, 1));

    // Configure pullup/downs via RTCIO to tie wakeup pins to inactive level during deepsleep.
    // EXT0 resides in the same power domain (RTC_PERIPH) as the RTC IO pullup/downs.
    // No need to keep that power domain explicitly, unlike EXT1.
    ESP_ERROR_CHECK(rtc_gpio_pullup_dis(ext_wakeup_pin_0));
    ESP_ERROR_CHECK(rtc_gpio_pulldown_en(ext_wakeup_pin_0));
}*/

bool isOff = false;
// This modifier is used to reduce the number of PIXELS_PER_SEGMENT as the rotation rate increases
int pixelsPerSegmentModifier = 0;
int extraGap = 0;
int currHueModifier = 0;

void loop() {
    strip.clear();

    if (digitalRead(BUTTON_PIN) == LOW) {
        if (!isOff) {
            for (int i = 0; i < LED_COUNT; i++) {
                //strip.setPixelColor(i, 128, 128, 128);
                strip.setPixelColor(i, 0, 0, 0);
            }
            strip.show();
            isOff = true;
        }
        return;
    }

    isOff = false;

    brightness = 255;

    currHueModifier = (int) ((65534 * (1 - abs(gyro.gyro.z) / 40)) + 65534 + 40000) % 65534;

    if (abs(gyro.gyro.z) > 40) {
        pixelsPerSegmentModifier = 2;
    } else if (abs(gyro.gyro.z) > 20) {
        pixelsPerSegmentModifier = 1;
    } else if (abs(gyro.gyro.z) > 10) {
        pixelsPerSegmentModifier = 0;
    } else {
        pixelsPerSegmentModifier = 0;
    }

    extraGap = (LED_COUNT - (NUM_LIT_SEGMENTS * PIXELS_PER_SEGMENT)) / NUM_LIT_SEGMENTS % NUM_LIT_SEGMENTS;

    for (int i = 1; i <= NUM_LIT_SEGMENTS; i++ ) {
        for (int z = 0; z < PIXELS_PER_SEGMENT; z++) {
            // Make adjacent segments opposite colors on the color wheel.
            if (z >= pixelsPerSegmentModifier && z < PIXELS_PER_SEGMENT - pixelsPerSegmentModifier) {
                if (i % 2 == 0) {
                    currHue = currHueModifier;
                } else {
                    currHue = (65534 - currHueModifier + 65534) % 65534;
                }
                strip.setPixelColor( lround(spinningIndex + (LED_COUNT / NUM_LIT_SEGMENTS * i) + LED_COUNT + z + extraGap) % LED_COUNT, strip.ColorHSV(currHue, 255, round(brightness)));
            }
            // Uncomment this for a progressive/rainbow effect
            /*currHue += COLOR_INCREMENT * modifier;
            if (currHue > MAXHUE) {
                currHue = MAXHUE;
                modifier = -1;
            } else if (currHue < MINHUE) {
                currHue = MINHUE;
                modifier = 1;
            }*/
        }
    }

    strip.show();
}

/*
 * 360d : 1s :: 7.826pixels : 1 s
 * 7.28rad
 * 1 pix = 7.826d
 * 1 pix = 0.158 rad
 *
 * 14 rad / x pix = .158 rad / 1 pix
 *     14/.158 = x pix
 *
 *     pixMil = gyro.gyro.z / .158 / 1000 =pixels/millisecond
 *     elapsedTime = millis() - lastSpinningMillis;
 */

/*if (ENABLE_VOLTAGE) {
        voltageReading = analogRead(A2);
        voltageReading = voltageReading / 1024 * 2;
    }
    *//*if (ENABLE_SERIAL) {
        *//**//*voltageReading = voltageReading * BATTERY_VOLTAGE_MAX / .939;*//**//*
        Serial.println(voltageReading);
        *//**//*if (voltageReading < 4.3) {
            for (int x = 0; x < LED_COUNT; x++) {
                strip.setPixelColor(x, strip.ColorHSV(1, 255, 255));
            }
            strip.show();
        }*//**//*
        Serial.print("gyro z: ");Serial.print(gyro.gyro.z);Serial.print("  pixelsMoved: ");Serial.println(pixelsMoved);
    }*//*

    *//*
     * HSV
     * 0,100,100 = #ff0000 red = 0
     * 30,100,100 = #ff80000 orange = 5LED_COUNT1
     * 60,100,100 = #ffff00 yellow = 10,927
     * 11,000 / LED_COUNT = 239.13 // too yellow/green
     * 5LED_COUNT1/LED_COUNT = 118
     */