#include <fstream>

#include "stb_image.h"
#include "json.hpp"

void convertImage(const std::string &filename, FILE *fout) {
  // https://learnopengl.com/Getting-started/Textures
  // OpenGL expects the 0.0 coordinate on the y-axis to be on the bottom side of the image,
  // but images usually have 0.0 at the top of the y-axis.
  stbi_set_flip_vertically_on_load(true);

  int width, height;
  stbi_uc *data = stbi_load(filename.c_str(), &width, &height, nullptr, 4);

  std::string name = filename.substr(0, filename.find('.'));
  std::transform(name.begin(), name.end(), name.begin(), toupper);

  fprintf(fout, "static constexpr int %s_WIDTH = %d;\n", name.c_str(), width);
  fprintf(fout, "static constexpr int %s_HEIGHT = %d;\n", name.c_str(), height);
  fprintf(fout, "static const unsigned %s_DATA[] = {", name.c_str());
  for (int y = 0; y < height; y++) {
    fprintf(fout, "\n//%d\n", y);
    for (int x = 0; x < width; x++) {
      int i = x + y * width;
      auto c = reinterpret_cast<unsigned *>(data + 4 * i);
      fprintf(fout, "0x%08x,", *c);
    }
  }
  fprintf(fout, "\n};\n\n");

  stbi_image_free(data);
}

int main(int argc, const char *argv[]) {
  if (argc != 3) return -1;

  std::ifstream fin(argv[1]);
  nlohmann::json input = nlohmann::json::parse(fin);
  if (input.type() != nlohmann::json::value_t::array) return -2;

  FILE *fout = nullptr;
  if (fopen_s(&fout, argv[2], "w")) return -3;

  fprintf(fout, "// AUTOMATICALLY GENERATED\n\n");

  for (auto &[_, image] : input.items()) {
    if (image.type() != nlohmann::json::value_t::string) continue;
    convertImage(image.get<std::string>(), fout);
  }

  fclose(fout);

  return 0;
}
