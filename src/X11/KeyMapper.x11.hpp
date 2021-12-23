#pragma once

#include <X11/keysymdef.h>
#include <xkbcommon/xkbcommon-x11.h>

#include <map>

#include "NamelessWindow/Events/Key.hpp"

namespace NLSWIN {

class X11KeyMapper {
   public:
   static KeyValue TranslateKey(xkb_keysym_t keysym) {
      if (mTranslationTable.find(keysym) == mTranslationTable.end()) {
         return KeyValue::KEY_NULL;
      }
      return mTranslationTable.at(keysym);
   }

   private:
   static std::map<xkb_keysym_t, KeyValue> mTranslationTable;
};
}