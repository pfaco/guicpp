#include <gui/gui.h>
#include <chrono>
#include <thread>

struct ApplicationState
{
        int nclicks = 0;
        bool running = true;
        std::string nclicks_str;
};

int main(int argc, char *argv[])
{
        using namespace guicpp;
        ApplicationState state;
        Application app(Size{800,600}, "Application");
        app.init();

        state.nclicks_str = fmt::format("Number of clicks = {}", state.nclicks);

        app.add(Window("Main Window", Size{400,400})
            .add(HBox()
                .add(OnClickButton("Click", [&]() {
                        state.nclicks++; 
                        state.nclicks_str = fmt::format("Number of Clicks = {}", state.nclicks);
                        app.info("Button clicked");
                }))
                .add(OnClickButton("Exit", [&]() {
                        state.running = false;
                }))
                .add(LabelString(state.nclicks_str))
            )
        );

        while (state.running && app.should_run()) {
                app.run();
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        return 0;
}
