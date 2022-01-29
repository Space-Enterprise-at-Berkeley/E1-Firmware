import subprocess
import os
from datetime import datetime

Import("env")

def get_firmware_specifier_build_flag():
    ret = subprocess.run(["git", "rev-parse", "--short", "HEAD"], stdout=subprocess.PIPE, text=True)
    build_version = ret.stdout.strip()
    build_flag = "-D FW_COMMIT=\\\"" + build_version + "\\\""
    print ("Firmware Revision: " + build_version)
    return (build_flag)

# def get_username_build_flag():
#     name = os.getlogin()
#     build_flag = "-D FW_USERNAME=\\\"" + name + "\\\""
#     print("Username: " + name)
#     return (build_flag)

# def get_datetime_build_flag():
#     # date = ""
#     # date += ("0" + str(datetime.now().month))[-2:]
#     # date += ("0" + str(datetime.now().day))[-2:]
#     # date += ("0" + str(datetime.now().year))[-2:]
#     # date += ("0" + str(datetime.now().hour))[-2:]
#     # date += ("0" + str(datetime.now().minute))[-2:]
#     # date += ("0" + str(datetime.now().second))[-2:]
#     date = datetime.now().isoformat()
#     build_flag = "-D FW_BUILD_DATE=\\\"" + date + "\\\""
#     print("Build date: " + date)
#     return (build_flag)

# def get_project_name_build_flag():
#     pname = env["PIOENV"]
#     build_flag = "-D FW_PROJECT=\\\"" + pname + "\\\""
#     print("Firmware Project: " + pname)
#     return (build_flag)

env.Append(
    BUILD_FLAGS=[get_firmware_specifier_build_flag()]
)