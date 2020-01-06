// Official
// Copyright Casa Jasmina 2016
// LGPL License
//
// TelegramBot library
// https://github.com/CasaJasmina/TelegramBot-Library

#include "TelegramBot.h"

static String payload;
static message m;

TelegramBot::TelegramBot(const char* token, Client &client)	{
	this->client = &client;
	this->token=token;
}


void TelegramBot::begin()	{
	if(!client->connected()){
//		Serial.println("Try client connection!");
		client->connect(HOST, SSL_PORT);
	}
//	Serial.println(String("Client connected: ") + (client->connected()?"YES":"No!"));
}

/************************************************************************************
 * GetUpdates - function to receive messages from telegram as a Json and parse them *
 ************************************************************************************/
message TelegramBot::getUpdates()  {
		begin();
		m.chat_id = "";
		if (client->connected()) {
			//Send your request to api.telegram.org
			String getRequest = "GET /bot"+String(token)+"/getUpdates?limit=1&offset="+String(last_message_recived)+" HTTP/1.1";
			client->println(getRequest);
			client->println("User-Agent: curl/7.37.1");
			client->println("Host: api.telegram.org");
			client->println("Accept: */*");
			client->println();

			readPayload();
			Serial.println(payload);
//			Serial.println();
//			Serial.println(String("Payload Length: ") + payload.length());
		}
		else
			payload = "";
	    if (payload != "") {

#if defined (ARDUINOJSON5)
			StaticJsonBuffer<JSON_BUFF_SIZE> jsonBuffer;
			JsonObject & root = jsonBuffer.parseObject(payload);
			if(root.success()){
				JsonObject & result0 = root["result"][0];
#else
			StaticJsonDocument<JSON_BUFF_SIZE> root;
			auto error = deserializeJson(root, payload);
			if (!error)	{
				JsonObject result0 = root["result"][0];
#endif




				int update_id = root["result"][0]["update_id"];
				update_id = update_id+1;
//				Serial.println(String("Identifizierte Update-ID: ") + update_id);
				if((last_message_recived != update_id) && (update_id - 1)) {
					Serial.println(String("Muss neue Nachricht sein: ") + update_id + String(" vs.: ") + last_message_recived);
					last_message_recived=update_id;
#if defined (ARDUINOJSON5)
					JsonObject & message = result0["message"];
					if (message.success()) {
#else
					JsonObject message = result0["message"];
					if (!message.isNull()) {
#endif
						Serial.println("Standard Chat Message erhalten!");
						String sender = message["from"]["username"];
						String text = message["text"];
						String chat_id = message["chat"]["id"];
						String date = message["date"];

						m.sender = sender;
						m.text = text;
						m.chat_id = chat_id;
						m.date = date;
					} else {
#if defined (ARDUINOJSON5)
						JsonObject & message = result0["channel_post"];
						if (message.success()) {
#else
						JsonObject message = result0["channel_post"];
						if (!message.isNull()) {
#endif
							long long chat_id = message["chat"]["id"];
							char s[50];
							sprintf(s, "%lld", chat_id);
							Serial.println("Channel Message erhalten!");
							String text = message["text"];
							String date = message["date"];
							Serial.print("ChatId Im Channel: ");Serial.println(s);
							m.sender = "";
							m.text = text;
							m.chat_id = s;
							m.date = date;

						}
					}
				}else{
//					m.chat_id = "";
//					return m;
				}
			}
			else{
				Serial.println("");
				Serial.println("Message too long, skipped.");
				Serial.println("");
				int update_id_first_digit=0;
				int update_id_last_digit=0;
				for(int a =0; a<3; a++){
					update_id_first_digit= payload.indexOf(':',update_id_first_digit+1);
				}
				for(int a =0; a<2; a++){
					update_id_last_digit= payload.indexOf(',',update_id_last_digit+1);
				}
			last_message_recived = payload.substring(update_id_first_digit+1,update_id_last_digit).toInt() +1;
			}
		}
	return m;
	}
	
TelegramMessage::TelegramMessage() {
	_chatId = 0;
	_text = NULL;
	type(TelegramMessage::isNull);
//	_type = TelegramMessage::isNull;
};

TelegramMessage::TelegramMessage(long long chatId, char* text) {
	_chatId = chatId;
	if (text)
		_text = strdup(text);
	else
		_text = NULL;
	type(TelegramMessage::isNull);
	//	_type = TelegramMessage::isNull;

};

TelegramMessage::~TelegramMessage() {
	if (_text)
		free(_text);
};

int TelegramMessage::type(void) {
	return _type;
};

void TelegramMessage::type(int type) {
	_type = type;
}

long long TelegramMessage::chatId() {
	return _chatId;
}

void TelegramMessage::chatId(long long chatId) {
	_chatId = chatId;
}

char* TelegramMessage::text() {
	return _text;
}

void TelegramMessage::text(const char *text) {
	if (_text)
		free(_text);
	_text = NULL;
	if (text)
		_text = strdup(text);
}

bool TelegramBot::getUpdate(TelegramMessage & m) {
char s[200];
		begin();
		m.type(TelegramMessage::isNull);
		if (client->connected()) {
			//Send your request to api.telegram.org
			sprintf(s, "GET /bot%s/getUpdates?limit=1&offset=%d HTTP/1.1", token, last_message_recived);
//			String getRequest = "GET /bot"+String(token)+"/getUpdates?limit=1&offset="+String(last_message_recived)+" HTTP/1.1";
//			client->println(getRequest);
			client->println(s);
			client->println("User-Agent: curl/7.37.1");
			client->println("Host: api.telegram.org");
			client->println("Accept: */*");
			client->println();

			readPayload();
			Serial.println(payload);
//			Serial.println();
//			Serial.println(String("Payload Length: ") + payload.length());
			StaticJsonDocument<JSON_BUFF_SIZE> root;
			auto error = deserializeJson(root, payload);
			if (!error)	{
				JsonObject result0 = root["result"][0];
				int update_id = result0["update_id"];
				update_id = update_id + 1;
				if((last_message_recived != update_id) && (update_id - 1)) {
					last_message_recived = update_id;
					JsonObject message;
					message	= result0["message"];
					if (!message.isNull()) {
						m.type(TelegramMessage::isChatMessage);
						m.chatId(message["chat"]["id"]);
						m.text(message["text"]);
					} else {
						message = result0["channel_post"];
						if (!message.isNull()) {
							m.type(TelegramMessage::isChannelMessage);
							m.chatId(message["chat"]["id"]);
							m.text(message["text"]);
						}
					}
				}
			}
		}
	return m.type() != TelegramMessage::isNull;
}


void TelegramBot::sendMessage(TelegramMessage& tm) {
	char msg[500];
	StaticJsonDocument<JSON_BUFF_SIZE> buff;
	buff["chat_id"] = tm.chatId();
	buff["text"] = tm.text();
	serializeJson(buff, msg);
	postMessage(msg);
}

// send message function
// send a simple text message to a telegram char
String TelegramBot::sendMessage(String chat_id, String text)  {
	Serial.println(String("Send Message (") + chat_id + "): " +text); 
	if(chat_id!="0" && chat_id!=""){
#if defined (ARDUINOJSON5)
		StaticJsonBuffer<JSON_BUFF_SIZE> jsonBuffer;
		JsonObject& buff = jsonBuffer.createObject();
#else
		StaticJsonDocument<JSON_BUFF_SIZE> buff;
#endif	
		buff["chat_id"] = chat_id;
		buff["text"] = text;

		String msg;
#if defined (ARDUINOJSON5)		
		buff.printTo(msg);
#else
		serializeJson(buff, msg);
#endif
		return postMessage(msg);
	} else {
		Serial.println("Chat_id not defined");
	}
}

// send a message to a telegram chat with a reply markup
String TelegramBot::sendMessage(String chat_id, String text, TelegramKeyboard &keyboard_markup, bool one_time_keyboard, bool resize_keyboard)  {
#if defined (ARDUINOJSON5)
		StaticJsonBuffer<JSON_BUFF_SIZE> jsonBuffer;
		JsonObject& buff = jsonBuffer.createObject();
#else
		StaticJsonDocument<JSON_BUFF_SIZE> buff;
#endif	
		buff["chat_id"] = chat_id;
		buff["text"] = text;
#if defined (ARDUINOJSON5)
		JsonObject& reply_markup = buff.createNestedObject("reply_markup");
		JsonArray& keyboard = reply_markup.createNestedArray("keyboard");
#else
		JsonObject reply_markup = buff.createNestedObject("reply_markup");
		JsonArray keyboard = reply_markup.createNestedArray("keyboard");
	
#endif
		for (int a = 1 ; a <= keyboard_markup.length() ; a++){
#if defined (ARDUINOJSON5)
			JsonArray& row = keyboard.createNestedArray();
#else
			JsonArray row = keyboard.createNestedArray();
#endif
				for( int b = 1; b <= keyboard_markup.rowSize(a) ; b++){
					row.add(keyboard_markup.getButton(a,b));
				}
		}
#if defined (ARDUINOJSON5)
		reply_markup.set<bool>("one_time_keyboard", one_time_keyboard);
		reply_markup.set<bool>("resize_keyboard", resize_keyboard);
		reply_markup.set<bool>("selective", false);
#else
		reply_markup["one_time_keyboard"] = (bool)one_time_keyboard;
		reply_markup["resize_keyboard"] = (bool)resize_keyboard;
		reply_markup["selective"] = false;
#endif
		String msg;
#if defined (ARDUINOJSON5)		
		buff.printTo(msg);
#else
		serializeJson(buff, msg);
#endif
		// Serial.println(msg);
		return postMessage(msg);
}

// gets the telegram json string
// posts the message to telegram
// returns the payload
String TelegramBot::postMessage(String msg) {
		begin();

		client->println("POST /bot"+String(token)+"/sendMessage"+" HTTP/1.1");
		client->println("Host: api.telegram.org");
	    client->println("Content-Type: application/json");
	    client->println("Connection: close");
	    client->print("Content-Length: ");
	    client->println(msg.length());
	    client->println();
	    client->println(msg);

		return readPayload();
}

void TelegramBot::postMessage(const char* msg) {
	begin();
	Serial.println("Send Message Char!");
	if (client->connected()) {
//		char s[200];
//		sprintf(s, "POST /bot%s/sendMessage HTTP/1.1", token);
		client->print("POST /bot");
		client->print(token);
		client->println("/sendMessage HTTP/1.1");
		client->println("Host: api.telegram.org");
	    client->println("Content-Type: application/json");
	    client->println("Connection: close");
	    client->print("Content-Length: ");
//		sprintf(s, "%d", strlen(msg));
		client->println(strlen(msg));
		client->println();
		client->println(msg);
		readPayload();
	}
}

// reads the payload coming from telegram server
// returns the payload string
String TelegramBot::readPayload(){
	char c;

	payload = "";
		//Read the answer and save it in String payload
		while (client->connected()) {
		payload = client->readStringUntil('\n');
		if (payload == "\r") {
			break;
		 }
	  }
	payload = client->readStringUntil('\r');
	// Serial.println(payload);
	return payload;
}
