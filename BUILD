# https://docs.bazel.build/versions/master/be/c-cpp.html#cc_binary
cc_binary(
  name = "main",
  # srcs = glob(["src/*.cpp", "src/*.h"]),
  srcs = ["src/main.cpp"],
  copts = [],
  deps = ["//test:test", "//lib/Scheduler:mock", "//lib/INA219:mock"],
  includes = []
)
