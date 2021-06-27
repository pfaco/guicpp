#include <gui/gui.h>
#include "imgui.h"
#include "imgui_memory_editor.h"

namespace guicpp
{

void Label::draw() const
{
    ImGui::Text("%s", text_);
}

void LabelString::draw() const
{
    ImGui::Text(text_.c_str());
}

void Separator::draw() const
{
    ImGui::Separator();
}

void FrameRateLabel::draw() const
{
    ImGui::Text("Application average %.1f FPS", ImGui::GetIO().Framerate);
}

void OnClickButton::draw() const
{
    if (ImGui::Button(text_)) {
        onclick_();
    }
}

void StateButton::draw() const
{
    ImGui::Button(text_);
    if (ImGui::IsItemActive()) {
        is_pressed_ = true;
    } else {
        is_pressed_ = false;
    }
}

void CheckBox::draw() const
{
    ImGui::Checkbox(text_, &is_selected_);
}

struct FontText::impl
{
    impl(ImFont *font, const char* text, Position position = {}) :
        font_{font},
        text_{text},
        position_{position}
    {}

    void draw() {
        ImGui::PushFont(font_);
        ImGui::SetCursorPos(ImVec2(position_.x, position_.y));
        ImGui::Text("%s", text_);
        ImGui::PopFont();
    }

    ImFont *font_;
    const char* text_;
    Position position_;
};

FontText::FontText(FontConfig config, const char* text, Position position)
{
    ImFontConfig fontConfig;
    fontConfig.FontDataOwnedByAtlas = false;
    fontConfig.GlyphExtraSpacing = {config.x_extra_space, 0};
    pimpl_ = std::make_unique<impl>(ImGui::GetIO().Fonts->AddFontFromMemoryTTF(reinterpret_cast<void*>(const_cast<uint8_t*>(config.font_data)), config.font_data_size, config.pixel_size, &fontConfig), text, position);
}

FontText::FontText(FontText const& rhs) :
    pimpl_{std::make_unique<impl>(rhs.pimpl_->font_, rhs.pimpl_->text_, rhs.pimpl_->position_)}
{}

FontText::~FontText() = default;

void FontText::draw() const {
    pimpl_->draw();
}

void TextField::draw() const
{
    auto pos = ImGui::GetCursorPos();
    ImGui::SetCursorPos(ImVec2{(float)position_.x + pos.x, (float)position_.y + pos.y});
    ImGui::Text("%s", text_);
}

void InputInteger::draw() const
{
    auto pos = ImGui::GetCursorPos();
    ImGui::SetCursorPos(ImVec2{(float)position_.x + pos.x, (float)position_.y + pos.y});
    ImGui::InputInt(text_, &value_);
}

void InputDouble::draw() const
{
    ImGui::InputDouble(text_, &value_, 0, 0, "%07.3f");
}

void ComboBoxString::draw()
{
    if (ImGui::BeginCombo(text_, current_.c_str(), 0)) {
        for (auto &port : list_)
        {
            bool is_selected = (current_ == port);
            if (ImGui::Selectable(port.c_str(), is_selected)) {
                current_ = port;
            }
            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
  }
}

struct ImageFile::impl
{
    impl(const char  *path, bool *show = nullptr) :
        path_{path},
        show_{show},
        texture_{backend_load_texture(path_, &width_, &height_)}
    {}

    void draw() {
        if (show_ == nullptr || *show_) {
            ImGui::Image((void*)(intptr_t)texture_.id, ImVec2(width_, height_));
        }
    }

    const char *path_;
    bool *show_;
    BackendTexture texture_;
    int width_;
    int height_;
};

ImageFile::ImageFile(const char *path, bool *show) :
    pimpl_{std::make_unique<impl>(path, show)}
{}

ImageFile::ImageFile(ImageFile const& rhs) :
    pimpl_{std::make_unique<impl>(rhs.pimpl_->path_, rhs.pimpl_->show_)}
{}

ImageFile::~ImageFile() = default;

void ImageFile::draw() {
    pimpl_->draw();
}

struct ImageResource::impl
{
    impl(const uint8_t *data, size_t size, bool *show = nullptr) :
        data_{data},
        size_{size},
        show_{show},
        texture_{backend_load_texture(data_, size_, &width_, &height_)}
    {}

    void draw() {
        if (show_ == nullptr || *show_) {
            ImGui::Image((void*)(intptr_t)texture_.id, ImVec2(width_, height_));
        }
    }

    const uint8_t *data_;
    size_t size_;
    bool *show_;
    BackendTexture texture_;
    int width_;
    int height_;
};

ImageResource::ImageResource(const uint8_t *data, size_t size, bool *show) :
    pimpl_{std::make_unique<impl>(data, size, show)}
{}

ImageResource::ImageResource(ImageResource const& rhs) :
    pimpl_{std::make_unique<impl>(rhs.pimpl_->data_, rhs.pimpl_->size_, rhs.pimpl_->show_)}
{}

ImageResource::~ImageResource() = default;

void ImageResource::draw() {
    pimpl_->draw();
}

void HBox::draw() const
{
    bool first = true;
    for (auto &w : widgets_) {
        if (!first) { ImGui::SameLine(); }
        first = false;
        w.draw();
    }
}

void Stack::draw() const
{
    bool first = true;
    ImVec2 pos = ImGui::GetCursorPos();
    for (auto &w : widgets_) {
        if (!first) { ImGui::SetCursorPos(pos); }
        first = false;
        w.draw();
    }
}

void TabItem::draw() const
{
    if (ImGui::BeginTabItem(text_)) {
        for (auto &w : widgets_) {
            w.draw();
        }
        ImGui::EndTabItem();
    }
}

void TabBar::draw() const
{
    if (ImGui::BeginTabBar("")) {
        for (auto &w : widgets_) {
            w.draw();
        }
        ImGui::EndTabBar();
    }
}

void Window::draw() const
{
    ImGui::SetNextWindowSize(ImVec2(size_.width, size_.height), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(position_.x, position_.y), ImGuiCond_FirstUseEver);
    ImGui::Begin(text_);
    for (auto &w : widgets_) {
        w.draw();
    }
    ImGui::End();
}

struct LogWindow::impl
{
    impl(const char * text, Size size = {}, Position position = {}) : 
        text_{text}, size_{size}, position_{position}
    {}

    void clear() { 
        text_buffer.clear(); 
        lines_offsets.clear();
    }

    void add_log(const char* str)
    {
        int old_size = text_buffer.size();
        text_buffer.append(str);
        for (int new_size = text_buffer.size(); old_size < new_size; old_size++) {
            if (text_buffer[old_size] == '\n') {
                lines_offsets.push_back(old_size);
            }
        }
        scroll_to_bottom = true;
    }

    void draw()
    {
        ImGui::SetNextWindowSize(ImVec2(size_.width, size_.height), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(position_.x, position_.y), ImGuiCond_FirstUseEver);
        ImGui::Begin(text_);
        if (ImGui::Button("Clear")) {
            clear();
        }
        ImGui::SameLine();
        bool copy = ImGui::Button("Copy");
        ImGui::SameLine();
        filter.Draw("Filter", -100.0f);
        ImGui::Separator();
        ImGui::BeginChild("scrolling");
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,1));
        if (copy) {
            ImGui::LogToClipboard();
        }

        if (filter.IsActive())
        {
            const char* buf_begin = text_buffer.begin();
            const char* line = buf_begin;
            for (int line_no = 0; line != NULL; line_no++)
            {
                const char* line_end = (line_no < lines_offsets.Size) ? buf_begin + lines_offsets[line_no] : NULL;
                if (filter.PassFilter(line, line_end)) {
                    ImGui::TextUnformatted(line, line_end);
                }
                line = line_end && line_end[1] ? line_end + 1 : NULL;
            }
        }
        else
        {
            ImGui::TextUnformatted(text_buffer.begin());
        }

        if (scroll_to_bottom) {
            ImGui::SetScrollHereY(1.0f);
        }
        scroll_to_bottom = false;
        ImGui::PopStyleVar();
        ImGui::EndChild();
        ImGui::End();
    }


    const char *text_;
    Size size_;
    Position position_;
private:
    ImGuiTextBuffer text_buffer;
    ImGuiTextFilter filter;
    ImVector<int> lines_offsets;
    bool scroll_to_bottom;
};

LogWindow::LogWindow(const char *text, Size size, Position position) :
    pimpl_{std::make_unique<impl>(text, size, position)}
{}

LogWindow::~LogWindow() = default;

LogWindow::LogWindow(LogWindow const& rhs) :
    pimpl_(std::make_unique<impl>(rhs.pimpl_->text_, rhs.pimpl_->size_, rhs.pimpl_->position_))
{}

void LogWindow::draw() const
{
    pimpl_->draw();
}

void LogWindow::add_log(const char *text)
{
    pimpl_->add_log(text);
}

struct MemoryEditorWindow::impl
{
    const char *text_;
    uint8_t *bytes_;
    size_t bytes_size_;
    Size size_;
    Position position_;
    MemoryEditor memory_editor;

    impl(const char* text, uint8_t *bytes, size_t bytes_size, Size size = {}, Position position = {}) :
        text_{text},
        bytes_{bytes},
        bytes_size_{bytes_size},
        size_{size},
        position_{position}
    {}

    void draw() {
        ImGui::SetNextWindowSize(ImVec2(size_.width, size_.height), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(position_.x, position_.y), ImGuiCond_FirstUseEver);
        memory_editor.DrawWindow(text_, bytes_, bytes_size_);
    }

};

MemoryEditorWindow::MemoryEditorWindow(const char* text, uint8_t *bytes, size_t bytes_size, Size size, Position position) :
    pimpl_{std::make_unique<impl>(text, bytes, bytes_size, size, position)}
{}

MemoryEditorWindow::~MemoryEditorWindow() = default;

MemoryEditorWindow::MemoryEditorWindow(MemoryEditorWindow const& rhs) :
    pimpl_(std::make_unique<impl>(rhs.pimpl_->text_, rhs.pimpl_->bytes_, rhs.pimpl_->bytes_size_, rhs.pimpl_->size_, rhs.pimpl_->position_))
{}

void MemoryEditorWindow::draw() const
{
    pimpl_->draw();
}

struct SectorMemoryEditorWindow::impl
{
    const char *text_;
    std::vector<std::vector<uint8_t>> &sectors_;
    int &current_sector_;
    Size size_;
    Position position_;
    MemoryEditor memory_editor;
    std::string full_text;

    impl(const char* text, std::vector<std::vector<uint8_t>> &sectors, int &current_sector, Size size = {}, Position position = {}) :
        text_{text},
        sectors_{sectors},
        current_sector_{current_sector},
        size_{size},
        position_{position}
    {}

    void draw() {
        if (current_sector_ < sectors_.size()) {
            ImGui::SetNextWindowSize(ImVec2(size_.width, size_.height), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowPos(ImVec2(position_.x, position_.y), ImGuiCond_FirstUseEver);
            memory_editor.DrawWindow(text_, sectors_[current_sector_].data(), sectors_[current_sector_].size());
        }
    }

};

SectorMemoryEditorWindow::SectorMemoryEditorWindow(const char* text, std::vector<std::vector<uint8_t>> &sectors, int &current_sector, Size size, Position position) :
    pimpl_{std::make_unique<impl>(text, sectors, current_sector, size, position)}
{}

SectorMemoryEditorWindow::~SectorMemoryEditorWindow() = default;

SectorMemoryEditorWindow::SectorMemoryEditorWindow(SectorMemoryEditorWindow const& rhs) :
    pimpl_(std::make_unique<impl>(rhs.pimpl_->text_, rhs.pimpl_->sectors_, rhs.pimpl_->current_sector_, rhs.pimpl_->size_, rhs.pimpl_->position_))
{}

void SectorMemoryEditorWindow::draw() const
{
    pimpl_->draw();
}

}