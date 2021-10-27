cc_library(
    name = "tempController",
    srcs = glob(["*.cpp"]),
    hdrs = glob(["*.h"]),
    strip_include_prefix = "",
    visibility = ["//visibility:public"],
    deps = [
        "//lib/communications:command",
        "//lib/GpioExpander:gpio",
        "//lib/INA219:ina219",
    ],
)