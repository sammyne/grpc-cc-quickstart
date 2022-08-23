#include "xiangminli/os.h"

#include <cstdio>

namespace xiangminli::os {

int ReadFile(std::string &out, const char *path) {
  auto f = fopen(path, "r");
  if (!f) {
    return -1;
  }

  fseek(f, 0, SEEK_END);
  auto file_size = ftell(f);
  rewind(f);

  out.resize(file_size);
  if (auto n = fread(out.data(), 1, file_size, f); n != file_size) {
    fclose(f);
    return -2;
  }

  fclose(f);

  return 0;
}

}  // namespace xiangminli::os