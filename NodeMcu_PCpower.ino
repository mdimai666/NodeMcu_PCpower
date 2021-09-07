#ifndef UNIT_TEST
#include <Arduino.h>
#endif
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <IRremoteESP8266.h>
#include <WiFiClient.h>

const char *ssid = "DD";
const char *password = "J42ghaQY";
MDNSResponder mdns;

ESP8266WebServer server(80);

#define POWER_PIN 4

void setup()
{
	Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);

	WiFi.begin(ssid, password);
	Serial.println("");

	while (!Serial){;}

	Serial.println("PC power ON relay init\n");

	pinMode(POWER_PIN, OUTPUT);
	digitalWrite(POWER_PIN, HIGH);
	// digitalWrite(POWER_PIN, LOW);

	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.print("Connected to ");
	Serial.println(ssid);
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());

	if (mdns.begin("esp8266", WiFi.localIP()))
	{
		Serial.println("MDNS responder started");
	}

	server.on("/", handleRoot);
	server.on("/cmd", handleCommand);

	server.on("/inline", []()
			  { server.send(200, "text/plain", "this works as well"); });

	server.onNotFound(handleNotFound);

	server.begin();
	Serial.println("HTTP server started");
}

void loop()
{

	// if (0)
	// { //work
	// int khz = 38; // несущая частота 38kHz для протокола NEC
	//   irsend.sendRaw(a_on, sizeof(a_on) / sizeof(a_on[0]), khz);
	//   delay(2000);
	//   irsend.sendRaw(a_off, sizeof(a_off) / sizeof(a_off[0]), khz);
	//   delay(2000);
	// }

	// if (1) //work
	// {
	//   irsend.sendCOOLIX(0xB2BF50);
	//   delay(2000);
	//   irsend.sendCOOLIX(0xB27BE0);
	//   delay(2000);
	// }
	server.handleClient();
}

void handleRoot()
{
	// server.send(200, "text/html",
	// 			"<html>"
	// 			"<head><title>ESP8266 Rele</title></head>"
	// 			"<body>"
	// 			"<h1>Hello from ESP8266, you can send PC on command"
	// 			"</h1>"
	// 			"<p><a href=\"cmd?code=toggle\">Toggle</a></p>"
	// 			"<p><a href=\"cmd?code=on\">ON</a></p>"
	// 			"<p><a href=\"cmd?code=off\">OFF</a></p>"
	// 			"</body>"
	// 			"</html>");

	server.send(200, "text/html",
	R""""(
<!DOCTYPE html>
<html lang="ru">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta http-equiv="X-UA-Compatible" content="ie=edge">
    <meta name="Description" content="Home3" />
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/twitter-bootstrap/4.6.0/css/bootstrap.min.css">
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.15.3/css/all.min.css">
    <!-- <link rel="stylesheet" href="/style.css"> -->

    <meta name="robots" content="noindex">

    <title>Home3</title>
</head>

<body style="height:100vh">
    <main role="main" class="h-100">

        <div class="container h-100">
			<p></p>
			<p></p>
			<p></p>
			<p><a class="btn btn-primary   w-100" href="cmd?code=start">Tick</a></p>
			<p><a class="btn btn-secondary w-100" href="cmd?code=on">ON</a></p>
			<p><a class="btn btn-secondary w-100" href="cmd?code=off">OFF</a></p>
        </div>

    </main>

    <script src="https://cdnjs.cloudflare.com/ajax/libs/jquery/3.5.1/jquery.min.js"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.16.1/umd/popper.min.js"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/twitter-bootstrap/4.6.0/js/bootstrap.min.js"></script>

    <script src="https://cdnjs.cloudflare.com/ajax/libs/toastr.js/latest/toastr.min.js"
        integrity="sha512-VEd+nq25CkR676O+pLBnDW09R7VQX9Mdiij052gVCp5yVH3jGtH70Ho/UUv4mJDsEdTvqRCFZg0NKGiojGnUCw=="
        crossorigin="anonymous" referrerpolicy="no-referrer"></script>
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/toastr.js/latest/toastr.min.css"
        integrity="sha512-vKMx8UnXk60zUwyUnUPM3HbQo8QfmNx7+ltw8Pm5zLusl1XIfwcxo8DbWCqMGKaWeNxWA8yrx5v3SaVpMvR3CA=="
        crossorigin="anonymous" referrerpolicy="no-referrer" />

    <script>
        document.addEventListener("DOMContentLoaded", function () {
            $(function () {
                // if(defkey(window,'moment'))moment.locale('ru');
                
                //Success,Info,Warning,Error
                toastr.options = {
                    "closeButton": false,
                    "debug": false,
                    "newestOnTop": true,
                    "progressBar": false,
                    "positionClass": "toast-top-right",
                    "preventDuplicates": false,
                    "onclick": null,
                    "showDuration": "300",
                    "hideDuration": "1000",
                    "timeOut": "5000",
                    "extendedTimeOut": "1000",
                    "showEasing": "swing",
                    "hideEasing": "linear",
                    "showMethod": "fadeIn",
                    "hideMethod": "fadeOut"
                };
                
                window.onerror = function (msg, url, linenumber) {
                // var t = ('Error message: ' + msg);
                toastr.error("JavaScript", msg);
                    // callError(msg);
                }
                
            });
            
        
        });
    </script>
</body>
</html>
)"""");

}

void handleCommand()
{
	for (uint8_t i = 0; i < server.args(); i++)
	{
		if (server.argName(i) == "code")
		{
			String val = server.arg(i);

			Serial.print("input arg: code=");
			Serial.println(val);

			if(val == "start"){
				digitalWrite(POWER_PIN, LOW);
				delay(600);
				digitalWrite(POWER_PIN, HIGH);
				
			} else if(val == "on"){
				digitalWrite(POWER_PIN, LOW);
				
			} else if(val == "off"){
				digitalWrite(POWER_PIN, HIGH);
				
			} 

		}
	}
	handleRoot();
}

void handleNotFound()
{
	String message = "File Not Found\n\n";
	message += "URI: ";
	message += server.uri();
	message += "\nMethod: ";
	message += (server.method() == HTTP_GET) ? "GET" : "POST";
	message += "\nArguments: ";
	message += server.args();
	message += "\n";
	for (uint8_t i = 0; i < server.args(); i++)
		message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
	server.send(404, "text/plain", message);
}