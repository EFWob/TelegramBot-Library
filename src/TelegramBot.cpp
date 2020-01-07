// Official
// Copyright 
// LGPL License
//
// TelegramBot library
// https://github.com/

#include "TelegramBot.h"

static String payload;


TelegramBot::TelegramBot(const char* token)	{
	_client = new WiFiClientSecure;
	_token=strdup(token);
	_probeUpdateId = 0;
}

TelegramBot::~TelegramBot() {
	if(_token)
		free(_token);
	if (_client->connected())
		_client->stop();
	delete _client;
}

bool TelegramBot::connect()	{
	if(!_client->connected()){
		_client->setInsecure();
		Serial.println("Verbindungsversuch mit Telegram!");
		_client->connect(HOST, SSL_PORT);
		Serial.println(String("Verbunden mit Telegram: ") + (_client->connected()?"YES":"No!"));
		return _client->connected();
	}
	return true;
}


	
TelegramChat::TelegramChat(long long id) {
	_id = id;
	_type = NULL;
	type("undefined");
}


TelegramChat::TelegramChat(JsonObject chatObjectJson) {
	_id = chatObjectJson["id"];
	type(chatObjectJson["type"]);
}
		
TelegramChat::~TelegramChat() {
	if (_type)
		free(_type);
}

long long TelegramChat::id() {
	return(_id);
}

const char* TelegramChat::type() {
	return(_type);
}

void TelegramChat::type(const char* t) {
	if (_type)
		free(_type);
	_type = NULL;
	if (t)
		_type = strdup(t);
}


TelegramMessage::TelegramMessage() {
	_text = NULL;
	_chat = new TelegramChat(0);
};

//TelegramMessage::TelegramMessage(long long chatId, char* text) {
//	_chatId = chatId;
//	if (text)
//		_text = strdup(text);
//	else
//		_text = NULL;
//	_chat = new TelegramChat(0);
//};

TelegramMessage::~TelegramMessage() {
	if (_text)
		free(_text);
	if (_chat)
		delete(_chat);
};

TelegramChat& TelegramMessage::chat() {
	return *_chat;
}

void TelegramMessage::chat(TelegramChat* chat) {
	if (_chat)
		delete(_chat);
	_chat = chat;
}

/*
long long TelegramMessage::chatId() {
	return _chatId;
}

void TelegramMessage::chatId(long long chatId) {
	_chatId = chatId;
}
*/

const char* TelegramMessage::text() {
	return _text;
}

void TelegramMessage::text(const char *text) {
	if (_text)
		free(_text);
	_text = NULL;
	if (text)
		_text = strdup(text);
}

int TelegramMessage::type(void) {
	return _type;
}

void TelegramMessage::type(int type) {
	_type = type;
}


long long TelegramBot::getUpdate(TelegramMessage & m) {
//char s[200];
long long ret = 0;
		if (connect()) {

			_client->print("GET /bot");
			_client->print(_token);
			_client->print("/getUpdates?limit=1&offset=");
			_client->print(_probeUpdateId);
			_client->println("&allowed_update[""message"", ""channel_post"", ""edited_message"", ""edited_channel_post""] HTTP/1.1");

			_client->println("User-Agent: curl/7.37.1");
			_client->println("Host: api.telegram.org");
			_client->println("Accept: */*");
			_client->println();

			readPayload();
//			Serial.println(payload);
//			Serial.println();
//			Serial.println(String("Payload Length: ") + payload.length());
			StaticJsonDocument<JSON_BUFF_SIZE> root;
			auto error = deserializeJson(root, payload);
			if (!error)	{
				JsonObject result0 = root["result"][0];
				int update_id = result0["update_id"];
				update_id = update_id + 1;
				if((_probeUpdateId != update_id) && (update_id - 1)) {
					Serial.println(payload);
					Serial.print("Update id: ");Serial.println(update_id);
					Serial.print("Probe Update id: ");Serial.println(_probeUpdateId);
					_probeUpdateId = update_id;
					JsonObject message;
					if (!(message	= result0["message"]).isNull()) { 
						m.type(TelegramMessage::isMessage);
					}
					else if (!(message = result0["channel_post"]).isNull()) {
						m.type(TelegramMessage::isChannelPost);
					}
					else if(!(message = result0["edited_message"]).isNull()) {
						m.type(TelegramMessage::isEditedMessage);
					}
					else if(!(message = result0["edited_channel_post"]).isNull()) {
						m.type(TelegramMessage::isEditedChannelPost);
					}
					else
						m.type(TelegramMessage::isNull);
					if (!message.isNull())  {
						JsonObject chatObject = message["chat"];
						ret = update_id - 1;
						m.chat(new TelegramChat(chatObject));
						m.text(message["text"]);
					} 
				}
			}
		}
	return ret;
}


void TelegramBot::sendMessage(TelegramMessage& tm) {
long long id = tm.chat().id();
	sendMessage(id, tm.text());
}	

void TelegramBot::sendMessage(long long id, const char* txt) {
	if (id) {
		char msg[500];
		StaticJsonDocument<JSON_BUFF_SIZE> buff;
		buff["chat_id"] = id;
		buff["text"] = txt;
		serializeJson(buff, msg);
		postMessage(msg);
	}
}



void TelegramBot::postMessage(const char* msg) {
//	Serial.println("Send Message Char!");
	if (connect()) {
//		char s[200];
//		sprintf(s, "POST /bot%s/sendMessage HTTP/1.1", token);
		_client->print("POST /bot");
		_client->print(_token);
		_client->println("/sendMessage HTTP/1.1");
		_client->println("Host: api.telegram.org");
	    _client->println("Content-Type: application/json");
//	    client->println("Connection: close");
	    _client->print("Content-Length: ");
//		sprintf(s, "%d", strlen(msg));
		_client->println(strlen(msg));
		_client->println();
		_client->println(msg);
		readPayload();
	}
}

// reads the payload coming from telegram server
// returns the payload string
String TelegramBot::readPayload(){
	char c;

	payload = "";
		//Read the answer and save it in String payload
		while (_client->connected()) {
		payload = _client->readStringUntil('\n');
		if (payload == "\r") {
			break;
		 }
	  }
	payload = _client->readStringUntil('\r');
	// Serial.println(payload);
	return payload;
}
