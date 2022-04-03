#include <Model.hpp>
#include <ModelRenderer.hpp>

int main(int argc, char **argv) {

    Model model("ExampleModel3_copy.mod");

    ModelRenderer &renderer = ModelRenderer::getInstance();
    renderer.renderModel(&model);

    return 0;
}
