#include "inputtable.h"
#include "validatableinput.h"

#include <ftxui/component/component.hpp>
#include <ftxui/dom/table.hpp>

using namespace ftxui;

constexpr auto InputPlaceholder = "Input";

InputTableImpl::InputTableImpl(std::vector<std::string> &values, Validator validator)
    : m_values(values)
    , m_validator(std::move(validator))
{
    if (values.empty())
    {
        values.resize(1);
    }

    m_inputs.push_back(CreateInput(&values[0], InputPlaceholder));
    m_container = Container::Vertical(m_inputs, &m_selector);

    Add(m_container);
}

ftxui::Element InputTableImpl::Render()
{
    std::vector<Elements> elements = {{ text("Input") | xflex }};

    for (auto&& component : m_inputs)
    {
        elements.push_back({ component->Render() | xflex });
    }

    Table table(elements);
    table.SelectAll().Border(LIGHT);

    table.SelectColumn(0).Border(LIGHT);
    
    table.SelectRow(0).Decorate(bold);
    table.SelectRow(0).SeparatorVertical(LIGHT);
    table.SelectRow(0).Border(DOUBLE);
    
    auto content = table.SelectRows(1, -1);
    content.DecorateCellsAlternateRow(color(Color::Blue), 3, 0);
    content.DecorateCellsAlternateRow(color(Color::Cyan), 3, 1);
    content.DecorateCellsAlternateRow(color(Color::White), 3, 2);

    return table.Render();
}

bool InputTableImpl::OnEvent(ftxui::Event event)
{
    if (event == Event::Special("Clear"))
    {
        m_values.clear();
        m_values.resize(1);
        UpdateInputs();
        return true;
    }
    else if (event == Event::Special("Update"))
    {
        UpdateInputs();
        return true;
    }
    return ComponentBase::OnEvent(std::move(event));
}

Component InputTableImpl::CreateInput(StringRef content, StringRef placeholder)
{
    auto input = ValidatableInput(content, placeholder, m_validator);

    input |= CatchEvent([&](Event event) {
        int& i = m_selector;

        if (event == Event::Backspace || event == Event::Delete)
        {
            if (m_values.size() == 1 || !m_values[i].empty())
            {
                return false;
            }
            m_values.erase(m_values.begin() + i);
            UpdateInputs();

            i = event == Event::Backspace ? i - 1 : std::min((int)m_container->ChildCount() - 1, i);

            return true;
        }
        else if (event == Event::Return)
        {
            m_values.insert(m_values.begin() + i + 1, "");
            i++;
            UpdateInputs();
            return true;
        }
        return false;
    });

    return input;
}

void InputTableImpl::UpdateInputs()
{
    m_container->DetachAllChildren();
    m_inputs.clear();
    for (auto& value : m_values)
    {
        m_container->Add(m_inputs.emplace_back(CreateInput(&value, InputPlaceholder)));
    }
}

Component InputTable(std::vector<std::string>& values, Validator validator)
{
    return Make<InputTableImpl>(values, std::move(validator));
}
