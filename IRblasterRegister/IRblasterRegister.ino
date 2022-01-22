// Allow over air update
// #define OTA_ENABLE true
// #include "BaseOTA.h"

#include <Arduino.h>
#include <assert.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRac.h>
#include <IRtext.h>
#include <IRutils.h>

const uint16_t kRecvPin = 4;
const uint32_t kBaudRate = 115200;
const uint16_t kCaptureBufferSize = 1024;
#if DECODE_AC
const uint8_t kTimeout = 50;
#else	 // DECODE_AC
const uint8_t kTimeout = 15;
#endif // DECODE_AC
const uint16_t kMinUnknownSize = 12;
const uint8_t kTolerancePercentage = kTolerance; // kTolerance is normally 25%
#define LEGACY_TIMING_INFO false
IRrecv irrecv(kRecvPin, kCaptureBufferSize, kTimeout, true);
decode_results results; // Somewhere to store the results

// const int LED = 14;
void setup()
{
	// pinMode(LED, OUTPUT);
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, HIGH);
	#if defined(ESP8266)
	Serial.begin(kBaudRate, SERIAL_8N1, SERIAL_TX_ONLY);
	#else							// ESP8266
	Serial.begin(kBaudRate, SERIAL_8N1);
	#endif						// ESP8266
	while (!Serial) // Wait for the serial connection to be establised.
		delay(50);
	assert(irutils::lowLevelSanityCheck() == 0);

	Serial.printf("\n" D_STR_IRRECVDUMP_STARTUP "\n", kRecvPin);
	#if DECODE_HASH
	irrecv.setUnknownThreshold(kMinUnknownSize);
	#endif																			 // DECODE_HASH
	irrecv.setTolerance(kTolerancePercentage); // Override the default tolerance.
	irrecv.enableIRIn();
	delay(4000);
  Serial.println("started");
}

void loop()
{
	if (irrecv.decode(&results))
	{
		// digitalWrite(LED, HIGH);
		// delay(100);
		// digitalWrite(LED, LOW);
		// delay(100);
		digitalWrite(LED_BUILTIN, LOW);
		delay(100);
		digitalWrite(LED_BUILTIN, HIGH);
		uint32_t now = millis();
		Serial.printf(D_STR_TIMESTAMP " : %06u.%03u\n", now / 1000, now % 1000);
		if (results.overflow)
			Serial.printf(D_WARN_BUFFERFULL "\n", kCaptureBufferSize);
		Serial.println(D_STR_LIBRARY "   : v" _IRREMOTEESP8266_VERSION_STR "\n");
		if (kTolerancePercentage != kTolerance)
			Serial.printf(D_STR_TOLERANCE " : %d%%\n", kTolerancePercentage);
		Serial.print(resultToHumanReadableBasic(&results));
		String description = IRAcUtils::resultAcToString(&results);
		if (description.length())
			Serial.println(D_STR_MESGDESC ": " + description);
		yield(); // Feed the WDT as the text output can take a while to print.
		#if LEGACY_TIMING_INFO
				// Output legacy RAW timing info of the result.
		Serial.println(resultToTimingInfo(&results));
		yield(); // Feed the WDT (again)
		#endif			 // LEGACY_TIMING_INFO
		Serial.println(resultToSourceCode(&results));
		Serial.println(); // Blank line between entries

		yield(); // Feed the WDT (again)
	}

	// OTAloopHandler();
}
