// Copyright Casa Jasmina 2016
// LGPL License
//
// TelegramBot library
// https://github.com/CasaJasmina/TelegramBot-Library

#ifndef TelegramBot_h
#define TelegramBot_h
#define ARDUINOJSON_USE_LONG_LONG	1
#define ARDUINOJSON_DECODE_UNICODE 1
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Client.h>
#include <TelegramKeyboard.h>

#define HOST "api.telegram.org"
#define SSL_PORT 443

#ifndef JSON_BUFF_SIZE
#ifdef ESP8266
#define JSON_BUFF_SIZE 1000
#else
#define JSON_BUFF_SIZE 10000
#endif
#endif

struct message{
  String text;
  String chat_id;
  String sender;
  String date;
};

class TelegramMessage {
	public:
		TelegramMessage();
		TelegramMessage(long long chatId, char *text);
		~TelegramMessage();
		int type(void);
		void type(int type);
		long long chatId();
		void chatId(long long chatId);
		char* text();
		void text(const char* text);
		static const int isNull = 0;
		static const int isChatMessage = 1;
		static const int isChannelMessage = 2;
	protected:
		long long _chatId;
		char* _text;
		int _type;
};

class TelegramBot
{
  public:
    TelegramBot(const char* token, Client &client);
  	void begin();
    String sendMessage(String chat_id, String text);
    String sendMessage(String chat_id, String text, TelegramKeyboard &keyboard_markup, bool one_time_keyboard = true, bool resize_keyboard = true);
    String postMessage(String msg);
	void postMessage(const char* msg);
    message getUpdates();
	bool getUpdate(TelegramMessage & m);
	void sendMessage(TelegramMessage & tm);
  private:
      String readPayload();
      const char* token;
      int last_message_recived;

      Client *client;
};

#endif
