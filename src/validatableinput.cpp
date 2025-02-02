#include "validatableinput.h"

#include <ftxui/component/component.hpp>

using namespace ftxui;

ftxui::Component ValidatableInput(ftxui::StringRef content, ftxui::StringRef placeholder, Validator validator)
{
    auto cursor = std::make_shared<int>();

    auto option = InputOption::Default();
    option.cursor_position = cursor.get();
    option.multiline = false;

    auto input = Input(content, placeholder, option);
    input |= CatchEvent([&, content, cursor, validator](Event event) {
        return validator(*cursor, *content, std::move(event));
    });

    return input;
}