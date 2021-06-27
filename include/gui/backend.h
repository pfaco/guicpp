#pragma once
#include <memory>

namespace guicpp
{
    struct BackendContext {
        void *window;
    };

    struct BackendTexture {
        unsigned int id;
    };

    auto backend_init(int width, int height, const char *title, const uint8_t *font_data = nullptr, size_t font_data_size = 0, float font_size = 13.0) -> BackendContext;
    bool backend_should_close(BackendContext ctx);
    void backend_set_frame(BackendContext ctx);
    void backend_render(BackendContext ctx);
    void backend_teardown(BackendContext ctx);

    auto backend_load_texture(const char *path, int *width, int *height) -> BackendTexture;
    auto backend_load_texture(const uint8_t *data, size_t size, int *width, int *height) -> BackendTexture;
}
