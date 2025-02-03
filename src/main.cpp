#include "binarydatabase.h"
#include "inputtable.h"
#include "inputvalidator.h"
#include "qsort.h"

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/table.hpp>

#include <memory>
#include <cmath>

using namespace ftxui;

class App
{
private:
    enum Tabs
    {
        Input,
        SelectFromDB
    };

public:
    App(std::unique_ptr<Database> dbi)
        : m_dbi(std::move(dbi))
    {
        m_inputStrData.resize(1);
    }

    void run()
    {
        auto screen = ScreenInteractive::Fullscreen();

        Component layout = Container::Tab({
            InputTab(),
            SelectFromDBTab(),
        }, &m_tabSelector);

        screen.Loop(layout);
    }

// Input (main) Tab
private:
    Component InputTab()
    {
        m_inputTable = InputTable(m_inputStrData, IntegerValidator);

        Component layout = Container::Horizontal({
            ControlPanel() | yflex_grow | borderRounded,
            m_inputTable | frame | flex_grow,
            Container::Vertical({
                AnimatedButton("Sort", [&] { DoSort(); })
            }),
            OutputTable() | frame | flex_grow
        }) | flex_grow;

        return layout;
    }

    Component ControlPanel()
    {
        auto hasSelectedArray = [&] { return m_selectedDataId != -1; };

        return Container::Vertical({
            Container::Vertical({
                DatabaseWindow(),
                AnimatedButton("Clear DB", [&] { m_dbi->Clear(); m_selectedDataId = -1; }),
                AnimatedButton("Create New", [&] { CreateNew(); }),
                AnimatedButton("Select From DB", [&] { UpdateDBArrays(); m_tabSelector = Tabs::SelectFromDB; }),
                AnimatedButton("Save As New", [&] {
                    if (m_data.sorted.empty() || m_outputStrData.empty()) return; 
                    m_dbi->Add(m_data);
                    m_selectedDataId = m_dbi->GetCount() - 1;
                }),
                Maybe(Container::Vertical({
                    Renderer([] { return separator(); }),
                    AnimatedButton("Save", [&] { m_dbi->Modify(m_selectedDataId, m_data); }),
                    AnimatedButton("Delete", [&] { m_dbi->Remove(m_selectedDataId); m_selectedDataId = -1; }),
                }), hasSelectedArray)
            }) | flex_grow,
        });
    }

    Component OutputTable()
    {
        auto options = MenuOption::Vertical();

        auto menu = Menu(&m_outputStrData, &m_outputSelector, options);
        return Renderer(menu, [&, menu] {
            return vbox({
                text("Output") | borderDouble,
                hbox({
                    separator(), 
                    menu->Render() | xflex_grow | color(Color::White),
                    separator()
                }),
                separator()
            });
        });
    }

    Component DatabaseWindow()
    {
        return Renderer([&] {
            auto statusColor = m_dbi->GetStatus() == Database::Status::Ready ? Color::Green : Color::Red;
            auto content = vbox({
                hbox({
                    text(" - Status: "), text(fmt::format("{} ", m_dbi->GetStatus())) | bold | color(statusColor)
                }),
                text(fmt::format(" - Record count: {} ", m_dbi->GetCount())),
                text(fmt::format(" - Selected: {}", m_selectedDataId))
            });
            return window(text(" Database "), content);
        });
    }

    void DoSort()
    {
        m_outputStrData.clear();

        Data data;
        for (const auto& in : m_inputStrData)
        {
            try
            {
                data.original.push_back(std::stoi(in));
            }
            catch (const std::exception& e)
            {
                m_outputStrData.push_back(std::string("Exception raised while parsing input data: ") + e.what());
                SPDLOG_INFO("Exception raised while parsing input data: {}", e.what());
                return;
            }
        }

        data.sorted = data.original;
        QuickSort(data.sorted.begin(), data.sorted.end(), std::less());

        m_data = std::move(data);
        UpdateOutputData();
    }

    void ApplySelectedArray()
    {
        if (m_selectedDataId == -1) return;

        m_data = m_dbi->Get(m_selectedDataId);

        UpdateInputData();
        UpdateOutputData();
    }

    void CreateNew()
    {
        m_selectedDataId = -1;
        m_outputStrData.clear();
        m_inputTable->OnEvent(Event::Special("Clear"));
    }

    void UpdateInputData()
    {
        m_inputStrData.clear();
        for (auto value : m_data.original)
        {
            m_inputStrData.push_back(std::to_string(value));
        }
        m_inputTable->OnEvent(Event::Special("Update"));
    }

    void UpdateOutputData()
    {
        m_outputStrData.clear();
        for (auto value : m_data.sorted)
        {
            m_outputStrData.push_back(std::to_string(value));
        }
    }

    void UpdateDBArrays()
    {
        m_dataSelection.clear();

        for (const auto& data : m_dbi->GetAll())
        {
            m_dataSelection.push_back(fmt::format("[{}]\n[{}]", data.original, data.sorted));
        }

        m_selectedDataId = 0;
    }

// Select From DB Tab
private:
    Component SelectFromDBTab()
    {
        auto options = MenuOption::Vertical();
        auto apply = [&] { ApplySelectedArray(); m_tabSelector = Tabs::Input; };
        options.on_enter = apply;

        Component layout = Container::Horizontal({
            AnimatedButton("Select", apply) | border,
            Container::Vertical({
                Menu(&m_dataSelection, &m_selectedDataId, std::move(options))
            }) | border | flex_grow
        });

        return layout;
    }

// Common components
private:
    Component AnimatedButton(const std::string& lable, std::function<void()> onClick)
    {
        return Button(lable, onClick, ButtonOption::Animated(0xeee4da_rgb, 0x776e65_rgb));
    }

private:
    std::unique_ptr<Database> m_dbi;

    Component m_inputTable;

    std::vector<std::string> m_inputStrData;
    std::vector<std::string> m_outputStrData;
    int m_outputSelector = 0;

    std::vector<std::string> m_dataSelection;
    int m_selectedDataId = -1;

    Data m_data;

    int m_tabSelector = Tabs::Input;
};

int main()
{
    spdlog::global_logger()->sinks() = { std::make_shared<spdlog::sinks::basic_file_sink_st>("logs/main.log", true) };

    App app(std::make_unique<BinaryDatabase>("database.dat"));
    app.run();
}
