#include <Arduino.h>
#include <Wire.h>

#include "speech_recognizer.h"
#include "text_to_speech_synthesizer.h"

SpeechRecognizer g_speech_recognizer(0x30);
TextToSpeechSynthesizer g_text_to_speech_synthesizer;
constexpr auto g_led_pin = 3;  // led灯

void setup() {
  Serial.begin(115200);
  Serial.println("setup");
  g_speech_recognizer.Setup();
  g_speech_recognizer.SetTrigger(SpeechRecognizer::kTriggerKeyword);  // 设置为关键词唤醒模式
  g_speech_recognizer.AddIdentification(0, F("xiao yi xiao yi"));     // 添加小易小易词条为关键词，index为0
  g_speech_recognizer.AddIdentification(1, F("qing kai deng"));       // 添加"请开灯"词条
  g_speech_recognizer.AddIdentification(2, F("qing guang deng"));     // 添加"请关灯"词条
  pinMode(g_led_pin, OUTPUT);
  digitalWrite(g_led_pin, 0);  // 默认关闭led灯
  Serial.println("setup done");
}

void loop() {
  switch (g_speech_recognizer.ReadEvent()) {
    case SpeechRecognizer::kEventTriggeredByKeyword: {
      Serial.println(F("kEventTriggeredByKeyword"));
      // 识别到关键词，播报"在呢"
      g_text_to_speech_synthesizer.StartSynthesizing(F("在呢"), TextToSpeechSynthesizer::kTextEncodingTypeUtf8);
      break;
    }
    case SpeechRecognizer::kEventRecognizeSuccess: {
      Serial.println(F("kEventRecognizeSuccess"));
      auto result = g_speech_recognizer.ReadResult();
      Serial.print("result:");
      Serial.println(result);
      switch (result) {
        case 1: {
          // 识别到"请开灯"，播报"已开灯"，并打开led灯
          g_text_to_speech_synthesizer.StartSynthesizing(F("已开灯"), TextToSpeechSynthesizer::kTextEncodingTypeUtf8);
          digitalWrite(g_led_pin, 1);
          break;
        }
        case 2: {
          // 识别到"请关灯"，播报"已关灯"，并关闭led灯
          g_text_to_speech_synthesizer.StartSynthesizing(F("已关灯"), TextToSpeechSynthesizer::kTextEncodingTypeUtf8);
          digitalWrite(g_led_pin, 0);
          break;
        }
        default:
          break;
      }
      break;
    }
    case SpeechRecognizer::kEventRecognizeTimeout: {
      // 小易小易唤醒后，如果超时未识别到新的指令，则播报"有需要再叫我"
      Serial.println(F("kEventStartRecognizing"));
      g_text_to_speech_synthesizer.StartSynthesizing(F("有需要再叫我"),
                                                     TextToSpeechSynthesizer::TextEncodingType::kTextEncodingTypeUtf8);
      break;
    }
    default:
      break;
  }
}
