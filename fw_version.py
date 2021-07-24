import subprocess

Import("env")

def get_firmware_specifier_build_flag():
    ret = subprocess.run(["git", "rev-parse", "--short", "HEAD"], stdout=subprocess.PIPE, text=True)
    build_version = ret.stdout.strip()
    build_flag = "-D FW_COMMIT=\\\"" + build_version + "\\\""
    print ("Firmware Revision: " + build_version)
    return (build_flag)

env.Append(
    BUILD_FLAGS=[get_firmware_specifier_build_flag()]
)
