// #include <TelegramCertificate.h>
#include <UniversalTelegramBot.h>
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include <WakeOnLan.h>
#include <WiFiClientSecure.h>
#define WIFI_SSID "ssid"
#define WIFI_PASSWORD "pass"
WiFiUDP UDP;
WakeOnLan WOL(UDP);
const char *macaddr_wyse = "A1:A2:A3:A4:A5:A6";
const char *hypervisor = "B1:B2:B3:B4:B5:B6";

const char _cert[] = R"=EOF=(
-----BEGIN CERTIFICATE-----
MIIDxTCCAq2gAwIBAgIBADANBgkqhkiG9w0BAQsFADCBgzELMAkGA1UEBhMCVVMx
EDAOBgNVBAgTB0FyaXpvbmExEzARBgNVBAcTClNjb3R0c2RhbGUxGjAYBgNVBAoT
EUdvRGFkZHkuY29tLCBJbmMuMTEwLwYDVQQDEyhHbyBEYWRkeSBSb290IENlcnRp
ZmljYXRlIEF1dGhvcml0eSAtIEcyMB4XDTA5MDkwMTAwMDAwMFoXDTM3MTIzMTIz
NTk1OVowgYMxCzAJBgNVBAYTAlVTMRAwDgYDVQQIEwdBcml6b25hMRMwEQYDVQQH
EwpTY290dHNkYWxlMRowGAYDVQQKExFHb0RhZGR5LmNvbSwgSW5jLjExMC8GA1UE
AxMoR28gRGFkZHkgUm9vdCBDZXJ0aWZpY2F0ZSBBdXRob3JpdHkgLSBHMjCCASIw
DQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAL9xYgjx+lk09xvJGKP3gElY6SKD
E6bFIEMBO4Tx5oVJnyfq9oQbTqC023CYxzIBsQU+B07u9PpPL1kwIuerGVZr4oAH
/PMWdYA5UXvl+TW2dE6pjYIT5LY/qQOD+qK+ihVqf94Lw7YZFAXK6sOoBJQ7Rnwy
DfMAZiLIjWltNowRGLfTshxgtDj6AozO091GB94KPutdfMh8+7ArU6SSYmlRJQVh
GkSBjCypQ5Yj36w6gZoOKcUcqeldHraenjAKOc7xiID7S13MMuyFYkMlNAJWJwGR
tDtwKj9useiciAF9n9T521NtYJ2/LOdYq7hfRvzOxBsDPAnrSTFcaUaz4EcCAwEA
AaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMCAQYwHQYDVR0OBBYE
FDqahQcQZyi27/a9BUFuIMGU2g/eMA0GCSqGSIb3DQEBCwUAA4IBAQCZ21151fmX
WWcDYfF+OwYxdS2hII5PZYe096acvNjpL9DbWu7PdIxztDhC2gV7+AJ1uP2lsdeu
9tfeE8tTEH6KRtGX+rcuKxGrkLAngPnon1rpN5+r5N9ss4UXnT3ZJE95kTXWXwTr
gIOrmgIttRD02JDHBHNA7XIloKmf7J6raBKZV8aPEjoJpL1E/QYVN8Gb5DKj7Tjo
2GTzLH4U/ALqn83/B2gX2yKQOC16jdFU8WnjXzPKej17CuPKf1855eJ1usV2GDPO
LPAvTK33sefOT6jEm0pUBsV/fdUID+Ic/n4XuKxe9tQWskMJDE32p2u0mYRlynqI
4uJEvlz36hz1
-----END CERTIFICATE-----
)=EOF=";

#define BOT_TOKEN "0000000000:BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
const unsigned long BOT_MTBS = 1000;
X509List cert(_cert);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime; 

void handleNewMessages(int numNewMessages) {
	for (int i = 0; i < numNewMessages; i++) {
		if (bot.messages[i].text == "/help") {
			bot.sendMessage(bot.messages[i].chat_id, "available commands:\n\n/wol_wyse - sends wake on lan to Wyse \n\n/wol_hypervisor - sends wake on lan Hypervisor", "");
		}
		else if (bot.messages[i].text == "/wol_wyse") {
			WOL.sendMagicPacket(macaddr_wyse);
			bot.sendMessage(bot.messages[i].chat_id, "sent magic packet to Wyse", "");
		} 
		else if (bot.messages[i].text == "/wol_hypervisor") {
			WOL.sendMagicPacket(hypervisor);
			bot.sendMessage(bot.messages[i].chat_id, "sent magic packet to Hypervisor", "");
		}
		else {
			bot.sendMessage(bot.messages[i].chat_id, "Hello, please type /help to see available commands", "");
		}
	}
}

void setup() {
	Serial.begin(115200);
	Serial.println();
	Serial.print("Connecting to Wifi SSID ");
	Serial.print(WIFI_SSID);
	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
	secured_client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org

	while (WiFi.status() != WL_CONNECTED) {
		Serial.print(".");
		delay(1000);
	}
	Serial.print("\nWiFi connected. IP address: ");
	Serial.println(WiFi.localIP());

	Serial.print("Retrieving time: ");
	configTime(0, 0, "pool.ntp.org"); // get UTC time via NTP
	time_t now = time(nullptr);
	while (now < 24 * 3600)	{
		Serial.print(".");
		delay(1000);
		now = time(nullptr);
	}
	Serial.println(now);
	bot.longPoll = 60;
	Serial.println("starting loop...");
}

void loop() {
	if (millis() - bot_lasttime > BOT_MTBS)	{
		int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
		while (numNewMessages) {
			handleNewMessages(numNewMessages);
			numNewMessages = bot.getUpdates(bot.last_message_received + 1);
		}
		bot_lasttime = millis();
	}
	delay(1000);
}
