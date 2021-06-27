#pragma once

#include <functional>
#include <vector>
#include <fmt/format.h>
#include <fmt/chrono.h>
#include <chrono>
#include <gui/backend.h>

namespace guicpp
{

struct Size
{
    int width;
    int height;
};

struct Position
{
    int x;
    int y;
};

struct vtable {
    void (*draw)(void* ptr);
    void (*destroy_)(void* ptr);
    void* (*clone_)(void *ptr);
    void* (*move_clone_)(void *ptr);
};

template<typename T>
constexpr vtable vtable_for {
    [](void* ptr) { static_cast<T*>(ptr)->draw(); },
    [](void* ptr) { delete static_cast<T*>(ptr); },
    [](void* ptr) -> void* { return new T(*static_cast<T*>(ptr)); },
    [](void* ptr) -> void* { return new T(std::move(*static_cast<T*>(ptr))); }
};

struct Widget
{
    void *concrete_;
    vtable const* vtable_;

    template<typename T>
    Widget(T const& t) :
        concrete_(new T(t)),
        vtable_(&vtable_for<T>)
    {}

    Widget(Widget const& rhs) :
        concrete_(rhs.vtable_->clone_(rhs.concrete_)),
        vtable_(rhs.vtable_)
    {}

    Widget(Widget&& rhs) :
        concrete_(rhs.vtable_->move_clone_(rhs.concrete_)),
        vtable_(rhs.vtable_)
    {}

    void draw() const {
        vtable_->draw(concrete_);
    }

    ~Widget() {
        vtable_->destroy_(concrete_);
    }
};

class Label
{
public:
    Label(const char* text, Size size = {}) : text_{text}, size_{size} {};
    void draw() const;

private:
    const char * text_;
    Size size_;
};

class LabelString
{
public:
    LabelString(std::string &text, Size size = {}) : text_{text}, size_{size} {};
    void draw() const;

private:
    std::string &text_;
    Size size_;
};

class Separator
{
public:
    void draw() const;
};

class FrameRateLabel
{
public:
    void draw() const;
};

class OnClickButton
{
public:
    OnClickButton(const char* text, std::function<void(void)> onclick, Size size={}) : text_{text}, onclick_(onclick), size_{size} {};
    void draw() const;

private:
    const char * text_;
    std::function<void(void)> onclick_;
    Size size_;
};

class StateButton
{
public:
    StateButton(const char* text, bool &is_pressed, Size size={}) : text_{text}, is_pressed_(is_pressed), size_{size} {};
    void draw() const;

private:
    const char * text_;
    bool &is_pressed_;
    Size size_;
};

class CheckBox
{
public:
    CheckBox(const char* text, bool &is_selected, Size size = {}) : text_{text}, is_selected_(is_selected), size_{size} {};
    void draw() const;

private:
    const char * text_;
    bool &is_selected_;
    Size size_;
};

struct FontConfig
{
    const uint8_t * font_data;
    size_t font_data_size;
    float pixel_size;
    float x_extra_space = 0;
};

class FontText
{
public:
    FontText(FontConfig config, const char* text, Position position = {});
    FontText(FontText const& rhs);
    ~FontText();
    void draw() const;

private:
    struct impl;
    std::unique_ptr<impl> pimpl_;
};

class TextField
{
public:
    TextField(const char* text, Position position = {}) : text_{text}, position_{position} {};
    void draw() const;

private:
    const char * text_;
    Position position_;
};

class InputInteger
{
public:
    InputInteger(const char* text, int &value, int min = 0, int max = INT_MAX, Position position = {}) : 
        text_{text}, value_{value}, min_{min}, max_{max}, position_{position} 
    {};
    void draw() const;

private:
    const char * text_;
    int &value_;
    int min_;
    int max_;
    Position position_;
};

class InputDouble
{
public:
    InputDouble(const char* text, double &value, Size size = {}) : text_{text}, value_{value}, size_{size} {};
    void draw() const;

private:
    const char * text_;
    double &value_;
    Size size_;
};

class ComboBoxString
{
public:
    ComboBoxString(const char* text, std::vector<std::string> &list, std::string &current, Size size = {}) : 
        text_{text}, list_{list}, current_{current}, size_{size}
    {};
    void draw();

private:
    const char * text_;
    std::vector<std::string> &list_;
    std::string &current_;
    Size size_;
};

class ImageFile
{
public:
    ImageFile(const char *path, bool *show = nullptr);
    ImageFile(ImageFile const& rhs);
    ~ImageFile();
    void draw();

private:
    struct impl;
    std::unique_ptr<impl> pimpl_;
};

class ImageResource
{
public:
    ImageResource(const uint8_t *data, size_t size, bool *show = nullptr);
    ImageResource(ImageResource const& rhs);
    ~ImageResource();
    void draw();

private:
    struct impl;
    std::unique_ptr<impl> pimpl_;
};

class HBox
{
public:
    void draw() const;
    HBox& add(Widget &&w) {
        widgets_.emplace_back(std::forward<Widget>(w));
        return *this;
    }

private:
    std::vector<Widget> widgets_;
};

class Stack
{
public:
    void draw() const;
    Stack& add(Widget &&w) {
        widgets_.emplace_back(std::forward<Widget>(w));
        return *this;
    }

private:
    std::vector<Widget> widgets_;
};

class TabItem
{
public:
    TabItem(const char *text) : text_{text}
    {}
    void draw() const;
    TabItem& add(Widget &&w) {
        widgets_.emplace_back(std::forward<Widget>(w));
        return *this;
    }

private:
    const char *text_;
    std::vector<Widget> widgets_;
};

class TabBar
{
public:
    void draw() const;
    TabBar& add(Widget &&w) {
        widgets_.emplace_back(std::forward<Widget>(w));
        return *this;
    }

private:
    std::vector<Widget> widgets_;
};

class Window
{
public:
    Window(const char* text, Size size = {}, Position position = {}) : text_{text}, size_{size}, position_{position} {};
    void draw() const;
    Window& add(Widget &&w) {
        widgets_.emplace_back(std::forward<Widget>(w));
        return *this;
    }

private:
    const char * text_;
    Size size_;
    Position position_;
    std::vector<Widget> widgets_;
};

class LogWindow
{
public:
    LogWindow(const char* text, Size size = {}, Position position = {});
    ~LogWindow();
    LogWindow(LogWindow const& rhs);
    void draw() const;
    void add_log(const char *text);

private:
    struct impl;
    std::unique_ptr<impl> pimpl_;
};

class MemoryEditorWindow
{
public:
    MemoryEditorWindow(const char* text, uint8_t *bytes, size_t bytes_size, Size size = {}, Position position = {});
    ~MemoryEditorWindow();
    MemoryEditorWindow(MemoryEditorWindow const& rhs);
    void draw() const;

private:
    struct impl;
    std::unique_ptr<impl> pimpl_;
};

class SectorMemoryEditorWindow
{
public:
    SectorMemoryEditorWindow(const char* text, std::vector<std::vector<uint8_t>> &sectors, int &current_sector, Size size = {}, Position position = {});
    ~SectorMemoryEditorWindow();
    SectorMemoryEditorWindow(SectorMemoryEditorWindow const& rhs);
    void draw() const;

private:
    struct impl;
    std::unique_ptr<impl> pimpl_;
};

class Application
{
public:
    Application(Size size, const char *title, Size log_size = Size{500,400}, Position log_position = {}) : 
        size_{size}, 
        title_(title), 
        log_{"Log", log_size, log_position}
    {}

    void init(const uint8_t *font_data = nullptr, size_t font_data_size = 0, float font_size = 13.0) {
        ctx_ = backend_init(size_.width, size_.height, title_, font_data, font_data_size, font_size);
    }

    ~Application()
    {
        backend_teardown(ctx_);
    }

    template <typename... Args>
    void info(Args&&... args)
    {
        log("info", std::forward<Args>(args)...);
    }

    template <typename... Args>
    void warn(Args&&... args)
    {
        log("warning", std::forward<Args>(args)...);
    }

    template <typename... Args>
    void error(Args&&... args)
    {
        log("error", std::forward<Args>(args)...);
    }

    void run()
    {
        while (!backend_should_close(ctx_))
        {
            guicpp::backend_set_frame(ctx_);
            for (auto &w : widgets_) {
                w.draw();
            }
            log_.draw();
            guicpp::backend_render(ctx_);
        }
    }

    Application& add(Widget &&w) {
        widgets_.emplace_back(std::forward<Widget>(w));
        return *this;
    }

private:
    Size size_;
    const char *title_;
    LogWindow log_;
    BackendContext ctx_;
    std::vector<Widget> widgets_;

    template <typename... Args>
    void log(std::string_view tag, std::string_view format_str, Args&&... args)
    {
        auto now = std::chrono::system_clock::now();
        auto result = fmt::format("{:%Y-%m-%d %H:%M:%S} {}: {}\n", now, tag, fmt::format(format_str, std::forward<Args>(args)...));
        log_.add_log(result.c_str());
    }
};

}
