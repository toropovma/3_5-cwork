#pragma once
#include <ftxui/component/event.hpp>

using Validator = std::function<bool(int, const std::string&, ftxui::Event)>;

inline bool IntegerValidator(int cursor, const std::string& string, ftxui::Event event)
{
    if (!event.is_character()) return false;
    if (cursor == 0 && string[0] == '-') return true;
    if (event.character()[0] == '-' && cursor == 0) return false;
    return !std::isdigit(event.character()[0]);
}
