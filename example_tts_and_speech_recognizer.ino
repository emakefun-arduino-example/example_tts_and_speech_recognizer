#include <Arduino.h>

#include "speech_recognizer.h"
#include "tts.h"

emakefun::SpeechRecognizer g_speech_recognizer(emakefun::SpeechRecognizer::kDefaultI2cAddress);
emakefun::Tts g_tts(emakefun::Tts::kDefaultI2cAddress);
constexpr auto g_led_pin = 3;  // led灯

void setup() {
  Serial.begin(115200);
  Serial.println("setup");
  g_tts.Initialize();                                                                   // 语音合成播报模块初始化
  g_speech_recognizer.Initialize();                                                     // 语音识别模块初始化
  g_speech_recognizer.SetRecognitionMode(emakefun::SpeechRecognizer::kKeywordTrigger);  // 设置为关键词唤醒模式
  g_speech_recognizer.SetTimeout(5000);                                                 // 设置超时时间为5s
  g_speech_recognizer.AddKeyword(0, F("xiao yi xiao yi"));  // 添加小易小易词条为关键词，index为0
  g_speech_recognizer.AddKeyword(1, F("qing kai deng"));    // 添加"请开灯"词条
  g_speech_recognizer.AddKeyword(2, F("qing guan deng"));   // 添加"请关灯"词条
  pinMode(g_led_pin, OUTPUT);
  digitalWrite(g_led_pin, 0);  // 默认关闭led灯
  Serial.println("setup done");
}

void loop() {
  // 语音识别模块进行识别并获取结果
  const auto result = g_speech_recognizer.Recognize();
  switch (result) {
    case 1: {
      // 识别到"请开灯"，播报"已开灯"，并打开led灯
      g_tts.Play(F("已开灯"));
      digitalWrite(g_led_pin, 1);
      break;
    }
    case 2: {
      // 识别到"请关灯"，播报"已关灯"，并关闭led灯
      g_tts.Play(F("已关灯"));
      digitalWrite(g_led_pin, 0);
      break;
    }
    default: {
      break;
    }
  }

  // 对各种事件进行处理
  switch (g_speech_recognizer.GetEvent()) {
    // 如果被关键词唤醒
    case emakefun::SpeechRecognizer::kEventKeywordTriggered: {
      // 识别到关键词，播报"在呢"
      g_tts.Play(F("在呢"));
      break;
    }

    // 如果识别超时
    case emakefun::SpeechRecognizer::kEventSpeechRecognitionTimedOut: {
      // 小易小易唤醒后，如果超时未识别到新的指令，则播报"有需要再叫我"
      g_tts.Play(F("有需要再叫我"));
      break;
    }

    default: {
      break;
    }
  }
}
