#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <Arduino.h>
#include <assert.h>
// #include <IRrecv.h>
#include <IRsend.h>
#include <IRremoteESP8266.h>
#include <IRac.h>
#include <IRtext.h>
#include <IRutils.h>
#include <uri/UriBraces.h>

#ifndef STASSID
#define STASSID "MY_SSID"
#define STAPSK "MY_PASS"
#endif

const char *ssid = STASSID;
const char *password = STAPSK;

ESP8266WebServer server(80);

const uint16_t kRecvPin = 4;
const uint32_t kBaudRate = 115200;
const uint16_t kCaptureBufferSize = 1024;
#if DECODE_AC
const uint8_t kTimeout = 50;
#else  // DECODE_AC
const uint8_t kTimeout = 15;
#endif // DECODE_AC
const uint16_t kMinUnknownSize = 12;
const uint8_t kTolerancePercentage = kTolerance; // kTolerance is normally 25%
#define LEGACY_TIMING_INFO false
IRrecv irrecv(kRecvPin, kCaptureBufferSize, kTimeout, true);
decode_results results; // Somewhere to store the results

//IR sender: D1 == GPIO5
const uint16_t IR_LED = 5;
IRsend irsend(IR_LED);
// LG buttons:
uint16_t tvLgPwrBtn[71] = {9086, 4418, 644, 488, 680, 452, 656, 1586, 648, 486, 644, 490, 642, 490, 650, 484, 648, 484, 648, 1594, 640, 1600, 646, 488, 680, 1560, 650, 1590, 684, 1558, 686, 1556, 648, 1592, 650, 484, 650, 484, 650, 484, 672, 1568, 652, 512, 616, 516, 592, 542, 620, 512, 626, 1584, 680, 1560, 684, 1556, 684, 480, 652, 1558, 678, 1562, 652, 1590, 648, 1592, 646, 39942, 9090, 2172, 644}; // NEC 20DF10EF
// Soundbar buttons:
uint16_t creativePwrBtn[71] = {9054, 4460,  640, 488,  612, 518,  610, 518,  612, 518,  610, 518,  610, 1650,  610, 518,  612, 518,  612, 1648,  610, 1650,  610, 1648,  610, 1650,  610, 1648,  612, 518,  612, 1648,  612, 1648,  612, 1648,  612, 518,  610, 520,  610, 1648,  612, 518,  610, 518,  612, 518,  612, 518,  612, 518,  612, 1648,  612, 1648,  610, 520,  610, 1648,  610, 1652,  608, 1648,  612, 1648,  612, 39654,  9054, 2228,  614};  // NEC 4FB906F
uint16_t creativeInputBtn[71] = {9058, 4458,  640, 490,  612, 518,  612, 518,  612, 518,  612, 518,  612, 1648,  612, 518,  612, 518,  612, 1648,  612, 1648,  636, 1624,  612, 1648,  612, 1648,  612, 518,  638, 1624,  612, 1648,  612, 1648,  612, 516,  614, 1648,  612, 1648,  612, 518,  612, 516,  612, 518,  614, 516,  614, 516,  614, 1646,  612, 518,  612, 518,  612, 1646,  612, 1650,  610, 1646,  612, 1648,  614, 39656,  9096, 2188,  618};
// dekoder buttons: 
uint16_t backBtn[77] = {4522, 4506,  532, 446,  550, 446,  552, 446,  552, 448,  550, 1480,  530, 1482,  530, 448,  550, 468,  530, 1482,  528, 470,  528, 1482,  530, 1482,  528, 468,  530, 470,  528, 468,  528, 470,  528, 4508,  528, 462,  526, 462,  528, 462,  528, 462,  526, 1486,  526, 470,  526, 1486,  526, 1486,  526, 1486,  526, 472,  526, 1486,  524, 472,  526, 472,  524, 1488,  524, 474,  524, 474,  522, 476,  522, 1490,  522, 476,  520, 1492,  524};  // SAMSUNG36 CB00BA45
uint16_t dekPwr[77] = {4546, 4482,  556, 440,  556, 442,  556, 442,  556, 442,  554, 1456,  558, 1454,  556, 442,  556, 442,  556, 1456,  558, 440,  556, 1456,  556, 1456,  556, 442,  558, 442,  556, 442,  558, 440,  556, 4482,  580, 408,  556, 434,  556, 434,  556, 434,  556, 442,  556, 1456,  556, 1456,  556, 1454,  556, 1456,  556, 442,  556, 440,  558, 442,  556, 1456,  556, 442,  556, 442,  556, 442,  556, 442,  556, 1456,  556, 1456,  556, 1456,  556};  // SAMSUNG36 CB007887
// dekoder NUM buttons:
uint16_t num1[77] = {4524, 4482,  556, 440,  556, 442,  556, 442,  556, 442,  556, 1456,  556, 1456,  556, 442,  556, 442,  556, 1454,  558, 442,  556, 1456,  558, 1454,  558, 442,  554, 444,  556, 442,  556, 440,  558, 4480,  556, 434,  554, 436,  556, 434,  556, 434,  558, 442,  556, 442,  558, 440,  556, 442,  556, 442,  556, 442,  556, 442,  556, 442,  556, 1454,  556, 1456,  556, 1456,  556, 1456,  556, 1454,  558, 1454,  558, 1454,  558, 1454,  556};  // SAMSUNG36 CB0000FF
uint16_t num2[77] = {4576, 4454,  588, 408,  558, 440,  558, 440,  558, 440,  558, 1456,  556, 1454,  558, 440,  556, 442,  556, 1454,  558, 440,  556, 1454,  558, 1454,  558, 440,  556, 442,  556, 442,  556, 440,  558, 4480,  588, 402,  558, 432,  558, 432,  558, 432,  558, 1454,  558, 440,  558, 440,  558, 440,  558, 440,  558, 440,  558, 440,  558, 440,  558, 440,  556, 1456,  556, 1454,  558, 1454,  558, 1454,  558, 1454,  558, 1454,  558, 1454,  582};  // SAMSUNG36 CB00807F
uint16_t num3[77] = {4578, 4452,  586, 408,  558, 440,  558, 440,  558, 440,  558, 1454,  558, 1454,  558, 440,  558, 440,  558, 1454,  558, 440,  558, 1454,  558, 1454,  558, 440,  558, 440,  558, 440,  558, 440,  556, 4482,  586, 402,  558, 434,  556, 434,  558, 432,  558, 440,  558, 1454,  556, 440,  558, 440,  558, 440,  558, 440,  558, 440,  556, 440,  558, 1456,  556, 440,  558, 1454,  556, 1456,  556, 1456,  556, 1454,  558, 1454,  558, 1454,  556};  // SAMSUNG36 CB0040BF
uint16_t num4[77] = {4578, 4452,  588, 410,  556, 440,  558, 440,  558, 440,  558, 1454,  556, 1454,  558, 442,  556, 440,  556, 1456,  558, 440,  556, 1454,  558, 1454,  558, 440,  558, 440,  558, 440,  558, 440,  558, 4482,  588, 402,  558, 432,  558, 432,  558, 434,  558, 1454,  558, 1454,  582, 444,  558, 440,  556, 442,  532, 464,  536, 464,  564, 434,  570, 428,  596, 402,  596, 1388,  622, 1390,  622, 1390,  618, 1394,  590, 1422,  586, 1426,  584};  // SAMSUNG36 CB00C03F
uint16_t num5[77] = {4576, 4452,  586, 408,  558, 440,  558, 442,  556, 440,  558, 1454,  558, 1454,  558, 440,  558, 440,  558, 1454,  558, 440,  558, 1454,  558, 1454,  556, 440,  558, 440,  556, 440,  558, 440,  556, 4480,  586, 404,  558, 432,  558, 432,  558, 432,  558, 440,  556, 440,  558, 1454,  558, 442,  556, 440,  558, 440,  558, 440,  558, 440,  558, 1454,  558, 1454,  558, 442,  556, 1454,  558, 1454,  558, 1454,  558, 1454,  558, 1454,  558};  // SAMSUNG36 CB0020DF
uint16_t num6[77] = {4576, 4452,  586, 410,  558, 440,  558, 440,  558, 440,  558, 1454,  558, 1454,  558, 440,  558, 440,  558, 1454,  558, 440,  558, 1454,  558, 1454,  556, 442,  556, 442,  558, 440,  558, 440,  556, 4480,  586, 404,  558, 432,  558, 434,  556, 432,  558, 1454,  556, 440,  558, 1454,  558, 440,  558, 440,  558, 440,  558, 440,  558, 440,  558, 440,  558, 1454,  558, 440,  558, 1454,  556, 1454,  558, 1454,  558, 1454,  582, 1428,  562};  // SAMSUNG36 CB00A05F
uint16_t num7[77] = {4576, 4452,  586, 410,  558, 442,  556, 440,  558, 440,  558, 1454,  558, 1454,  558, 442,  556, 440,  558, 1454,  556, 442,  556, 1456,  556, 1454,  558, 440,  558, 440,  558, 440,  558, 440,  558, 4480,  586, 402,  556, 434,  556, 434,  558, 432,  556, 442,  558, 1454,  558, 1456,  556, 440,  558, 440,  558, 440,  558, 442,  556, 442,  556, 1454,  558, 440,  558, 440,  556, 1456,  556, 1454,  556, 1456,  556, 1454,  558, 1454,  558};  // SAMSUNG36 CB00609F
uint16_t num8[77] = {4574, 4452,  586, 410,  558, 440,  556, 442,  556, 440,  558, 1454,  558, 1454,  558, 440,  558, 440,  558, 1454,  558, 440,  558, 1454,  558, 1454,  556, 440,  558, 440,  558, 440,  558, 440,  556, 4482,  586, 402,  560, 432,  558, 432,  556, 434,  558, 1454,  556, 1456,  556, 1454,  558, 440,  558, 440,  558, 440,  556, 440,  558, 440,  558, 440,  556, 440,  558, 440,  558, 1454,  558, 1454,  558, 1454,  558, 1454,  558, 1454,  584};  // SAMSUNG36 CB00E01F
uint16_t num9[77] = {4576, 4452,  588, 408,  558, 440,  558, 440,  558, 440,  558, 1454,  558, 1456,  556, 440,  558, 440,  558, 1454,  558, 440,  556, 1454,  558, 1454,  558, 440,  558, 440,  558, 440,  556, 440,  558, 4482,  584, 404,  556, 434,  558, 432,  558, 434,  556, 440,  558, 440,  556, 442,  556, 1454,  558, 440,  558, 440,  558, 442,  556, 440,  556, 1454,  558, 1454,  558, 1454,  556, 442,  556, 1454,  558, 1454,  558, 1454,  556, 1456,  556};  // SAMSUNG36 CB0010EF
uint16_t num0[77] = {4576, 4452,  586, 408,  558, 440,  558, 440,  558, 440,  558, 1454,  558, 1454,  558, 440,  558, 440,  558, 1454,  556, 440,  558, 1454,  558, 1454,  558, 440,  556, 442,  558, 440,  556, 440,  558, 4482,  586, 402,  558, 432,  558, 434,  558, 432,  558, 1454,  556, 440,  558, 440,  556, 1456,  558, 440,  558, 440,  558, 440,  558, 442,  586, 440,  560, 1422,  562, 1452,  562, 464,  534, 1452,  564, 1448,  592, 1420,  622, 1390,  622};  // SAMSUNG36 CB00906F
uint16_t progUp[77] = {4546, 4482,  554, 442,  556, 442,  556, 442,  556, 442,  556, 1456,  556, 1456,  556, 442,  556, 442,  556, 1456,  556, 442,  556, 1456,  556, 1456,  556, 442,  556, 442,  556, 442,  556, 442,  556, 4482,  556, 434,  556, 434,  556, 434,  556, 434,  558, 442,  556, 440,  556, 442,  556, 442,  556, 442,  556, 1456,  558, 1454,  558, 440,  556, 1456,  556, 1456,  556, 1456,  556, 1456,  556, 1456,  554, 442,  556, 442,  556, 1456,  556};  // SAMSUNG36 CB0006F9
uint16_t progDown[77] = {4548, 4482,  556, 440,  556, 442,  554, 442,  556, 444,  554, 1456,  556, 1456,  556, 442,  556, 442,  556, 1456,  556, 442,  556, 1456,  556, 1456,  556, 442,  556, 442,  556, 444,  556, 442,  554, 4482,  556, 434,  556, 434,  556, 434,  554, 436,  556, 1456,  556, 442,  556, 442,  556, 442,  556, 442,  556, 1454,  556, 1456,  558, 440,  556, 442,  556, 1454,  556, 1456,  556, 1456,  556, 1454,  558, 440,  556, 442,  558, 1454,  556};  // SAMSUNG36 CB008679
uint16_t volDown[67] = {9056, 4462,  638, 490,  612, 518,  612, 518,  612, 518,  612, 518,  612, 1648,  612, 520,  610, 518,  612, 1650,  610, 1650,  610, 1650,  610, 1650,  610, 1650,  610, 520,  610, 1650,  612, 1650,  610, 1650,  610, 1650,  612, 1650,  610, 520,  610, 520,  610, 518,  612, 518,  610, 520,  610, 520,  610, 520,  610, 518,  612, 1650,  610, 1650,  610, 1650,  610, 1650,  612, 1650,  610};  // NEC 4FBE01F
uint16_t volUp[67] = {9054, 4462,  610, 520,  636, 492,  610, 520,  614, 516,  636, 494,  610, 1650,  608, 522,  610, 518,  612, 1650,  612, 1648,  610, 1650,  610, 1650,  610, 1650,  610, 520,  612, 1648,  610, 1650,  612, 518,  612, 1650,  610, 1650,  610, 520,  618, 512,  624, 506,  610, 518,  636, 494,  634, 1624,  610, 520,  636, 494,  612, 1648,  610, 1652,  608, 1652,  610, 1650,  610, 1652,  608};  // NEC 4FB609F

uint16_t tvInputBtn[67] = {9052, 4452,  682, 448,  612, 522,  612, 1628,  654, 480,  612, 520,  690, 442,  612, 520,  588, 546,  680, 1562,  646, 1596,  686, 446,  690, 1550,  648, 1594,  652, 1588,  644, 1596,  612, 1628,  614, 1628,  682, 1558,  640, 494,  688, 1552,  654, 478,  642, 492,  650, 482,  614, 518,  616, 518,  614, 520,  646, 1594,  612, 520,  612, 1628,  652, 1588,  654, 1588,  640, 1602,  654};  // NEC 20DFD02F
uint16_t tvEnterBtn[67] = {9092, 4412,  652, 480,  652, 480,  652, 1588,  652, 480,  652, 480,  652, 482,  652, 482,  650, 482,  652, 1588,  682, 1558,  652, 480,  652, 1588,  652, 1590,  652, 1590,  680, 1560,  652, 1590,  652, 482,  650, 482,  652, 1590,  652, 480,  654, 480,  652, 480,  652, 1590,  652, 482,  652, 1588,  652, 1590,  652, 480,  650, 1590,  654, 1588,  652, 1588,  654, 480,  652, 1588,  652};  // NEC 20DF22DD
uint16_t tvBackBtn[67] = {9084, 4418,  682, 450,  652, 482,  656, 1586,  650, 484,  648, 486,  652, 482,  680, 452,  650, 484,  648, 1592,  646, 1596,  648, 486,  650, 1590,  648, 1592,  682, 1558,  646, 1596,  646, 1594,  646, 488,  684, 450,  652, 480,  644, 1598,  656, 478,  648, 1594,  652, 480,  652, 480,  652, 1590,  648, 1590,  654, 1586,  654, 512,  620, 1590,  626, 538,  614, 1596,  650, 1590,  656};  // NEC 20DF14EB

void blinkLed() {
	digitalWrite(LED_BUILTIN, LOW);
	delay(20);
	digitalWrite(LED_BUILTIN, HIGH);
}

void handleNotFound() {
	blinkLed();
	server.send(404, "application/json", "{\"msg\":\"Not found\"}");
}

void creativePwr() {
	blinkLed();
	irsend.sendRaw(creativePwrBtn, 71, 38);
	server.send(200, "application/json", "{\"signal\":\"sent\"}");
}

void creativeInput() {
	blinkLed();
	irsend.sendRaw(creativeInputBtn, 71, 38);
	server.send(200, "application/json", "{\"signal\":\"sent\"}");
}

void dekoderPwr() {
	blinkLed();
	irsend.sendRaw(dekPwr, 77, 38);
	server.send(200, "application/json", "{\"signal\":\"sent\"}");
}

void dekoderBackBtn() {
	blinkLed();
	irsend.sendRaw(backBtn, 77, 38);
	server.send(200, "application/json", "{\"signal\":\"sent\"}");
}

void tvPwr() {
	blinkLed();
	irsend.sendRaw(tvLgPwrBtn, 71, 38);
	server.send(200, "application/json", "{\"signal\":\"sent\"}");
}

void tvInput() {
	blinkLed();
	irsend.sendRaw(tvInputBtn, 67, 38);
	server.send(200, "application/json", "{\"signal\":\"sent\"}");
}

void tvEnter() {
	blinkLed();
	irsend.sendRaw(tvEnterBtn, 67, 38);
	server.send(200, "application/json", "{\"signal\":\"sent\"}");
}

void tvBack() {
	blinkLed();
	irsend.sendRaw(tvBackBtn, 67, 38);
	server.send(200, "application/json", "{\"signal\":\"sent\"}");
}

void setup(void)
{
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, HIGH);
	Serial.begin(115200);
	irsend.begin();
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);
	Serial.println("");

	while (WiFi.status() != WL_CONNECTED) {
		delay(100);
		Serial.print(".");
	}

	Serial.println("");
	Serial.print("Connected to ");
	Serial.println(ssid);
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());

	if (MDNS.begin("esp8266")) {
		Serial.println("MDNS responder started");
	}

	server.on("/creative", creativePwr);
	server.on("/creative/input", creativeInput);
	server.on("/tv", tvPwr);
	server.on("/tv/input", tvInput);
	server.on("/tv/enter", tvEnter);
	server.on("/tv/back", tvBack);
	server.on("/dekoder", dekoderPwr);
	server.on("/dekoder-back", dekoderBackBtn);

	server.on(UriBraces("/tv/{}"), []() {
		String prog = server.pathArg(0);
		switch (prog.toInt()) {
		case 1:
			irsend.sendRaw(num1, 77, 38);
			break;
		case 2:
			irsend.sendRaw(num2, 77, 38);
			break;
		case 3:
			irsend.sendRaw(num3, 77, 38);
			break;
		case 4:
			irsend.sendRaw(num4, 77, 38);
			break;
		case 5:
			irsend.sendRaw(num5, 77, 38);
			break;
		case 6:
			irsend.sendRaw(num6, 77, 38);
			break;
		case 7:
			irsend.sendRaw(num7, 77, 38);
			break;
		case 8:
			irsend.sendRaw(num8, 77, 38);
			break;
		case 9:
			irsend.sendRaw(num9, 77, 38);
			break;
		case 0:
			irsend.sendRaw(num0, 77, 38);
			break;
		default:
			break;
		}
		server.send(200, "application/json", "{\"signal\":\"sent\"}");
	});
	server.on("/tv/prog/up", []() {
		irsend.sendRaw(progUp, 77, 38);
		server.send(200, "application/json", "{\"signal\":\"sent\"}");
	});
	server.on("/tv/prog/down", []() {
		irsend.sendRaw(progDown, 77, 38);
		server.send(200, "application/json", "{\"signal\":\"sent\"}");
	});
	server.on("/tv/volume/up", []() {
		irsend.sendRaw(volUp, 67, 38);
		server.send(200, "application/json", "{\"signal\":\"sent\"}");
	});
	server.on("/tv/volume/down", []() {
		irsend.sendRaw(volDown, 67, 38);
		server.send(200, "application/json", "{\"signal\":\"sent\"}");
	});


	server.onNotFound(handleNotFound);
	server.begin();
	blinkLed();

#if defined(ESP8266)
	Serial.begin(kBaudRate, SERIAL_8N1, SERIAL_TX_ONLY);
#else				// ESP8266
	Serial.begin(kBaudRate, SERIAL_8N1);
#endif				// ESP8266
	while (!Serial) // Wait for the serial connection to be establised.
		delay(50);
	assert(irutils::lowLevelSanityCheck() == 0);

	Serial.printf("\n" D_STR_IRRECVDUMP_STARTUP "\n", kRecvPin);
#if DECODE_HASH
	irrecv.setUnknownThreshold(kMinUnknownSize);
#endif										   // DECODE_HASH
	irrecv.setTolerance(kTolerancePercentage); // Override the default tolerance.
	irrecv.enableIRIn();

	Serial.println("HTTP server started");
}

void loop(void)
{
	server.handleClient();
	MDNS.update();

	if (irrecv.decode(&results)) {
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
}