#pragma once

#include <X11/keysymdef.h>
#include <xkbcommon/xkbcommon-x11.h>

#include <map>

#include "NamelessWindow/Events/Key.hpp"

namespace NLSWIN {

class X11InputMapper {
   public:
   static KeyValue TranslateKey(xkb_keysym_t keysym) {
      if (m_keyTranslationTable.find(keysym) == m_keyTranslationTable.end()) {
         return KeyValue::KEY_NULL;
      }
      return m_keyTranslationTable.at(keysym);
   }

   static ButtonValue TranslateButton(uint16_t x11Value) {
      if (m_buttonTranslationTable.find(x11Value) == m_buttonTranslationTable.end()) {
         return ButtonValue::NULLCLICK;
      }
      return m_buttonTranslationTable.at(x11Value);
   }

   private:
   static std::map<xkb_keysym_t, KeyValue> m_keyTranslationTable;
   static std::map<uint16_t, ButtonValue> m_buttonTranslationTable;
};
}  // namespace NLSWIN