#pragma once
#include "inputvalidator.h"

#include <ftxui/component/component_base.hpp>

class InputTableImpl : public ftxui::ComponentBase
{
public:
    InputTableImpl(std::vector<std::string>& values, Validator validator = nullptr);

    ftxui::Element Render() override;

    bool OnEvent(ftxui::Event event) override;

private:
    ftxui::Component CreateInput(ftxui::StringRef content, ftxui::StringRef placeholder);
    void UpdateInputs();

private:
    std::vector<std::string>& m_values;
    Validator m_validator;

    ftxui::Components m_inputs;
    ftxui::Component m_container;
    int m_selector;
};

ftxui::Component InputTable(std::vector<std::string>& values, Validator validator = nullptr);
