Import("env")
from subprocess import call
import os

def runCommonChecks(source,target, env):
    print("Started cppcheck...\n")
    call(["cppcheck", os.getcwd()+"/src", "--enable=all"])
    print("Finished cppcheck...\n")

env.AddPreAction("buildprog", runCommonChecks)
