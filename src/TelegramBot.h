// Copyright 
// LGPL License
//
// TelegramBot library
// https://github.com/

#ifndef TelegramBot_h
#define TelegramBot_h
#define ARDUINOJSON_USE_LONG_LONG	1
#define ARDUINOJSON_DECODE_UNICODE 1
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Client.h>
#include <WiFiClientSecure.h>
//#include <TelegramKeyboard.h>

#define HOST "api.telegram.org"
#define SSL_PORT 443

#ifndef JSON_BUFF_SIZE
#ifdef ESP8266
#define JSON_BUFF_SIZE 1000
#else
#define JSON_BUFF_SIZE 10000
#endif
#endif

class TelegramChat {
	public:
		TelegramChat(long long id);
		TelegramChat(JsonObject chatObjectJson);
		~TelegramChat();	
		long long id();
		const char* type();
	private:
		void type(const char* t);
		
		char* _type = NULL;
		long long _id = 0;
};

class TelegramMessage {
	public:
		TelegramMessage();
		~TelegramMessage();
		TelegramChat& chat();
		void chat(TelegramChat* chat);
		const char* text();
		void text(const char* text);
		int type();
		void type(int type);
		static const int isNull = 0;
		static const int isMessage = 1;
		static const int isEditedMessage = 2;
		static const int isChannelPost = 3;
		static const int isEditedChannelPost = 4;
	protected:
		TelegramChat* _chat = NULL;
		char* _text = NULL;
		int _type = isNull;
};

class TelegramBot
{
  public:
    TelegramBot(const char* token);
	~TelegramBot();
	void sendMessage(long long id, const char* txt);
	void postMessage(const char* msg);
	long long getUpdate(TelegramMessage & m);
	void sendMessage(TelegramMessage & tm);
  private:
      bool connect();
      String readPayload();
      char* _token;
      int _probeUpdateId;
      WiFiClientSecure *_client;
};

#endif
