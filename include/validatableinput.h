#pragma once
#include "inputvalidator.h"

#include <ftxui/component/component_base.hpp>

ftxui::Component ValidatableInput(ftxui::StringRef content, ftxui::StringRef placeholder, Validator validator);
